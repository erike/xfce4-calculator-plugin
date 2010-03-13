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
#include "parser.h"
#include "lexer.h"
#include "eval.h"


/* 
 * A recursive descent parser for the grammar defined in the file grammar.txt.
 */

/* 
 * A note on error handling: If a get_<something> function encounters an error,
 * it should:

 * 1) Set an apropriate error message in 'err'
 * 2) Either free any nodes it has created using free_parsetree() and return
 *    NULL, or return the node as is (so that it can be freed later).
 */


static struct {
    char *name;
    double value;
} constants[] = {
    { "pi", G_PI },
    { NULL, 0.0 }
};

static struct {
    char *name;
    double (*fun)(double x);
} functions[] = {
    { "sqrt", sqrt },
    { "log", log },
    { "ln", log },
    { "exp", exp },
    { "sin", my_sin },
    { "cos", my_cos },
    { "tan", my_tan },
    { "asin", my_asin },
    { "arcsin", my_asin },
    { "acos", my_acos },
    { "arccos", my_acos },
    { "atan", my_atan },
    { "arctan", my_atan },
    { "log2", log2 },
    { "log10", log10 },
    { "lg", log10 },
    { "abs", fabs },
    { "cbrt", cbrt },
    { NULL, NULL }
};


/* Look up the constant of name 'name', and put it in 'value' if found.  Return
   TRUE if found, FALSE if not. */

static gboolean find_constant(const char *name, double *value)
{
    int i = 0;

    while (constants[i].name) {
        if (strcmp(name, constants[i].name) == 0) {
            *value = constants[i].value;
            return TRUE;
        }
        i++;
    }

    return FALSE;
}


/* Lookup the function 'name', and put a pointer to its implementation in 'fun'
   if found. Return TRUE if found, FALSE if not. */

static gboolean find_function(const char *name, double (**fun)(double x))
{
    int i = 0;

    while (functions[i].name) {
        if (strcmp(name, functions[i].name) == 0) {
            *fun = functions[i].fun;
            return TRUE;
        }
        i++;
    }

    return FALSE;
}



static node_t *get_expr(token_stack_t *stack, GError **err);


static gboolean is_mult_op(char op)
{
    /*
    return op[0] == '/' || (op[0] == '*' && op[1] == '\0')
    */
    return op == '/' || op == '*';
}

/*
static gboolean is_add_op(char op)
{
    return op == '+' || op == '-';
}
*/


static void set_error(GError **err, const char *msg, const token_t *token)
{
    char pos_str[32];
    int pos;

    if (token) {
        pos = token->position;
        g_snprintf(pos_str, sizeof(pos_str), "position %i", pos+1);
    } else {
        pos = -1;
        g_snprintf(pos_str, sizeof(pos_str), "end of input");
    }

    g_set_error(err, 0, pos, "At %s: %s", pos_str, msg);
}


static node_t *get_number(token_stack_t *stack, GError **err)
{
    token_t *token;
    node_t *node;

    g_assert(stack);

    token = token_pop(stack);

    if (token && token->type == TOK_NUMBER) {
        node = g_malloc(sizeof(node_t));
        node->type = NODE_NUMBER;
        node->val.num = token->val.num;
        node->left = node->right = NULL;
    } else {
        node = NULL;
        set_error(err, "Expected number", token);
    }

    g_free(token);

    return node;
}


/* Look for '(' <expr> ')'. */

static node_t *get_parentised_expr(token_stack_t *stack, GError **err)
{
    token_t *token;
    GError *tmp_err = NULL;
    node_t *node;

    // '('
    token = token_pop(stack);
    if (!token || token->type != TOK_LPAREN) {
        set_error(err, "Expected '('", token);
        g_free(token);
        return NULL;
    }

    // expr
    node = get_expr(stack, &tmp_err); 
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        free_parsetree(node);
        return NULL;
    }

    if (!node) { 
        // Re-use the RPAREN token for this error message.
        token->position++;
        set_error(err, "Expected expression", token);
    }
    g_free(token);

    // ')'
    token = token_pop(stack);
    if (!token || token->type != TOK_RPAREN) {
        free_parsetree(node);
        set_error(err, "Expected ')'", token);
        g_free(token);
        return NULL;
    }

    g_free(token);
    return node;
}


