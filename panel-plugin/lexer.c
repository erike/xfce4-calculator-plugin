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
#include <ctype.h>
#include <glib.h>
#include "lexer.h"

static gboolean isoperator(int c)
{
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
        return TRUE;
    else
        return FALSE;
}

/* Return pointer to next token, starting at input[*index], or NULL if there
   were no tokens.  Put pointer to the next char after the token into '*end' Put
   the position of the start of the token into pos.  Tokens should be
   g_free():ed when no longer needed. */

static token_t *get_next_token(const char *input, int *index)
{
    const char *t;
    token_t *token;
    int i;

    g_assert(input);
    g_assert(index);
    g_assert(*index <= strlen(input));

    i = *index;

    while (isspace(input[i])) i++;

    if (!input[i]) return NULL;

    token = g_malloc(sizeof(token_t));
    token->position = i;

    if (isdigit(input[i]) || input[i] == '.') {
        token->type = TOK_NUMBER;
        token->val.num = g_strtod(input+i, (char **)&t);
        i = (t - input);
    } else if (input[i] == '(') {
        token->type = TOK_LPAREN;
        i++;
    } else if (input[i] == ')') {
        token->type = TOK_RPAREN;
        i++;
    } else if (isoperator(input[i])) {
        token->type = TOK_OPERATOR;
        if (input[i] == '*' && input[i+1] == '*') {
            // '**' is equivalent to '^'
            token->val.op = '^';
            i += 2;
        } else {
            token->val.op = input[i];
            i++;
        }
    } else if (isalpha(input[i])) {
        token->type = TOK_IDENTIFIER;

        int n = 0;
        while (isalnum(input[i]) && n < MAX_ID_LEN) {
            token->val.id[n] = input[i];
            n++, i++;
        }
        token->val.id[n] = '\0';
    } else {
        token->type = TOK_OTHER;
        token->val.other = input[i];
        i++;
    }

    *index = i;
    return token;
}


/* Return string representation of token.  String mstn't be freed, and must be
   copied if it needs to be stored. */

const char *token2str(const token_t *token)
{
    static char s[MAX_ID_LEN+1];

    g_assert(token);

    switch (token->type) {
    case TOK_NUMBER:
        g_snprintf(s, MAX_ID_LEN, "%g", token->val.num);
        break;
    case TOK_OPERATOR:
        g_snprintf(s, MAX_ID_LEN, "%c", token->val.op);
        break;
    case TOK_IDENTIFIER:
        g_snprintf(s, MAX_ID_LEN, "%s", token->val.id);
        break;
    case TOK_LPAREN:
        g_strlcat(s, "(", MAX_ID_LEN);
        break;
    case TOK_RPAREN:
        g_strlcat(s, ")", MAX_ID_LEN);
        break;
    case TOK_OTHER:
        g_snprintf(s, MAX_ID_LEN, "%c", token->val.other);
        break;
    case TOK_NULL:
        g_strlcat(s, "(null)", MAX_ID_LEN);
        break;
    default:
        g_print("Hoho! %i\n", token->type);
        g_assert_not_reached();
    }

    return s;
}


/* Return a stack of tokens, representing the input. */

token_stack_t *lexer(const char *input)
{
    token_t *token;
    token_stack_t *stack;
    int index = 0;

    stack = g_malloc(sizeof(token_stack_t));
    stack->top = get_next_token(input, &index);
    token = stack->top;
    while (token) {
        //g_print("Token: %s at %i\n", token2str(token), token->position);
        token->next = get_next_token(input, &index);
        token = token->next;
    }

    return stack;
}


const token_t *token_peak(const token_stack_t *stack)
{
    g_assert(stack);
    return stack->top;
}


token_t *token_pop(token_stack_t *stack)
{
    token_t *token;

    g_assert(stack);

    token = stack->top;

    if (stack->top) 
        stack->top = stack->top->next;

    return token;
}


void free_token_stack(token_stack_t *stack)
{
    token_t *token;

    g_assert(stack);

    while ((token = token_pop(stack))) 
        g_free(token);

    g_free(stack);
}
