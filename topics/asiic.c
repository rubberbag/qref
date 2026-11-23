#include "qref.h"

static const struct Qref data[] =
    {
        {"lf", "Control", "Line Feed (0x0A, 10)", CASE_INSENTIVE},
        {"cr", "Control", "Carriage Return (0x0D, 13", CASE_INSENTIVE},
        {"A", "Uppercase", "65(0x41)", CASE_SENSITIVE},
        {"a", "Lowercase", "97(0x61)", CASE_SENSITIVE}};

REGISTER_TOPIC(ascii, "qref ascii <term>", "ASCII character reference", data)