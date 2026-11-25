/*
 *SPDX-License-Identifier: GPL-3.0-or-later
 *Copyright (C) 2026 Philip Nerison Boadu
 */

#include "qref.h"

static const struct Qref data[] =
{
    {"lf", "Control",   "Line Feed (0x0A, 10)",       CASE_INSENSITIVE, {"10", "0x0A", "\\n", "NL"}},
    {"cr", "Control",   "Carriage Return (0x0D, 13)", CASE_INSENSITIVE, {"13", "0x0D", "\\r"}},
    {"A",  "Uppercase", "65 (0x41)",                  CASE_SENSITIVE,   {"65", "0x41"}},
    {"a",  "Lowercase", "97 (0x61)",                  CASE_SENSITIVE,   {"97", "0x61"}},
};

REGISTER_TOPIC(ascii, "qref ascii <term>", "ASCII character reference", data)
