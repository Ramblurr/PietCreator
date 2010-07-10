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

#include "config.h"

/*
 * helper to guess codel size:
 */
void
c_check (int i, int c, int *last_c, int *last_p, int *min_w)
{
  if (i == 0) {
    *last_c = c;
    *last_p = i;
  } else if (*last_c != c) {
    int w = i - *last_p;
    if (w < *min_w) {
      *min_w = w;
    }
    *last_c = c;
    *last_p = i;
  }
}