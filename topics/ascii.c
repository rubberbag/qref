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
 
#include "qref.h"

static const struct Qref data[] =
{
    {"lf", "Control",   "Line Feed (0x0A, 10)",       CASE_INSENSITIVE, {"10", "0x0A", "\\n", "NL"}},
    {"cr", "Control",   "Carriage Return (0x0D, 13)", CASE_INSENSITIVE, {"13", "0x0D", "\\r"}},
    {"A",  "Uppercase", "65 (0x41)",                  CASE_SENSITIVE,   {"65", "0x41"}},
    {"a",  "Lowercase", "97 (0x61)",                  CASE_SENSITIVE,   {"97", "0x61"}},
};

REGISTER_TOPIC(ascii, "qref ascii <term>", "ASCII character reference", data)
