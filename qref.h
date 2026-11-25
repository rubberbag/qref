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
#ifndef QREF_H
#define QREF_H

#include <stddef.h>

#define MAX_ALIASES  4

/**
 * Controls whether an individual entry matches case-sensitively or not
 */
enum CaseMode
{
    CASE_SENSITIVE = 0,
    CASE_INSENSITIVE = 1
};

/**
 * Represents a single reference entry  within a topic.
 */
struct Qref
{
    const char *name; 
    const char *category; 
    const char *details;
    enum CaseMode case_mode;
    const char *aliases[MAX_ALIASES];  
};

/**
 * Represents a registered  CLI topic and its linked-list node.
 */
struct Subcommand
{
    const char *name;
    const char *usage;
    const char *description;
    const struct Qref *qrefs;
    size_t count;
    struct Subcommand *next;
};

void register_topic(struct Subcommand *cmd);
const struct Subcommand *find_subcommand(const char *name);
size_t registry_total_refs(void);
const struct Subcommand *registry_head(void);
void print_version(void);
void print_help(void);
int lookup_and_print(const char *item, const struct Subcommand *scope);
void print_qrefs(const struct Qref *const *items, size_t count, const char *title);


/**
 * Register a static array of Qref entries as a CLI topic before main() runs.
 */
#define REGISTER_TOPIC(topic, usage_str, desc_str, data_arr)          \
    static struct Subcommand _subcmd_##topic =                        \
        {                                                             \
            .name = #topic,                                           \
            .usage = (usage_str),                                     \
            .description = (desc_str),                                \
            .qrefs = (data_arr),                                      \
            .count = sizeof(data_arr) / sizeof((data_arr[0])),        \
            .next = nullptr,                                             \
    };                                                                \
    __attribute__((constructor)) static void _registers_##topic(void) \
    {                                                                 \
        register_topic(&_subcmd_##topic);                             \
    }

#endif /* QREF_H */