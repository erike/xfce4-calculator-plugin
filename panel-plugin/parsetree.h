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
#ifndef __PARSETREE_H__
#define __PARSETREE_H__

#include "constants.h"

typedef enum { NODE_OPERATOR, NODE_NUMBER, NODE_FUNCTION } node_type_t;

typedef enum { OP_PLUS, OP_MINUS,
               OP_UMINUS,
               OP_TIMES, OP_DIV,
               OP_POW } operator_type_t;

typedef struct _node_t {
    node_type_t type;
    union {
        double num;
        operator_type_t op;
        double (*fun)(double x);
    } val;
   struct _node_t *left, *right; 
} node_t;

void free_parsetree(node_t *parsetree);

#endif
