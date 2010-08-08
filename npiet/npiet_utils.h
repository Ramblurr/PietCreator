/*
Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>

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


struct trace_step {
    int execution_step; /**< step number */

    int p_xpos, p_ypos; /**<  */
    int p_dp, p_cc; /**< previous values of dp and cc */
    int p_color; /**< color of cell at p_xpos, p_ypos */

    int n_xpos, n_ypos; /**< next x and y positions */
    int n_dp, n_cc; /**< next values of dp and cc */
    int n_color; /**< color of cell at n_xpos, n_ypos */
};

struct trace_action {
    int hue_change;
    int light_change;
    int value;

    int before_num;
    long *before_stack;

    int after_num;
    long* after_stack;

    char* msg;
};

/**
* step    - step number
* px/py   - prev x,y coord
* pdp/pcc - prev dp/cc
* pcol    - prev color
*
* nx/ny   - next x,y coord
* ndp/ncc - next dp/cc
* ncol    - next color
*/
void notify_step( int step, int px, int py, int pdp, int pcc, int pcol,
                  int nx, int ny, int ndp, int ncc, int ncol );

void notify_action( int hue_change, int light_change, int value, char* msg );

void notify_stack_before( long* stack, int num_stack );
void notify_stack_after( long* stack, int num_stack );

typedef void (*step_callback_t)( void* object, struct trace_step* );
typedef void (*action_callback_t)( void* object, struct trace_action* );

void register_step_callback( step_callback_t callable, void* obj );
void register_action_callback( action_callback_t callable, void* obj );


int read_int();
char read_char();

typedef int (*readint_callback_t)( void* object );
typedef char (*readchar_callback_t)( void* object );

void register_readint_callback( readint_callback_t callable, void* obj );
void register_readchar_callback( readchar_callback_t callable, void* obj );
