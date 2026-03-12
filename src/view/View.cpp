#include "view/View.hpp"
#include "model/AppState.hpp"
#include <fmt/core.h>

namespace app {

void View::greet(const AppState& state) const {
    if (state.verbose)
        fmt::print("Greeting {}.\n", state.name);
    fmt::print("Hello, {}!\n", state.name);
}

void View::showBuild(const AppState& state) const {
    if (state.verbose)
        fmt::print("Build (verbose).\n");
    fmt::print("Build type: {}, clean: {}\n", state.build_type, state.build_clean);
}

void View::showRun(const AppState& state) const {
    if (state.verbose)
        fmt::print("Run (verbose).\n");
    fmt::print("Script: {}\n", state.run_script);
    if (!state.run_arguments.empty()) {
        fmt::print("Arguments:");
        for (const auto& a : state.run_arguments)
            fmt::print(" {}", a);
        fmt::print("\n");
    }
}

void View::showCacheClear(const AppState& state) const {
    if (state.verbose)
        fmt::print("Cache clear (verbose).\n");
    fmt::print("Clearing cache");
    if (!state.cache_path.empty())
        fmt::print(" at {}", state.cache_path);
    fmt::print(".\n");
    if (!state.cache_arguments.empty()) {
        fmt::print("Extra args:");
        for (const auto& a : state.cache_arguments)
            fmt::print(" {}", a);
        fmt::print("\n");
    }
}

void View::showCacheList(const AppState& state) const {
    if (state.verbose)
        fmt::print("Cache list (verbose).\n");
    fmt::print("Listing cache");
    if (!state.cache_path.empty())
        fmt::print(" at {}", state.cache_path);
    fmt::print(".\n");
    if (!state.cache_arguments.empty()) {
        fmt::print("Extra args:");
        for (const auto& a : state.cache_arguments)
            fmt::print(" {}", a);
        fmt::print("\n");
    }
}

} // namespace app
