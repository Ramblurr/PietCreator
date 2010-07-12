#ifndef NPIET_H
#define NPIET_H

/* print debugging stuff: */
extern int debug;


/* internal used index for filling areas: */
#define c_mark_index    9999

/*
 * color and hue values:
 *
 * we order the colors linear:
 *
 *   idx 0:   light red
 *   [...]
 *   idx 15:  dark margenta
 *   idx 16:  white
 *   idx 17:  black

 */
#define n_hue           6               /* 4 colors */
#define n_light         3               /* 4 shades */
#define c_white         (n_hue * n_light)
#define c_black         (c_white + 1)
#define n_colors        (c_black + 1)

// extern int width;
// extern int height;
// extern int *cells;
// extern int p_dir_pointer;              /* DP: p_{left, right, up, down} */
// extern int p_codel_chooser;            /* CC: p_left or p_right */

int set_image( unsigned int* _cells, int w, int h);
int read_ppm (char *fname);
int piet_run();
void piet_init ();
int piet_step ();
/*
 * walk along the border of a given colorblock looking about the
 * next codel described by dir dp and the cc.
 *
 * return the coordinates of the new codel and the new directions.
 */
int piet_walk_border (int *n_x, int *n_y, int *num_cells);
/*
 *  Commands
 *                           Lightness change
 *  Hue change      None    1 Darker   2 Darker
 *
 *       None                  push        pop
 *     1 Step       add    subtract   multiply
 *    2 Steps    divide         mod        not
 *    3 Steps   greater     pointer     switch
 *    4 Steps duplicate        roll in(number)
 *    5 Steps  in(char) out(number)  out(char)
 *
 * fill msg with a string describing the action (limited space).
 *
 * return -1 on error condition (actually there is none)
 */

int piet_action (int c_col, int a_col, int num_cells, char *msg);

#endif /*NPIET_H*/