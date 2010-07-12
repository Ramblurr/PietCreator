/*
    Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
    Copyright (C) 2004-2010 Erik Schoenfelder <schoenfr@web.de>


    npiet is an interperter for the piet programming language.

    about the piet programming language see:

          http://www.dangermouse.net/esoteric/piet.html

    more about the piet programming language and the npiet interpreter see:
         http://www.bertnase.de/npiet/

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "npiet.h"
#include "npiet_utils.h"

#include "config.h"

#include <gd.h>
#include <gdfonts.h>
#include <gdfontt.h>

int *cells = 0;
int width = 0, height = 0;

int p_dir_pointer = 0;              /* DP: p_{left, right, up, down} */
int p_codel_chooser = 0;            /* CC: p_left or p_right */
int p_xpos = 0;                     /* execution position */
int p_ypos = 0;


/**** OPTIONS ****/
/* maximum number of execution steps (0 == unlimited): */
unsigned max_exec_step = 0;

/* be somewhat verbose: */
int verbose = 0;

/* be quiet - actually only suppresses the prompt when doing input: */
int quiet = 0;

/* show program execution information: */
int trace = 0;

/* print debugging stuff: */
int debug = 0;

/* work around broken source (wrong assumption about toggle of dp and cc): */
int toggle_bug = 0;

/* fall back to npiet v1.1 behavior (fixed white codel crossing but
 * without trace info:
 */
int version_11 = 0;

/*** GD opts ****/
/* with gd2 lib linked we try to save trace output: */
int do_gdtrace = 0;
char *gd_trace_filename = "npiet-trace.png";
int gd_trace_simple = 0;
unsigned gd_trace_start = 0;
unsigned gd_trace_end = 1 << 31;                /* lot's to print */


#define p_left                  'l'
#define p_right                 'r'
#define p_up                    'u'
#define p_down                  'd'

#define toggle_cc(cc)   ((cc) == 'r' ? 'l' : 'r')

#define turn_dp(dp)     ((dp) == 'r' ? 'd' : ((dp) == 'd' ? 'l' : \
                         ((dp) == 'l' ? 'u' : 'r')))
#define turn_dp_inv(dp) ((dp) == 'r' ? 'u' : ((dp) == 'u' ? 'l' : \
                         ((dp) == 'l' ? 'd' : 'r')))
#define dp_dx(dp)       ((dp) == 'l' ? -1 : ((dp) == 'r' ? 1 : 0))
#define dp_dy(dp)       ((dp) == 'u' ? -1 : ((dp) == 'd' ? 1 : 0))

/* helper: */
#define dprintf         if (debug) printf
#define d2printf        if (debug > 1) printf
#define tprintf         if (trace \
                            && exec_step >= gd_trace_start \
                            && exec_step <= gd_trace_end) printf
#define t2printf        if (trace > 1) printf
#define vprintf         if (verbose) printf

/* informal step counter: */
unsigned exec_step = 0;

/*
 * stack space for runtime action:
 */
long *stack = 0;                /* stack space */
int num_stack = 0;              /* current number of values on stack */
int max_stack = 0;              /* max size of stack allocated */

void
alloc_stack_space (int val)
{
  if (val <= max_stack) {
    return;
  } else if (! stack) {
    max_stack = val;
    stack = (long *) calloc (val, sizeof (long));
  } else {
    long *new_stack = (long *) calloc (val, sizeof (long));
    memcpy (new_stack, stack, num_stack * sizeof (long));
    free (stack);
    max_stack = val;
    stack = new_stack;
  }
  dprintf ("deb: stack extended to %d entries (num_stack is %d)\n",
           max_stack, num_stack);
}


void
tdump_stack ()
{
  int i;

  if (num_stack == 0) {
    tprintf ("trace: stack is empty");
  } else {
    tprintf ("trace: stack (%d values):", num_stack);
  }
  for (i = 0; i < num_stack; i++) {
    tprintf (" %ld", stack [num_stack - i - 1]);
  }
  tprintf ("\n");
}

int set_image(int* _cells, int w, int h)
{
    cells = _cells;
    height = h;
    width = w;
}


