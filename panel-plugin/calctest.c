#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include "parser.h"
#include "eval.h"

#define LINE_LENGTH 1024

void calc(const char *input, char *result, size_t result_len);

void interactive()
{
    char line[LINE_LENGTH], result[LINE_LENGTH];

    while (fgets(line, LINE_LENGTH, stdin)) {
        calc(line, result, LINE_LENGTH);
        printf("%s\n", result);
    }
}


int main(int argc, char **argv)
{
    char result[LINE_LENGTH];

    if (argc == 1) {
        interactive();
    } else if (argc == 2) {
        calc(argv[1], result, LINE_LENGTH);
        printf("%s\n", result);
    } else {
        fprintf(stderr,"Usage: %s [expr]\n", argv[0]);
        return 1;
    }
    return 0;
}


void calc(const char *input, char *result, size_t result_len)
{
    node_t *parsetree;
    double r;
    GError *err = NULL;

    parsetree = build_parse_tree(input, &err);
    if (err) {
        snprintf(result, result_len, "%s\n", err->message);
        g_error_free(err);
    } else if (parsetree) {
        r = eval_parse_tree(parsetree, FALSE);
        snprintf(result, result_len, "%g\n", r);
    } else
        snprintf(result, result_len, "böö\n");
    free_parsetree(parsetree);
}
