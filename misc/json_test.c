
#include <stdio.h>
#include <assert.h>
#include "bstr/bstr.h"
#include "talloc.h"
#include "json.h"

int mainX(int argc, char *argv[])
{
    ta_enable_leak_report();
    void *ctx = talloc_new(NULL);
    bstr buf = {0};
    FILE *f = stdin;
    if (argc == 2)
        f = fopen(argv[1], "r");
    if (!f)
        return 1;
    while (!feof(f) && !ferror(f)) {
        char tmp[1000];
        size_t r = fread(tmp, 1, sizeof(tmp), f);
        bstr_xappend(ctx, &buf, (bstr){tmp, r});
    }
    bstr_xappend(ctx, &buf, bstr0(""));
    char *in = buf.start;
    struct mpv_node tree;
    if (json_parse(ctx, &tree, &in, 10) < 0) {
        fprintf(stderr, "read failure\n");
    } else {
        char *s = talloc_strdup(ctx, "");
        if (json_write(&s, &tree) < 0) {
            fprintf(stderr, "write failure\n");
        } else {
            printf("%s\n", s);
            // Now parse it again
            struct mpv_node tree2;
            char *sc = talloc_strdup(ctx, s);
            if (json_parse(ctx, &tree2, &sc, 10) < 0) {
                fprintf(stderr, "read failure (2)\n");
            } else {
                // And write again
                char *s2 = talloc_strdup(ctx, "");
                if (json_write(&s2, &tree2) < 0) {
                    fprintf(stderr, "write failure (2)\n");
                } else {
                    printf("%s\nres=%d\n", s2, strcmp(s, s2));
                }
            }
        }
    }
    talloc_free(ctx);
    return 0;
}
