/* main.c — Compiler driver: read source → parse → generate MMIX */

#include <stdio.h>
#include <stdlib.h>
#include "ir.h"

/* From parser.c */
extern IRToken *parse_program(const char *src, int *out_count);

/* From codegen.c */
extern void generate_mmix(IRToken *ir, int n);

static char source[65536];

static int read_source(const char *filename) {
    FILE *f;
    size_t n;

    if (filename) {
        f = fopen(filename, "r");
        if (!f) {
            fprintf(stderr, "Error: cannot open '%s'\n", filename);
            return -1;
        }
    } else {
        f = stdin;
    }

    n = fread(source, 1, sizeof(source) - 1, f);
    source[n] = '\0';

    if (filename)
        fclose(f);

    return 0;
}

int main(int argc, char *argv[]) {
    const char *filename = NULL;
    int ir_count = 0;
    IRToken *ir;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [input.c]\n", argv[0]);
        fprintf(stderr, "       (reads stdin if no file given)\n");
        return 1;
    }

    if (argc == 2)
        filename = argv[1];

    if (read_source(filename) != 0)
        return 1;

    /* Parse source → IR */
    ir = parse_program(source, &ir_count);

    if (ir_count == 0) {
        fprintf(stderr, "No statements generated.\n");
        return 1;
    }

    /* IR → MMIX assembly (written to stdout) */
    generate_mmix(ir, ir_count);

    return 0;
}
