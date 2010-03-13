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
#ifndef __LEXER_H__
#define __LEXER_H__

#include "constants.h"

typedef enum { TOK_NUMBER, 
               TOK_OPERATOR, 
               TOK_IDENTIFIER, 
               TOK_LPAREN, 
               TOK_RPAREN, 
               TOK_OTHER,
               TOK_NULL } token_type_t;

typedef struct _token_t {
    token_type_t type;
    gint position;
    union {
        double num;
        char op;
        char id[MAX_ID_LEN+1];
        char other;
    } val;

    struct _token_t *next;
} token_t;


typedef struct {
    token_t *top;
} token_stack_t;


token_stack_t  *lexer(const char *input);
const char *token2str(const token_t *token);

/* Token-stack functions */
const token_t *token_peak(const token_stack_t *stack);
token_t *token_pop(token_stack_t *stack);
void free_token_stack(token_stack_t *stack);

#endif
