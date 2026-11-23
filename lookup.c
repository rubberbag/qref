#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "qref.h"

#define FNV_OFFEST_BASIS    2166136261u
#define FNV_PRIME           16777619u      
/**
 * Compute a 32-bit FNV-la hash for a NULL-terminated string.
 */
static uint32_t hash_str_lower(const char *str)
{
    assert (str != NULL);

    uint32_t hash = FNV_OFFEST_BASIS;

    while (*str)
    {
        hash ^= (unsigned char)(*str++);
        hash *= FNV_PRIME;
    }
    
    return hash;
}

/**
 * Return true if two NLL-terminated strings match ignoring ascii case.
 */
static bool str_ieq(const char *a, const char *b)
{
    while (*a && *b)
    {
        if ( tolower((unsigned char) *a) != tolower((unsigned char) *b))
        {
            return false;
        }

        a++;
        b++;
    }
    
    return *a == *b;
}

static bool str_matches(const char *a, const char *b, enum CaseMode mode)
{
    if(mode == CASE_SENSITIVE)
    {
        return strcmp(a,b) == 0;
    }

    return str_ieq(a,b);
}

/**
 * Build an exact-sized hash table table on the stack for the givrn scope ( or all
 * registered topics if scope if NULL), preferring exact case matches before
 * falling back to case-insensetive matches
 */
int lookup_and_print( const char *term, const struct Subcommand *scope)
{

    if (!term)
    {
        fprintf(stderr,"Error: term can not be null");
        return EXIT_FAILURE;
    }

    size_t total_items = scope ? scope->count :  registry_total_refs();

    if (total_items == 0)
    {
        fprintf(stderr, "No entries registered.\n");
        return EXIT_FAILURE;
    }
    
    size_t  table_size = total_items * 2;
    const struct Qref *items[total_items];
    int next_idx[total_items];
    int lut[table_size];

    memset(next_idx, -1, sizeof(next_idx));
    memset(lut, -1, sizeof(lut));

    /* Collect pointers to all Qref entries in scope */
    size_t idx = 0;
    if (scope)
    {
        for ( size_t i = 0; i < scope->count; i++)
        {
            items[idx++] = &scope->qrefs[i];
        }
    }
    else
    {
        for (const struct Subcommand *cmd = registry_head(); cmd != NULL; cmd = cmd->next)
        {
            for (size_t i = 0; i < cmd->count; i++)
            {
                items[idx++] = &cmd->qrefs[i];
            }
        }
    }
    

    /* Insert pass: group case-insensitive equivalents, linear-probe lut[] and chain duplicate names via next_idx[] */
    for (size_t i = 0; i< total_items; i++)
    {
        size_t slot = hash_str_lower(items[i]->name) % table_size;

        while (lut[slot] !=-1)
        {
            if ( str_ieq(items[lut[slot]]->name, items[i]->name))
            {
                next_idx[i] = lut[slot];
                break;
            }
            slot = (slot + 1) % table_size;
        }
        lut[slot] = (int)i;
    }

    /* Search pass: probe lut[] for term and walk any chain duplicates */
    size_t slot = hash_str_lower(term) % table_size;
    while (lut[slot] !=-1)
    {
        int curr = lut[slot];

        if (str_ieq(items[curr]->name, term))
        { 
            const struct Qref *matches[total_items];
            size_t match_count = 0;
            
            while(curr != -1)
            {
                if(str_matches(items[curr]->name, term, items[curr]->case_mode))
                {
                    matches[match_count++] = items[curr];
                }

                curr = next_idx[curr];

                if (match_count > 0)
                {
                    print_qrefs(matches, match_count, scope ? scope->description: "Global lookup");

                    return EXIT_SUCCESS;
                }

            }
        }

        slot = (slot + 1) % table_size;
    }

    fprintf(stderr, "No entry found for '%s'\n", term);
    return EXIT_FAILURE;
}