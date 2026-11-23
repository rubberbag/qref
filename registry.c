#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "qref.h"

static struct  Subcommand *subcommand_head = NULL;
static size_t subcommand_count = {};
static size_t total_ref_count  = {};


/**
 * Prepend a  topic subcommand to the global registry linked list.
 */
void register_topic(struct Subcommand *cmd)
{
    if(!cmd)
    {
        return;
    }

    cmd->next = subcommand_head;
    subcommand_head = cmd;
    subcommand_count++;
    total_ref_count += cmd->count;
}

/**
 * Find a registered topic by name, or retun NULL if not found
 */
const struct Subcommand *find_subcommand(const char *name)
{
    const struct Subcommand *curr = subcommand_head;

    while (curr != NULL)
    {
        if (strcmp(curr->name, name) ==0)
        {
            return curr;
        }

        curr = curr->next;
    }
    
    return NULL;
}


/**
 * Return the total number of Qref entries across all registered topics
 */
size_t registry_total_refs(void)
{
    return total_ref_count;
}


/**
 * Return the head of the registered  subcommands list (for global lookups).
 */
const struct Subcommand *registry_head(void)
{
    return subcommand_head;
};



#ifndef PROGRAM_VERSION
#define PROGRAM_VERSION "0.0.0"
#endif
/**
 * Print the program verison and copyright line
 */
void print_version(void)
{
    printf("qref v%s © 2026 by Philip Nerison Boadu\n", PROGRAM_VERSION);
}


/**
 * Print the program help
 */
void print_help(void)
{
    const struct Subcommand *curr = subcommand_head;

    puts("\nUsage: qref [topic] <term>");
    puts("       qref <topic>");

    puts("\nOptions:");
    puts("  -h, --help          Show this help message");
    puts("  -v, --version       Show version information");
    puts("  -g, --global        Force global query. e.g., qref -g \"ascii\"");

    puts("\nAvailable topics");
    while(curr !=NULL)
    {
    printf(" %-12s      %28s (%zu entries)\n",
            curr->name, curr->description, curr->count);
        curr = curr->next;
    }
}

/**
 * Print a box-drawn table of qref entry with dynamic sizeed columns
 */
void print_qrefs(const struct Qref *const *items, size_t count, const char *title)
{
    if (!items || count == 0)
    {
        return;
    }

    printf("[+] %s (%zu %s)\n",
            title ? title: "Lookup Result",
            count,
            count == 1 ? "match" : "matches");
    puts("-------------------------------------------------------------");

    if (count == 1)
    {
        printf("ENTRY DETAILS:\n");
        printf("    Name:           %s\n", items[0]->name);
        printf("    Category:       %s\n", items[0]->category);
        printf("    Details:        %s\n", items[0]->details);

        return;
    }

    const char *last_cat = NULL;
    for (size_t i = 0; i < count; i++)
    {
        if(!last_cat || strcmp(last_cat, items[i]->category) != 0)
        {
            if(last_cat != NULL)
            {
                puts("");
            }

            for (const char *p = items[i]->category; *p; p++)
            {
                putchar(toupper((unsigned char) *p));
            }
            puts(":");

            last_cat = items[i]->category;
        }
        printf("    %-8s - %s\n", items[i]->name, items[i]->details);
    }

}