int
piet_run ()
{
  if (width <= 0 || height <= 0) {
    fprintf (stderr, "nothing to execute...%s %s\n", width, height);
    return -1;
  }

  piet_init ();

  while (1) {

    t2printf ("trace:  pos=%d,%d dp=%c cc=%c\n",
              p_xpos, p_ypos, p_dir_pointer, p_codel_chooser);

    if (piet_step () < 0) {
      vprintf ("\ninfo: program end\n");
      break;
    }

    /*TODO-GDTRACEif (do_gdtrace && trace) {
      /*
       * in case of additional tracing, make sure we always have
       * an up-to-date picture; it's way expensive, so it may be
       * get an extra option...
       */
      //gd_save ();
    //}
  }

  return 0;
}

void
piet_init ()
{
  p_dir_pointer = p_right;
  p_codel_chooser = p_left;
  p_xpos = p_ypos = 0;

  /* init anyway: */
  exec_step = 0;
}

int
piet_step ()
{
  int rc, tries, n_x, n_y, a_x, a_y, pre_dp, pre_cc;
  int pre_xpos, pre_ypos;
  int c_col, a_col, num_cells;
  char msg [128];
  // a noop from a white codel:
  int white_crossed = 0;
  // flag about white to white crossing:
  int in_white = 0;

  /* hmm - maybe we can simulate other piet dialect this way: */
  static int p_toggle = 0;

  if (max_exec_step > 0 && exec_step >= max_exec_step) {
    fprintf (stderr, "error: configured execution steps exceeded (%d steps)\n",
             exec_step);
    return -1;
  }


  /* current cell col_idx: */
  c_col = get_cell (p_xpos, p_ypos);

  /*
   * toggle cc first, then alternate with dp, because so say the spec:
   *
   *    Black Blocks and Edges
   *
   *    Black colour blocks and the edges of the program restrict program
   *    flow. If the Piet interpreter attempts to move into a black block or
   *    off an edge, it is stopped and the CC is toggled. The interpreter then
   *    attempts to move from its current block again. If it fails a second
   *    time, the DP is moved clockwise one step. These attempts are repeated,
   *    with the CC and DP being changed between alternate attempts. If after
   *    eight attempts the interpreter cannot leave its current colour block,
   *    there is no way out and the program terminates.
   *
   */
  if (! toggle_bug) {
    p_toggle = 0;
  }

  white_crossed = (c_col == c_white);

  /* save for trace output: */
  pre_xpos = p_xpos;
  pre_ypos = p_ypos;
  pre_dp = p_dir_pointer;
  pre_cc = p_codel_chooser;

  /*TODO-GDTRACE if (do_gdtrace) {
    gd_try_init ();
  }*/

  if (c_col == c_black) {
    /* we are lost in a black hole: */
    tprintf ("trace: special case: we started at a black cell - exiting...\n");
    return -1;
  }

  /*
   * now try to find a way to continue:
   */
  for (tries = 0; tries < 8; tries++) {

    n_x = p_xpos;
    n_y = p_ypos;

    if (c_col == c_white) {

      /* head on: */
      if (tries == 0) {
        tprintf ("trace: special case: we at a white codel"
                 " - continuing\n");
      }
      num_cells = 1;
    } else {
      /* find dp/cc edge and codel: */
      piet_walk_border (&n_x, &n_y, &num_cells);
    }

    /* find adjacent cell to border and dir: */
    a_x = n_x + dp_dx (p_dir_pointer);
    a_y = n_y + dp_dy (p_dir_pointer);
    a_col = get_cell (a_x, a_y);

    dprintf ("deb: try %d: testing cell %d, %d (col_idx %d) "
             "with dp='%c', cc='%c'\n",
             tries, a_x, a_y, a_col, p_dir_pointer, p_codel_chooser);

    /*TODO-GDTRACEif (do_gdtrace && ! gd_trace_simple
        && exec_step >= gd_trace_start && exec_step <= gd_trace_end) {
      gd_try_step (exec_step, tries, n_x, n_y,
                   p_dir_pointer, p_codel_chooser);
    }*/

    /*
     * a white cell is passed without any command:
     *
     *   White Blocks
     *
     *    White colour blocks are "free" zones through which the
     *    interpreter passes unhindered. If it moves from a colour
     *    block into a white area, the interpreter "slides" through
     *    the white codels in the direction of the DP until it reaches
     *    a non-white colour block. If the interpreter slides into a
     *    black block or an edge, it is considered restricted (see
     *    above), otherwise it moves into the colour block so
     *    encountered.  Sliding across white blocks does not cause a
     *    command to be executed (see below).
     *
     *    [...]
     *    If the transition between colour blocks occurs via a slide
     *    across a white block, no command is executed.
     */
    if (a_col == c_white) {
      while (a_col == c_white) {
        dprintf ("deb: white cell passed to %d, %d (now col_idx %d)\n",
                 a_x, a_y, a_col);
        a_x += dp_dx (p_dir_pointer);
        a_y += dp_dy (p_dir_pointer);
        a_col = get_cell (a_x, a_y);
      }

      if (a_col >= 0 && a_col != c_black) {
        /* a valid cell - continue without action: */
        tprintf ("trace: white cell(s) crossed - continuing with no command "
                 "at %d,%d...\n", a_x, a_y);
        white_crossed = 1;
      } else {
        /*
         * When sliding into a black block or over the edge of the world,
         * the Perl Piet interpreter sets the white block as the current
         * block. The Tower of Hanoi example relies on this behaviour.
         */
        if (version_11) {
          /*
           * patch from Yusuke ENDOH <mame@tsg.ne.jp>
           *
           * ``According to `Clarification of white block behaviour
           *   (added 25 January, 2008)' in the Piet specification [1],
           *   when sliding into a black block, the interpreter must
           *   not stay in the coloured block but move to the white
           *   block. But the current behaviour of npiet is `stay'.''
           */
          int *visited = NULL, visited_len = 0, i;
          white_crossed = 1;
          while (a_col < 0 || a_col == c_black) {
            a_col = c_white;
            a_x -= dp_dx (p_dir_pointer);
            a_y -= dp_dy (p_dir_pointer);
            tprintf("trace: hitting black block when sliding at %d,%d %c %c\n",
                    a_x, a_y, p_codel_chooser, p_dir_pointer);

            p_codel_chooser = toggle_cc(p_codel_chooser);
            p_dir_pointer = turn_dp(p_dir_pointer);

            for (i = 0; i < visited_len; i++) {
              if (visited[i * 4 + 0] == a_x &&
                  visited[i * 4 + 1] == a_y &&
                  visited[i * 4 + 2] == p_codel_chooser &&
                  visited[i * 4 + 3] == p_dir_pointer) {
                return -1;
              }
            }

            visited = realloc(visited, 4 * (visited_len + 1) * sizeof(int));
            visited[i * 4 + 0] = a_x;
            visited[i * 4 + 1] = a_y;
            visited[i * 4 + 2] = p_codel_chooser;
            visited[i * 4 + 3] = p_dir_pointer;
            visited_len++;

            while (a_col == c_white) {
              dprintf ("deb: white cell passed to %d, %d (now col_idx %d)\n",
                       a_x, a_y, a_col);
              a_x += dp_dx (p_dir_pointer);
              a_y += dp_dy (p_dir_pointer);
              a_col = get_cell (a_x, a_y);
            }
          }
          if (visited) free(visited);
        } else {
          white_crossed = 1;
          a_col = c_white;
          a_x -= dp_dx (p_dir_pointer);
          a_y -= dp_dy (p_dir_pointer);
          tprintf("trace: entering white block at %d,%d (like the perl "
                  "interpreter would)...\n", a_x, a_y);
        }
      }
    }

    if (a_col < 0 || a_col == c_black) {
      /*
       * we hit something black or a wall:
       */
      if (c_col == c_white || in_white) {
        // toggle dp and cc:
        p_codel_chooser = toggle_cc(p_codel_chooser);
        p_dir_pointer = turn_dp(p_dir_pointer);
        dprintf ("deb: in white codel - toggle both dp and cc\n");
        dprintf ("deb: toggle cc to '%c'\n", p_codel_chooser);
        dprintf ("deb: toggle dp to '%c'\n", p_dir_pointer);
      } else {
        if ((p_toggle % 2) == 0) {
          p_codel_chooser = toggle_cc(p_codel_chooser);
          dprintf ("deb: toggle cc to '%c'\n", p_codel_chooser);
        } else {
          p_dir_pointer = turn_dp(p_dir_pointer);
          dprintf ("deb: toggle dp to '%c'\n", p_dir_pointer);
        }
      }
      p_toggle++;

    } else {
      tprintf ("\ntrace: step %d  (%d,%d/%c,%c %s -> %d,%d/%c,%c %s):\n",
               exec_step, p_xpos, p_ypos, pre_dp, pre_cc,
               cell2str (get_cell (p_xpos, p_ypos)),
               a_x, a_y, p_dir_pointer, p_codel_chooser,
               cell2str (get_cell (a_x, a_y)));

      exec_step++;

      if (white_crossed) {
        /* no command is executed - anything is fine: */

        /*TODO-GDTRACE if (gd_trace_simple) {
          strcpy (msg, "no");
        } else {*/
          strcpy (msg, "noop");
        /*}*/
        t2printf ("action: none\n");

        rc = 0;
      } else {
        /* make a program step: */
        rc = piet_action (c_col, a_col, num_cells, msg);
      }

      /*TODO-GDTRACEif (do_gdtrace
          && exec_step >= gd_trace_start && exec_step <= gd_trace_end) {
        /* graphical trace output: * /
        gd_action (pre_xpos, pre_ypos, n_x, n_y, a_x, a_y, msg);
      }*/

      if (rc < 0) {
        /* we had an error: */
        return -1;
      }

      t2printf ("step done: continuing at %d,%d...\n", a_x, a_y);
      p_xpos = a_x;
      p_ypos = a_y;

      return 0;
    }
  }

  /* tries exausted, no way to step on: */
  return -1;
}

