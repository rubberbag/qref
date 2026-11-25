# qref

A fast, zero-dependency command-line quick reference tool. Look up technical references, such as ASCII codes, system calls, or memory layouts instantly from your terminal without leaving your workflow.

## Features

- **Zero heap allocations at lookup time:** Builds an exact-sized, stack-allocated FNV-1a open-addressed hash table on the fly with linear probing and duplicate chaining.
- **Self-registering topic modules:** Drop any new `.c` file into `topics/` using the `REGISTER_TOPIC` macro and rebuild, no changes needed to `main`, `registry.c`, `lookup.c`, or the `makefile`.
- **Flexible CLI parsing:** Accepts flags (`-g`, `-h`, `-v`), topic scopes, and search terms in any order.
- **Per-entry case sensitivity:** Supports strict case-sensitive entries (`A` vs `a`) alongside case-insensitive entries (`lf` / `LF`) within the same topic.

## Build

Requires `gcc` (or `clang`) and `make`.

```bash
git clone https://github.com/rubberbag/qref.git
cd qref
make
```

The compiled binary is placed at `build/qref` alongside an automatic section size report (`text`, `data`, `bss`).

To clean build artifacts:

```bash
make clean
```

## Usage

```text
qref [options] [topic] <term>
```

### Options

| Flag | Description |
| :--- | :--- |
| `-h`, `--help` | Show usage information and all registered topics |
| `-v`, `--version` | Show version and license information |
| `-g`, `--global` | Force a global lookup across all topics (even if `<term>` matches a topic name) |

### Examples

```bash
# List all available topics
./build/qref --help

# Dump all entries in a topic (0 hash lookups)
./build/qref ascii

# Look up a term inside a specific topic (any argument order works)
./build/qref ascii lf
./build/qref lf ascii

# Case-sensitive vs. case-insensitive lookup
./build/qref ascii A
./build/qref ascii a
./build/qref ascii LF

# Global lookup across all registered topics
./build/qref lf
./build/qref -g ascii
```

## Adding a New Reference Topic

`qref` uses GCC/Clang constructor attributes (`__attribute__((constructor))`) to register topics automatically before `main()` runs.

1. Create a new `.c` file inside `topics/` (e.g., `topics/signals.c`):

```c
#include "qref.h"

static const struct Qref data[] =
{
    { "SIGINT",  "Termination", "2  - Interrupt from keyboard (Ctrl+C)", CASE_INSENSITIVE },
    { "SIGKILL", "Termination", "9  - Kill signal (cannot be caught)",   CASE_INSENSITIVE },
    { "SIGSEGV", "Fault",       "11 - Invalid memory reference",         CASE_INSENSITIVE }
};

REGISTER_TOPIC(signals, "qref signals <term>", "POSIX signal reference", data);
```

2. Run `make`. The `makefile` automatically discovers `topics/*.c` and compiles it into `build/qref`.

## Project Structure

```text
qref/
├── makefile          # Out-of-tree build into build/ with automatic topic discovery
├── qref.h            # Core structs (Qref, Subcommand), CaseMode enum, and REGISTER_TOPIC macro
├── qref.c            # CLI argument parser and execution router
├── registry.c        # Pre-main linked-list registry, help/version output, and table formatter
├── lookup.c          # Stack-allocated FNV-1a hash table with linear probing & duplicate chaining
├── topics/           # Modular reference data files
│   └── asiic.c
└── LICENSE           # GNU General Public License v3.0
```

## Contributing

Contributions, especially new reference modules under `topics/` are welcome! A dedicated `CONTRIBUTING.md` guide is coming soon. In the meantime, feel free to open an issue or submit a pull request.

## License

Copyright (C) 2026 `rubberbag`

See the [LICENSE](LICENSE) file for details.