static node_t *get_pow(token_stack_t *stack, GError **err)
{
    const token_t *token;
    node_t *node;
    token_type_t type;
    GError *tmp_err = NULL;
    double x;
    double (*fun)(double x);
    char msg[128];

    token = token_peak(stack);

    type = (token) ? token->type : TOK_NULL;
    switch (type) {
    case TOK_LPAREN:
        node = get_parentised_expr(stack, &tmp_err);
        if (tmp_err)
            g_propagate_error(err, tmp_err);
        break;
    case TOK_NUMBER:
        node = get_number(stack, &tmp_err);
        if (tmp_err)
            g_propagate_error(err, tmp_err);
        break;
    case TOK_IDENTIFIER:
        token = token_pop(stack);
        if (find_constant(token->val.id, &x)) {
            node = g_malloc(sizeof(node_t));
            node->type = NODE_NUMBER;
            node->val.num = x;
            node->left = node->right = NULL;
        } else if (find_function(token->val.id, &fun)) {
            node = g_malloc(sizeof(node_t));
            node->type = NODE_FUNCTION;
            node->val.fun = fun;
            node->left = NULL;
            node->right = get_parentised_expr(stack, &tmp_err);
            if (tmp_err)
                g_propagate_error(err, tmp_err);
            break;
        } else {
            g_snprintf(msg,sizeof(msg),"Unknown identifier '%s'",token->val.id);
            set_error(err, msg, token);
            node = NULL;
        }
        break;
    default:
        set_error(err,"Expected '(', number, constant or function",token);
        node = NULL;
    }
    return node;
}


static node_t *get_spow(token_stack_t *stack, GError **err)
{
    const token_t *token;
    node_t *node;
    GError *tmp_err = NULL;

    token = token_peak(stack);

    if (!token) {
        set_error(err, "Expected '(', number, constant or function", token);
        return NULL;
    }

    if (token->type == TOK_OPERATOR && token->val.op == '-') {
        g_free(token_pop(stack));
        node = g_malloc(sizeof(node_t));
        node->type = NODE_OPERATOR;
        node->val.op = OP_UMINUS;
        node->left = NULL;
        node->right = get_spow(stack, &tmp_err);
        if (tmp_err)
            g_propagate_error(err, tmp_err);
    } else {
        node = get_pow(stack, &tmp_err);
        if (tmp_err)
            g_propagate_error(err, tmp_err);
    }

    return node;
}


static node_t *get_spowtail(token_stack_t *stack, node_t *left_expr, GError **err)
{
    const token_t *token;
    node_t *op, *expr;
    GError *tmp_err = NULL;

    token = token_peak(stack);

    /* First check if we really have a spowtail here. If not, return the
     * left_expr. */
    if (token == NULL) {
        g_free(token_pop(stack));
        return left_expr;
    } else if (!(token->type == TOK_OPERATOR && token->val.op == '^'))
        return left_expr;

    op = g_malloc(sizeof(node_t));
    op->left = left_expr;
    op->type = NODE_OPERATOR;
    op->val.op = OP_POW;
    g_free(token_pop(stack));

     /* Then there should be a spow ... */
    op->right = get_spow(stack, &tmp_err);
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        return op;
    }

     /* ... and finally another spowtail. */
    expr = get_spowtail(stack, op, &tmp_err);
    if (tmp_err)
        g_propagate_error(err, tmp_err);

    return expr;
}


static node_t *get_factor(token_stack_t *stack, GError **err)
{
    node_t *spow, *expr;
    GError *tmp_err = NULL;

    spow = get_spow(stack, &tmp_err);
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        return spow;
    }

    expr = get_spowtail(stack, spow, &tmp_err);
    if (tmp_err)
        g_propagate_error(err, tmp_err);

    return expr;
}