int
piet_walk_border_do (int *n_x, int *n_y, int *num_cells)
{
  int rc, c_idx;

  dprintf("deb: n_x:%d n_y:%d num:%d\n", *n_x, *n_y, *num_cells );

  /* store current color index: */
  c_idx = get_cell (p_xpos, p_ypos);

  /* count connected cells found: */
  *num_cells = 0;

  /* we fill the area with another color and check the border: */
  rc = check_connected_cell (p_xpos, p_ypos, c_idx, c_mark_index,
                             n_x, n_y, num_cells);

  dprintf ("DEB: after check: rc is %d (num_cells = %d)\n", rc, *num_cells);

  if (rc >= 0) {
    /* reset: */
    reset_check_connected_cell (p_xpos, p_ypos, c_idx, c_mark_index);
  }

  /*TODO-DEBUGif (debug) {
    dump_cells ();
  }*/

  return rc;
}

int
piet_walk_border (int *n_x, int *n_y, int *num_cells)
{
  int rc;

  dprintf ("info: walk_border 1: n_x=%d, n_y=%d, n_dp=%c, n_cc=%c\n",
            *n_x, *n_y, p_dir_pointer, p_codel_chooser);

  rc = piet_walk_border_do (n_x, n_y, num_cells);

  if (rc < 0) {
    fprintf (stderr, "internal error... - exiting\n");
    exit (-99);
  }

  dprintf ("info: walk_border 2: n_x=%d, n_y=%d, n_dp=%c, n_cc=%c\n",
            *n_x, *n_y, p_dir_pointer, p_codel_chooser);

  return 0;
}

