# Command-Line Interface Guide (CLI11)

A complete guide to building a **professional command-line application** with [CLI11](https://github.com/CLIUtils/CLI11): options, flags, positional arguments, subcommands, validation, and best practices.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Basic setup](#2-basic-setup)
3. [Options (named arguments)](#3-options-named-arguments)
4. [Flags (boolean switches)](#4-flags-boolean-switches)
5. [Positional arguments](#5-positional-arguments)
6. [Subcommands](#6-subcommands)
   - [Nested subcommands (sub-subcommands)](#nested-subcommands-sub-subcommands)
7. [Validation and constraints](#7-validation-and-constraints)
8. [Help and version](#8-help-and-version)
9. [Option groups](#9-option-groups)
10. [Environment variables and config files](#10-environment-variables-and-config-files)
11. [Error handling and exit codes](#11-error-handling-and-exit-codes)
12. [Professional CLI design checklist](#12-professional-cli-design-checklist)
13. [Full example: multi-subcommand app](#13-full-example-multi-subcommand-app)

---

## 1. Overview

### What CLI11 gives you

- **Options** — `--file path`, `-f path`, with types (string, int, float, etc.)
- **Flags** — `--verbose`, `-v` (boolean, optionally counting: `-vvv`)
- **Positional arguments** — `app copy src dest`
- **Subcommands** — `app build`, `app run`, `app deploy`
- **Validation** — ranges, existing file, regex, custom validators
- **Auto-generated help** — `--help`, `-h` and per-subcommand help
- **Version** — `--version`, `-V` with one call

### Parsing flow

1. Create `CLI::App`.
2. Add options, flags, positionals, and/or subcommands.
3. Call `CLI11_PARSE(app, argc, argv)` (or `app.parse(...)` and handle exceptions).
4. Use the bound variables after parse succeeds.

---

## 2. Basic setup

### Include and parse

```cpp
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {
    CLI::App app{"My Tool", "my_tool"};
    // ... add options ...

    CLI11_PARSE(app, argc, argv);
    return 0;
}
```

`CLI11_PARSE` handles `--help` and `--version` (if set), prints errors to stderr, and exits the process with a non-zero code on parse failure. For custom handling, use `try { app.parse(argc, argv); } catch (const CLI::ParseError& e) { return app.exit(e); }`.

### Binding to variables

Options and flags bind to variables; CLI11 writes parsed values into them:

```cpp
std::string file;
int count = 1;
bool verbose = false;

app.add_option("-f,--file", file, "Input file");
app.add_option("-n,--count", count, "Repeat count");
app.add_flag("--verbose,-v", verbose, "Verbose output");
```

---

## 3. Options (named arguments)

### Syntax

- **Long:** `--name`, `--output-file`
- **Short:** `-n`, `-o`
- **Combined:** `-n,--name` (same option, multiple names)
- **Value:** next argument or `--name=value`

### Types

CLI11 infers type from the bound variable:

```cpp
std::string path;           // TEXT
app.add_option("-f,--file", path, "File path");

int port = 8080;            // INT
app.add_option("-p,--port", port, "Port number");

float scale = 1.0f;         // FLOAT
app.add_option("-s,--scale", scale, "Scale factor");

std::vector<std::string> files;  // multiple values
app.add_option("-i,--input", files, "Input files");
```

### Defaults and capture in help

- Set the variable before `add_option` to get a default.
- Use `->capture_default_str()` so the default appears in `--help`:

```cpp
int limit = 100;
app.add_option("--limit", limit, "Max items")->capture_default_str();
// Help shows: --limit INT [100]
```

### Required options

```cpp
app.add_option("-f,--file", file, "Input file")->required();
```

### Multiple values (vector)

```cpp
std::vector<std::string> inputs;
app.add_option("-i,--input", inputs, "Input files");
// Usage: app -i a.txt -i b.txt  or  app -i a.txt b.txt
```

### Taking only the last value

If an option is given multiple times, by default all are stored. To keep only the last:

```cpp
app.add_option("--config", config_path, "Config file")->take_last();
```

---

## 4. Flags (boolean switches)

### Simple flag

```cpp
bool verbose = false;
app.add_flag("--verbose,-v", verbose, "Verbose output");
```

### Flag that counts (e.g. -v, -vv, -vvv)

```cpp
int verbosity = 0;
app.add_flag("--verbose,-v", verbosity, "Verbosity level");
// User can pass -v -v -v or -vvv
```

### Flag with default true

```cpp
bool no_color = false;
app.add_flag("--no-color", no_color, "Disable colored output");
// Then use: bool use_color = !no_color;
```

---

## 5. Positional arguments

Positional arguments are options **without** a leading `-` or `--` in the definition. They consume arguments in order.

### Single positional

```cpp
std::string input_file;
app.add_option("input", input_file, "Input file")->required();
// Usage: app <input>
```

### Multiple positionals

```cpp
std::string src, dest;
app.add_option("source", src, "Source path")->required();
app.add_option("destination", dest, "Destination path")->required();
// Usage: app copy <source> <destination>
```

### Variable number (vector)

```cpp
std::vector<std::string> files;
app.add_option("files", files, "Files to process");
// Usage: app file1.txt file2.txt ...
```

### Positional vs options

- Options can appear in any order: `app -o out.txt -i in.txt`.
- Positionals must appear in the defined order and, by default, after options. Use `->required()` so the app fails clearly if they are missing.

---

## 6. Subcommands

Subcommands let you structure the CLI as `app <subcommand> [options]`, e.g. `git status`, `git commit`.

### Adding a subcommand

```cpp
CLI::App* build = app.add_subcommand("build", "Build the project");
CLI::App* run   = app.add_subcommand("run", "Run the application");

// Add options to subcommands
std::string build_type = "Release";
build->add_option("-t,--type", build_type, "Build type")->check(CLI::IsMember({"Debug", "Release"}));

// After parse, check which subcommand was used
CLI11_PARSE(app, argc, argv);

if (app.got_subcommand(build)) {
    // user ran: app build [options]
}
if (app.got_subcommand(run)) {
    // user ran: app run [options]
}
```

### Subcommand required (exactly one)

To require exactly one subcommand (no default top-level behavior):

```cpp
app.require_subcommand(1);  // exactly 1
// or
app.require_subcommand(0, 1);  // 0 or 1
```

### Options only on subcommands

Define options on the subcommand `CLI::App*`, not on the root app, so they only apply when that subcommand is used.

### Chained subcommands

You can add subcommands to subcommands (e.g. `app cache clear`, `app cache list`):

```cpp
CLI::App* cache = app.add_subcommand("cache", "Cache management");
CLI::App* clear = cache->add_subcommand("clear", "Clear the cache");
CLI::App* list  = cache->add_subcommand("list", "List cache entries");
```

---

## 7. Validation and constraints

### Built-in validators

Attach with `->check(...)`:

```cpp
// Integer in range
app.add_option("--port", port, "Port")->check(CLI::Range(1, 65535));

// Float in range
app.add_option("--ratio", ratio, "Ratio")->check(CLI::Range(0.0, 1.0));

// One of a set of strings
app.add_option("--mode", mode, "Mode")->check(CLI::IsMember({"fast", "normal", "slow"}));

// Existing file (path must exist)
app.add_option("-f,--file", path, "File")->check(CLI::ExistingFile);

// Existing path (file or directory)
app.add_option("--path", path, "Path")->check(CLI::ExistingPath);

// Nonexistent path (for output files)
app.add_option("-o,--output", path, "Output")->check(CLI::NonexistentPath);

// Positive number
app.add_option("--count", n, "Count")->check(CLI::PositiveNumber);
```

### Custom validator

```cpp
->check([](const std::string& s) {
    if (s.size() < 3) return "Name must be at least 3 characters";
    return std::string();  // empty = valid
})
```

### Required and optional

- `->required()` — option or positional must be provided.
- Default is optional; combine with `->check()` for validated required options.

---

## 8. Help and version

### Version flag

```cpp
app.set_version_flag("--version,-V", "1.0.0");
// or from a variable
app.set_version_flag("--version,-V", model.version);
```

When the user passes `--version` or `-V`, CLI11 prints the version and exits (exit code 0).

### Help flag

`-h` and `--help` are added automatically. They print the app (or subcommand) description and options and exit.

### Customizing description and footer

```cpp
CLI::App app{"My App", "my_app"};
app.description("A short one-line description.");
app.footer("Examples:\n  my_app build --release\n  my_app run script.lua");
```

### Subcommand description

Set when adding the subcommand: `add_subcommand("build", "Build the project")`.

---

## 9. Option groups

Group options in help under a heading:

```cpp
CLI::Option_group* io = app.add_option_group("I/O", "Input and output");
io->add_option("-i,--input", input, "Input file");
io->add_option("-o,--output", output, "Output file");

CLI::Option_group* opts = app.add_option_group("Options", "Other options");
opts->add_flag("--verbose", verbose, "Verbose");
```

---

## 10. Environment variables and config files

### Reading from environment

CLI11 does not read env vars by default. You can fill a variable from the environment before parse, then let CLI11 override it from argv:

```cpp
std::string config_path = std::getenv("MY_APP_CONFIG") ? std::getenv("MY_APP_CONFIG") : "";
app.add_option("--config", config_path, "Config file");
// User can override: app --config other.json
```

### Config file (INI/YAML)

CLI11 can load options from a config file and merge with command line. See the [CLI11 config file documentation](https://cliutils.github.io/CLI11/book/chapters/config.html) for `set_config()` and `config_file` behavior.

---

## 11. Error handling and exit codes

### CLI11_PARSE

- On parse failure (unknown option, validation error, required missing), prints to stderr and exits with non-zero code. You don’t need to check return value; the process exits.

### Manual parse and exit

```cpp
try {
    app.parse(argc, argv);
} catch (const CLI::ParseError& e) {
    return app.exit(e);  // prints error and returns exit code
}
```

### Exit codes

- Success: `0`.
- Parse/validation failure: CLI11 uses a non-zero code (e.g. 1). You can return custom codes from `main` after doing your own logic (e.g. file not found → 2).

### Custom validation after parse

If you need checks that span multiple options (e.g. “if A then B required”):

```cpp
CLI11_PARSE(app, argc, argv);
if (output_dir.empty() && !input_file.empty()) {
    std::cerr << "Error: --output required when --input is set.\n";
    return 1;
}
```

---

## 12. Professional CLI design checklist

- **Name and description** — Set `CLI::App{"Name", "binary_name"}` and `app.description("...")`.
- **--help / -h** — Always available (default in CLI11).
- **--version / -V** — Use `set_version_flag` for version and exit.
- **Consistent option style** — Prefer long `--option`; add short `-o` for common ones. Use `-` for single-letter, `--` for words.
- **Positionals** — Use for the “main” things (e.g. input file, subcommand); use options for modifiers.
- **Subcommands** — Use when the app has distinct actions (build, run, deploy). Require exactly one when there is no default action.
- **Defaults** — Set sensible defaults and use `->capture_default_str()` so help shows them.
- **Validation** — Use `->check()` for ranges, enums, file existence; add `->required()` where needed.
- **Errors** — Let CLI11 handle parse errors; use clear messages for your own checks.
- **Documentation** — Use short, clear option descriptions; add `footer()` with examples.
- **Exit codes** — 0 success; non-zero for errors (and document them if you use custom codes).

---

## 13. Full example: multi-subcommand app

Example layout: `app [global-opts] <subcommand> [subcommand-opts]`.

```cpp
#include <CLI/CLI.hpp>
#include <fmt/core.h>
#include <iostream>

int main(int argc, char** argv) {
    CLI::App app{"Tool", "tool"};
    app.set_version_flag("--version,-V", "1.0.0");

    // Global options
    bool global_verbose = false;
    app.add_flag("--verbose,-v", global_verbose, "Verbose output");

    // Subcommand: build
    CLI::App* build = app.add_subcommand("build", "Build the project");
    std::string build_type = "Release";
    build->add_option("-t,--type", build_type, "Build type")
         ->check(CLI::IsMember({"Debug", "Release"}));

    // Subcommand: run
    CLI::App* run = app.add_subcommand("run", "Run the application");
    std::string script;
    run->add_option("script", script, "Script to run")->required();

    // Subcommand: copy (with positionals)
    CLI::App* copy = app.add_subcommand("copy", "Copy files");
    std::string src, dest;
    copy->add_option("source", src, "Source")->required();
    copy->add_option("dest", dest, "Destination")->required();

    CLI11_PARSE(app, argc, argv);

    if (build->parsed()) {
        fmt::print("Building in {} mode.\n", build_type);
        if (global_verbose) fmt::print("(verbose)\n");
    } else if (run->parsed()) {
        fmt::print("Running script: {}\n", script);
    } else if (copy->parsed()) {
        fmt::print("Copy {} -> {}\n", src, dest);
    } else {
        fmt::print("Run with --help or a subcommand: build, run, copy.\n");
    }
    return 0;
}
```

Example runs:

```bash
tool --help
tool --version
tool build --type Debug
tool build -t Release -v
tool run myscript.lua
tool copy file.txt backup.txt
```

---

## Quick reference

| Task              | CLI11 API |
|-------------------|-----------|
| Option            | `add_option("-x,--name", var, "description")` |
| Flag              | `add_flag("--verbose,-v", var, "description")` |
| Positional        | `add_option("name", var, "description")` (no `-`) |
| Subcommand        | `add_subcommand("cmd", "description")` |
| Required          | `->required()` |
| Default in help   | `->capture_default_str()` |
| Range             | `->check(CLI::Range(min, max))` |
| Enum              | `->check(CLI::IsMember({"a","b"}))` |
| Existing file     | `->check(CLI::ExistingFile)` |
| Version           | `app.set_version_flag("--version,-V", "1.0.0")` |
| Which subcommand  | `app.got_subcommand(subcommand_ptr)` or `subcommand->parsed()` |

For more detail, see the [CLI11 book](https://cliutils.github.io/CLI11/book/) and [GitHub repo](https://github.com/CLIUtils/CLI11).
