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
#include "npiet.h"

#include <stdlib.h>
#include <string.h>

step_callback_t step_callback = 0;
action_callback_t action_callback = 0;

void notify_step( int step, int px, int py, int pdp, int pcc, int pcol,
                  int nx, int ny, int ndp, int ncc, int ncol )
{
    if( step_callback ) {
        struct trace_step *s;
        s = malloc( sizeof( struct trace_step ) );

        s->execution_step = step;

        s->p_xpos = px;
        s->p_ypos = py;
        s->p_dp = pdp;
        s->p_cc = pcc;
        s->p_color = pcol;

        s->n_xpos = nx;
        s->n_ypos = ny;
        s->n_dp = ndp;
        s->n_cc = ncc;
        s->n_color = ncol;

        step_callback( s );
    }
}

void notify_action( int hue_change, int light_change, int value, char* msg)
{
    if( step_callback ) {
        struct trace_action *a;
        a = malloc( sizeof( struct trace_action ) );

        a->hue_change = hue_change;
        a->light_change = light_change;
        a->value = value;
        a->msg = strdup( msg );

        action_callback( a );
    }
}

void register_step_callback( step_callback_t callable )
{
    step_callback = callable;
}

void register_action_callback( action_callback_t callable )
{
    action_callback = callable;
}
