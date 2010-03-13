/*
 *  Copyright (C) 2010 Erik Edelmann <erik.edelmann@iki.fi>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef __EVAL_H__
#define __EVAL_H__

#include <glib.h>
#include "parsetree.h"

double eval_parse_tree(node_t *parsetree, gboolean use_degrees);

double my_sin(double x);
double my_cos(double x);
double my_tan(double x);
double my_asin(double x);
double my_acos(double x);
double my_atan(double x);

#endif // !__EVAL_H__
