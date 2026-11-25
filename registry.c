/*
 * qref - A fast, zero-dependency command-line quick reference tool
 * Copyright (C) 2026 Philip Nerison Boadu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://gnu.org>.
 */
 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "qref.h"

static struct  Subcommand *subcommand_head = nullptr;
static size_t subcommand_count = 0;
static size_t total_ref_count  = 0;


/**
 * Prepend a  topic subcommand to the global registry linked list.
 */
void register_topic(struct Subcommand *cmd)
{
    if(!cmd)
        return;

    cmd->next = subcommand_head;
    subcommand_head = cmd;
    subcommand_count++;
    total_ref_count += cmd->count;
}

/**
 * Find a registered topic by name, or retun nullptr if not found
 */
const struct Subcommand *find_subcommand(const char *name)
{
    const struct Subcommand *curr = subcommand_head;

    while (curr != nullptr) {
        if (strcmp(curr->name, name) ==0)
        {
            return curr;
        }

        curr = curr->next;
    }
    
    return nullptr;
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
    puts("  -g, --global        Force global query across all topics. e.g., qref -g ascii / qref ascii -g");

    puts("\nAvailable topics");
    while(curr !=nullptr) {
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
        return;

    printf("[+] %s (%zu %s)\n",
            title ? title: "Lookup Result",
            count,
            count == 1 ? "match" : "matches");
    puts("-------------------------------------------------------------");

    if (count == 1) {
        printf("ENTRY DETAILS:\n");
        printf("    [Name]           %s\n", items[0]->name);
        if (items[0]->aliases[0] != nullptr) {
            printf("    [Aliases]        ");
            for (size_t j = 0; j < MAX_ALIASES && items[0]->aliases[j] != nullptr; j++)
                printf("%s%s", j > 0 ? ", " : "", items[0]->aliases[j]);
            puts("");
        }
        printf("    [Category]       %s\n", items[0]->category);
        printf("    [Details]        %s\n", items[0]->details);

        return;
    }

    const char *last_cat = nullptr;
    for (size_t i = 0; i < count; i++) {

        if(!last_cat || strcmp(last_cat, items[i]->category) != 0) {

            if(last_cat != nullptr)
                puts("");

            for (const char *p = items[i]->category; *p; p++)
                putchar(toupper((unsigned char) *p));

            puts(":");

            last_cat = items[i]->category;
        }
        else 
            puts("");

        printf("    %-14s %s\n", "[name]", items[i]->name);
        printf("    %-14s %s\n", "[description]", items[i]->details);

        if (items[i]->aliases[0] != nullptr) {
            printf("    %-14s ", "[aliases]");
            for (size_t j = 0; j < MAX_ALIASES && items[i]->aliases[j] != nullptr; j++)
            {
                printf("%s%s", j > 0 ? ", " : "", items[i]->aliases[j]);
            }
            puts("");
        }
    }
}