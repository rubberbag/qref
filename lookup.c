#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

#include "qref.h"

constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;
constexpr uint32_t FNV_PRIME        = 16777619u;

struct KeyEntry {
    const char *key;
    const struct Qref *qref;
};

/**
 * Compute a 32-bit FNV-la hash for a null-terminated string.
 */
static uint32_t hash_str_lower(const char *str)
{
    assert (str != nullptr);

    uint32_t hash = FNV_OFFSET_BASIS;

    while (*str) {
        hash ^= (unsigned char)tolower((unsigned char)(*str++));
        hash *= FNV_PRIME;
    }
    
    return hash;
}

/**
 * Return true if two null-terminated strings match ignoring ascii case.
 */
static bool str_ieq(const char *a, const char *b)
{
    while (*a && *b) {
        if ( tolower((unsigned char) *a) != tolower((unsigned char) *b))
            return false;

        a++;
        b++;
    }
    
    return *a == *b;
}

static bool str_matches(const char *a, const char *b, enum CaseMode mode)
{
    if (mode == CASE_SENSITIVE)
        return strcmp(a,b) == 0;

    return str_ieq(a,b);
}

/**
 * Build an exact-sized hash table table on the stack for the givrn scope ( or all
 * registered topics if scope if nullptr), preferring exact case matches before
 * falling back to case-insensetive matches
 */
int lookup_and_print( const char *term, const struct Subcommand *scope)
{

    if (!term) {
        fprintf(stderr,"Error: term cannot be nullptr");
        return EXIT_FAILURE;
    }

    size_t total_items = scope ? scope->count :  registry_total_refs();

    if (total_items == 0) {
        fprintf(stderr, "No entries registered.\n");
        return EXIT_FAILURE;
    }
    
    size_t max_keys = total_items * (1 + MAX_ALIASES);
    struct KeyEntry entries[max_keys];
    size_t key_count = 0;
    

    
    /* Collect all primary names and aliases in scope */
    for (const struct Subcommand *cmd = (scope ? scope : registry_head());
         cmd != nullptr;
         cmd = (scope ? nullptr : cmd->next))
    {
        for (size_t i = 0; i < cmd->count; i++) {
            const struct Qref *q = &cmd->qrefs[i];
            entries[key_count++] = (struct KeyEntry){ .key = q->name, .qref = q };

            for (size_t a = 0; a < MAX_ALIASES && q->aliases[a] != nullptr; a++) {
                entries[key_count++] = (struct KeyEntry){ .key = q->aliases[a], .qref = q };
            }
        }
    }

    
    size_t  table_size = key_count * 2;
    int next_idx[key_count];
    int lut[table_size];

    memset(next_idx, -1, sizeof(next_idx));
    memset(lut, -1, sizeof(lut));


    /* Insert pass: group case-insensitive equivalents, linear-probe lut[] and chain duplicate names via next_idx[] */
    for (size_t i = 0; i < key_count; i++) {
        size_t slot = hash_str_lower(entries[i].key) % table_size;

        while (lut[slot] !=-1){
            if ( str_ieq(entries[lut[slot]].key, entries[i].key)) {
                next_idx[i] = lut[slot];
                break;
            }
            slot = (slot + 1) % table_size;
        }
        lut[slot] = (int)i;
    }

    /* Search pass: probe lut[] for term and walk any chain duplicates */
    size_t slot = hash_str_lower(term) % table_size;
    while (lut[slot] !=-1) {
        int curr = lut[slot];

        if (str_ieq(entries[curr].key, term)){ 
            const struct Qref *matches[total_items];
            size_t match_count = 0;
            
            while(curr != -1) {
                const struct Qref *candidate = entries[curr].qref;

                if(str_matches(entries[curr].key, term, candidate->case_mode)) {
                    /* Avoid duplicate Qref if an entry somehow lists the same alias twice  */
                    bool already_added = false;
                    for (size_t m = 0; m < match_count; m++) {
                        if (matches[m] == candidate) {
                            already_added = true;
                            break;
                        }
                    }

                    if (!already_added) {
                        matches[match_count++] = candidate;
                    }
                }
    

                curr = next_idx[curr];

                }

                if (match_count > 0) {
                    print_qrefs(matches, match_count, scope ? scope->description: "Global lookup");

                    return EXIT_SUCCESS;
            }
        }

        slot = (slot + 1) % table_size;
    }

    fprintf(stderr, "No entry found for '%s'\n", term);
    return EXIT_FAILURE;
}