static node_t *get_factortail(token_stack_t *stack, node_t *left_expr, GError **err)
{
    const token_t *token;
    node_t *op, *expr;
    GError *tmp_err = NULL;

    token = token_peak(stack);

    /* First check if we really have a factortail here. If not, return the
     * factor. */
    if (token == NULL) {
        g_free(token_pop(stack));
        return left_expr;
    } else if (!(token->type == TOK_OPERATOR && is_mult_op(token->val.op)))
        return left_expr;

    op = g_malloc(sizeof(node_t));
    op->left = left_expr;
    op->type = NODE_OPERATOR;
    switch (token->val.op) {
    case '*':
        op->val.op = OP_TIMES;
        break;
    case '/':
        op->val.op = OP_DIV;
        break;
    default:
        set_error(err, "Expected '*' or '/'", token);
        g_free(op);
        return left_expr;
    }
    g_free(token_pop(stack));

    /* Then there should be a factor. */
    op->right = get_factor(stack, &tmp_err);
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        return op;
    }

    /* and finally another factortail */
    expr = get_factortail(stack, op, &tmp_err);
    if (tmp_err)
        g_propagate_error(err, tmp_err);

    return expr;
}


static node_t *get_term(token_stack_t *stack, GError **err)
{
    node_t *factor, *expr;
    GError *tmp_err = NULL;

    factor = get_factor(stack, &tmp_err);
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        return factor;
    }

    expr = get_factortail(stack, factor, &tmp_err);
    if (tmp_err)
        g_propagate_error(err, tmp_err);

    return expr;
}


/* Create a tree representing 'left_expr TAIL'. */

static node_t *get_termtail(token_stack_t *stack, node_t *left_expr,
                            GError **err)
{
    const token_t *token;
    node_t *op, *expr;
    GError *tmp_err = NULL;

    g_assert(stack);

    token = token_peak(stack);

    /* Is the tail an empty string? Then just give the left_expr back. */
    if (token == NULL) {
        g_free(token_pop(stack));
        return left_expr;
    } else if (token->type == TOK_RPAREN)
        return left_expr;

    /* First, there should an operator ... */
    if (token->type != TOK_OPERATOR) {
        set_error(err, "Expected operator", token);
        return left_expr;
    }

    op = g_malloc(sizeof(node_t));
    op->left = left_expr;
    op->type = NODE_OPERATOR;
    switch (token->val.op) {
    case '+':
        op->val.op = OP_PLUS;
        break;
    case '-':
        op->val.op = OP_MINUS;
        break;
    default:
        set_error(err, "Expected '+' or '-'", token);
        g_free(op);
        return left_expr;
    }
    g_free(token_pop(stack));

    /* ... then there should be a term ... */
    op->right = get_term(stack, &tmp_err);
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        return op;
    }

    /* ... and finally another termtail. */
    expr = get_termtail(stack, op, &tmp_err);
    if (tmp_err)
        g_propagate_error(err, tmp_err);

    return expr;
}


static node_t *get_expr(token_stack_t *stack, GError **err)
{
    node_t *term, *expr;
    GError *tmp_err = NULL;
    const token_t *token;

    token = token_peak(stack);
    if (token == NULL || token->type == TOK_RPAREN) return NULL;

    term = get_term(stack, &tmp_err);
    if (tmp_err) {
        g_propagate_error(err, tmp_err);
        return term;
    }

    expr = get_termtail(stack, term, &tmp_err);
    if (tmp_err)
        g_propagate_error(err, tmp_err);

    return expr;
}


node_t *build_parse_tree(const char *input, GError **err)
{
    token_stack_t *stack;
    node_t *tree;

    stack = lexer(input);
    tree = get_expr(stack, err);
    free_token_stack(stack);

    return tree;
}