int
piet_action (int c_col, int a_col, int num_cells, char *msg)
{
  int hue_change, light_change;

  hue_change = ((get_hue (a_col) - get_hue (c_col)) + n_hue) % n_hue;
  light_change = ((get_light (a_col) - get_light (c_col)) + n_light) % n_light;

  strcpy (msg, "unknown");

  t2printf ("action: c_col=%s, a_col=%s -> hue_change %d - %d = %d, "
            "light_change %d - %d = %d\n",
            cell2str (c_col), cell2str (a_col),
            get_hue (a_col), get_hue (c_col), hue_change,
            get_light (a_col), get_light (c_col), light_change);

  switch (hue_change) {

  case 0:
    /*  None                  push        pop     */
    if (light_change == 0) {
      /*
       * noop - nothing to do (should not happen)
       */
      strcpy (msg, "noop (oops ?)");
      tprintf ("action: noop (oops ?)\n");
    } else if (light_change == 1) {
      /*
         push: Pushes the value of the colour block just exited on to the
         stack. Note that values of colour blocks are not automatically
         pushed on to the stack - this push operation must be explicitly
         carried out.
       */
      if (gd_trace_simple) {
        strcpy (msg, "pu");
      } else {
        sprintf (msg, "push(%d)", num_cells);
      }
      tprintf ("action: push, value %d\n", num_cells);
      alloc_stack_space (num_stack + 1);
      stack [num_stack++] = num_cells;
      tdump_stack ();

    } else if (light_change == 2) {
      /*
         pop: Pops the top value off the stack and discards it.
       */
      if (gd_trace_simple) {
        strcpy (msg, "po");
      } else {
        strcpy (msg, "pop");
      }
      tprintf ("action: pop\n");
      if (num_stack > 0) {
        num_stack--;
      } else {
        tprintf ("info: pop failed: stack underflow\n");
      }
      tdump_stack ();
    }

    break;

  case 1:
    /*     1 Step       add    subtract   multiply */
    if (light_change == 0) {
      /*
         add: Pops the top two values off the stack, adds them, and pushes
         the result back on the stack.
       */
      if (gd_trace_simple) {
        strcpy (msg, "+");
      } else {
        strcpy (msg, "add");
      }
      tprintf ("action: add\n");
      if (num_stack < 2) {
        tprintf ("info: add failed: stack underflow \n");
      } else {
        stack [num_stack - 2] = stack [num_stack - 2] + stack [num_stack - 1];
        num_stack--;
      }
      tdump_stack ();

    } else if (light_change == 1) {
      /*
         subtract: Pops the top two values off the stack, subtracts the top
         value from the second top value, and pushes the result back on the
         stack.
       */
      if (gd_trace_simple) {
        strcpy (msg, "-");
      } else {
        strcpy (msg, "sub");
      }
      tprintf ("action: sub\n");
      if (num_stack < 2) {
        tprintf ("info: sub failed: stack underflow \n");
      } else {
        stack [num_stack - 2] = stack [num_stack - 2] - stack [num_stack - 1];
        num_stack--;
      }
      tdump_stack ();

    } else if (light_change == 2) {
      /*
         multiply: Pops the top two values off the stack, multiplies them,
         and pushes the result back on the stack.
       */
      if (gd_trace_simple) {
        strcpy (msg, "*");
      } else {
        strcpy (msg, "mul");
      }
      tprintf ("action: multiply\n");
      if (num_stack < 2) {
        tprintf ("info: multiply failed: stack underflow \n");
      } else {
        stack [num_stack - 2] = stack [num_stack - 2] * stack [num_stack - 1];
        num_stack--;
      }
      tdump_stack ();
    }
    break;

  case 2:
    /*    2 Steps    divide         mod        not */
    if (light_change == 0) {
      /*
         divide: Pops the top two values off the stack, calculates the
         integer division of the second top value by the top value, and
         pushes the result back on the stack.
       */
      if (gd_trace_simple) {
        strcpy (msg, "/");
      } else {
        strcpy (msg, "div");
      }
      tprintf ("action: divide\n");
      if (num_stack < 2) {
        tprintf ("info: divide failed: stack underflow \n");
      } else if (stack [num_stack - 1] == 0) {
        /* try to put a undefined, but visible value on stack: */
        stack [num_stack - 2] = 99999999;
        num_stack--;
        tprintf ("info: divide failed: division by zero\n");
      } else {
        stack [num_stack - 2] = stack [num_stack - 2] / stack [num_stack - 1];
        num_stack--;
      }
      tdump_stack ();

    } else if (light_change == 1) {
      /*
         mod: Pops the top two values off the stack, calculates the second
         top value modulo the top value, and pushes the result back on the
         stack.
       */
      if (gd_trace_simple) {
        strcpy (msg, "%");
      } else {
        strcpy (msg, "mod");
      }
      tprintf ("action: mod\n");
      if (num_stack < 2) {
        tprintf ("info: mod failed: stack underflow \n");
      } else {
        stack [num_stack - 2] = stack [num_stack - 2] % stack [num_stack - 1];
        num_stack--;
      }
      tdump_stack ();

    } else if (light_change == 2) {
      /*
         not: Replaces the top value of the stack with 0 if it is non-zero,
         and 1 if it is zero.
       */
      if (gd_trace_simple) {
        strcpy (msg, "!");
      } else {
        strcpy (msg, "not");
      }
      tprintf ("action: not\n");
      if (num_stack < 1) {
        tprintf ("info: not failed: stack underflow \n");
      } else {
        stack [num_stack - 1] = ! stack [num_stack - 1];
      }
      tdump_stack ();
    }

    break;

  case 3:
    /*    3 Steps   greater     pointer     switch */

    if (light_change == 0) {
      /*
         greater: Pops the top two values off the stack, and pushes 1 on to
         the stack if the second top value is greater than the top value,
         and pushes 0 if it is not greater.
       */
      if (gd_trace_simple) {
        strcpy (msg, ">");
      } else {
        strcpy (msg, "gt");
      }
      tprintf ("action: greater\n");
      if (num_stack < 2) {
        tprintf ("info: greater failed: stack underflow \n");
      } else {
        stack [num_stack - 2] = stack [num_stack - 2] > stack [num_stack - 1];
        num_stack--;
      }
      tdump_stack ();

    } else if (light_change == 1) {
      /*
         pointer: Pops the top value off the stack and rotates the DP
         clockwise that many steps (anticlockwise if negative).
       */
      int i, val;

      strcpy (msg, "dp");
      tprintf ("action: pointer\n");
      if (num_stack < 1) {
        tprintf ("info: pointer failed: stack underflow \n");
      } else {
        val = stack [num_stack - 1];

        for (i = 0; val > 0 && i < (val % 4); i++) {
          p_dir_pointer = turn_dp (p_dir_pointer);
        }
        for (i = 0; val < 0 && i > ((-1 * val) % 4); i++) {
          p_dir_pointer = turn_dp_inv (p_dir_pointer);
        }
        num_stack--;

        if (! gd_trace_simple) {
          /* add param to msg: */
          sprintf (msg, "dp(%d)", val);
        }
      }
      tdump_stack ();

    } else if (light_change == 2) {
      /*
         switch: Pops the top value off the stack and toggles the CC that
         many times.
       */
      int i, val;

      strcpy (msg, "cc");
      tprintf ("action: switch\n");
      if (num_stack < 1) {
        tprintf ("info: switch failed: stack underflow \n");
      } else {
        val = stack [num_stack - 1];

        for (i = 0; i < val; i++) {
          p_codel_chooser = toggle_cc (p_codel_chooser);
        }
        num_stack--;
        tdump_stack ();

        if (! gd_trace_simple) {
          /* add param to msg: */
          sprintf (msg, "cc(%d)", val);
        }
      }
      tdump_stack ();
    }

    break;

  case 4:
    /*    4 Steps  duplicate  roll  in(number) */
    if (light_change == 0) {
      /*
         duplicate: Pushes a copy of the top value on the stack on to the
         stack.
       */
      if (gd_trace_simple) {
        strcpy (msg, "du");
      } else {
        strcpy (msg, "dup");
      }
      tprintf ("action: duplicate\n");
      if (num_stack < 1) {
        tprintf ("info: duplicate failed: stack underflow \n");
      } else {
        alloc_stack_space (num_stack + 1);
        stack [num_stack] = stack [num_stack - 1];
        num_stack++;
      }
      tdump_stack ();

    } else if (light_change == 1) {
      /*
         roll: Pops the top two values off the stack and "rolls" the
         remaining stack entries to a depth equal to the second value
         popped, by a number of rolls equal to the first value popped. A
         single roll to depth n is defined as burying the top value on the
         stack n deep and bringing all values above it up by 1 place. A
         negative number of rolls rolls in the opposite direction. A
         negative depth is an error and the command is ignored.
       */
      int roll, depth;

      if (gd_trace_simple) {
        strcpy (msg, "ro");
      } else {
        strcpy (msg, "roll");
      }
      tprintf ("action: roll\n");
      if (num_stack < 2) {
        tprintf ("info: roll failed: stack underflow \n");
      } else {
        roll = stack [num_stack - 1];
        depth = stack [num_stack - 2];
        num_stack -= 2;

        if (depth < 0) {
          tprintf ("info: roll failed: negative depth \n");
        } else if (num_stack < depth) {
          tprintf ("info: roll failed: stack underflow \n");
        } else {
          int i;
          /* roll is positive: */
          for (i = 0; i < roll && roll > 0; i++) {
            int j, val = stack [num_stack - 1];
            for (j = 0; j < depth - 1; j++) {
              stack [num_stack - j - 1] = stack [num_stack - j - 2];
            }
            stack [num_stack - depth] = val;
          }
          /* roll is negative: */
          for (i = 0; i > roll && roll < 0; i--) {
            int j, val = stack [num_stack - depth];
            for (j = 0; j < depth - 1; j++) {
              stack [num_stack - depth + j ] =
                stack [num_stack - depth + j + 1];
            }
            stack [num_stack - 1] = val;
          }
        }
      }
      tdump_stack ();

    } else if (light_change == 2) {
      /*
         in: Reads a value from STDIN as either a number or character,
         depending on the particular incarnation of this command and pushes
         it on to the stack.
       */
      int c;

      if (gd_trace_simple) {
        strcpy (msg, "iN");
      } else {
        strcpy (msg, "inN");
      }
      tprintf ("action: in(number)\n");
      alloc_stack_space (num_stack + 1);

      if (! quiet) {
        /* show a prompt: */
        printf ("? "); fflush (stdout);
      }

      if (1 != fscanf (stdin, "%d", &c)) {
        tprintf ("info: cannot read int from stdin; reason: %s\n",
                 strerror (errno));
      } else {
        stack [num_stack++] = c;
      }
      tdump_stack ();
    }

    break;

  case 5:
    /*    5 Steps  in(char) out(number)  out(char) */

    if (light_change == 0) {
      /*
         in: Reads a value from STDIN as either a number or character,
         depending on the particular incarnation of this command and pushes
         it on to the stack.
       */
      int c;

      if (gd_trace_simple) {
        strcpy (msg, "iC");
      } else {
        strcpy (msg, "inC");
      }
      tprintf ("action: in(char)\n");
      alloc_stack_space (num_stack + 1);

      if (! quiet) {
        /* show a prompt: */
        printf ("? "); fflush (stdout);
      }
      if ((c = getchar ()) < 0) {
        tprintf ("info: cannot read char from stdin; reason: %s\n",
                 strerror (errno));
      } else {
        stack [num_stack++] = c % 0xff;
      }
      tdump_stack ();

    } else if (light_change == 1) {
      /*
         out: Pops the top value off the stack and prints it to STDOUT as
         either a number or character, depending on the particular
         incarnation of this command.
       */
      if (gd_trace_simple) {
        strcpy (msg, "oN");
      } else {
        strcpy (msg, "outN");
      }
      tprintf ("action: out(number)\n");
      if (num_stack < 1) {
        tprintf ("info: out(number) failed: stack underflow \n");
      } else {
        printf ("%ld", stack [num_stack - 1]); fflush (stdout);
        if (trace || debug) {
          /* increase readability: */
          tprintf ("\n");
        }
        num_stack--;
      }
      tdump_stack ();

    } else if (light_change == 2) {
      /*
         out: Pops the top value off the stack and prints it to STDOUT as
         either a number or character, depending on the particular
         incarnation of this command.
       */
      if (gd_trace_simple) {
        strcpy (msg, "oC");
      } else {
        strcpy (msg, "outC");
      }
      tprintf ("action: out(char)\n");
      if (num_stack < 1) {
        tprintf ("info: out(char) failed: stack underflow \n");
      } else {
        printf ("%c", (int) (stack [num_stack - 1] & 0xff));
        fflush (stdout);
        if (trace || debug) {
          /* increase readability: */
          tprintf ("\n");
        }
        num_stack--;
      }
      tdump_stack ();
    }

    break;
  }

  return 0;
}

