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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include "parsetree.h"
#include "constants.h"
#include "eval.h"

static gboolean trigonometrics_use_degrees;


double my_sin(double x)
// (I'm a sinner ...)
{
    if (trigonometrics_use_degrees) x = x/360*2*G_PI;
    return sin(x);
}

double my_cos(double x)
{
    if (trigonometrics_use_degrees) x = x/360*2*G_PI;
    return cos(x);
}

double my_tan(double x)
{
    if (trigonometrics_use_degrees) x = x/360*2*G_PI;
    return tan(x);
}

double my_asin(double x)
{
    double y = asin(x);
    if (trigonometrics_use_degrees) y = y/(2*G_PI)*360;
    return y;
}

double my_acos(double x)
{
    double y = acos(x);
    if (trigonometrics_use_degrees) y = y/(2*G_PI)*360;
    return y;
}

double my_atan(double x)
{
    double y = atan(x);
    if (trigonometrics_use_degrees) y = y/(2*G_PI)*360;
    return y;
}


static double eval(node_t *parsetree)
{
    double left, right, r, arg;

    if (!parsetree)
        return NAN;

    switch (parsetree->type) {

    case NODE_NUMBER:
        r = parsetree->val.num;
        break;

    case NODE_OPERATOR:

        left = eval(parsetree->left);
        right = eval(parsetree->right);

        switch (parsetree->val.op) {
        case OP_PLUS:
            r = left + right;
            break;
        case OP_MINUS:
            r = left - right;
            break;
        case OP_UMINUS:
            g_assert(isnan(left));
            r = -right;
            break;
        case OP_TIMES:
            r = left * right;
            break;
        case OP_DIV:
            r = left / right;
            break;
        case OP_POW:
            r = pow(left, right);
            break;
        default:
            g_assert_not_reached();
        }
        break;

    case NODE_FUNCTION:
        g_assert(parsetree->right);
        g_assert(parsetree->left == NULL);

        arg = eval(parsetree->right);
        r = parsetree->val.fun(arg);
        break;
        
    default:
        g_assert_not_reached();
    }

    return r;
}


double eval_parse_tree(node_t *parsetree, gboolean use_degrees)
{
    trigonometrics_use_degrees = use_degrees;

    return eval(parsetree);
}
