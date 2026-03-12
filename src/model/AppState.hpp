#pragma once

#include <string>
#include <vector>

namespace app {

/// Top-level command: default (greet), build, run, or cache.
enum class Command {
    Greet,
    Build,
    Run,
    Cache,
};

/// Subcommand under "cache": clear or list.
enum class CacheSubcommand {
    None,
    Clear,
    List,
};

/// Application state (Model). No CLI or I/O dependencies.
struct AppState {
    std::string version = "0.0.1";
    bool verbose = false;

    // --- Greet (default) ---
    std::string name = "World";

    // --- Build subcommand ---
    std::string build_type = "Release";
    bool build_clean = false;

    // --- Run subcommand ---
    std::string run_script;
    std::vector<std::string> run_arguments;

    // --- Cache subcommand (and sub-subcommands) ---
    CacheSubcommand cache_sub = CacheSubcommand::None;
    std::string cache_path;
    std::vector<std::string> cache_arguments;
};

} // namespace app
