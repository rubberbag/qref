#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "qref.h"

static bool force_global  = false;
const struct Subcommand *scope = NULL;
const char *term = NULL;

int main(int argc, char ** argv)
{
    for (int i = 1; i < argc; i++)
    {

        if (strcmp(argv[i], "--version")==0 || strcmp(argv[i], "-v")==0)
        {
            print_version();
            return EXIT_SUCCESS;
        }

        if (argc < 2 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
           print_help();
            return EXIT_SUCCESS;
        }

        if (strcmp(argv[i], "--global")==0 || strcmp(argv[i], "-g")==0)
        {
            force_global = true;
            continue;
        }

        const struct Subcommand *found = find_subcommand(argv[i]);

        if (found && !scope)
        {
            scope = found;
        }
        else if (!term)
        {
            term = argv[i];
        }
        else
        {
            fprintf(stderr, "Error; unexpected argument '%s'\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    /* If -g/--global was passed after a topic name (e.g., 'qref ascii -g'), treat that topic name as the search term */
    if (force_global && scope)
    {
        if (term)
        {
            fprintf(stderr,"Error: cannot  combine topic '%s' with -g\n", scope->name);
            return EXIT_FAILURE;
        }
        term = scope->name;
        scope = NULL;
    }

    if (!scope && !term)
    {
        if (force_global)
        {
            fprintf(stderr, "Error: -g requires a search term\n");
            return EXIT_FAILURE;
        }
        print_help();
        return EXIT_FAILURE;
    }

    if ( scope && !term)
    {
        const struct Qref *ptrs[scope->count];

        for (size_t i = 0; i < scope->count; i++)
        {
            ptrs[i] = &scope->qrefs[i];

        }
        print_qrefs(ptrs, scope->count, scope->description);
        return EXIT_FAILURE;
    }

    return lookup_and_print(term, scope);
}