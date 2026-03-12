#pragma once

namespace app {

struct AppState;

/// Output only (View). Prints to stdout.
class View {
public:
    void greet(const AppState& state) const;
    void showBuild(const AppState& state) const;
    void showRun(const AppState& state) const;
    void showCacheClear(const AppState& state) const;
    void showCacheList(const AppState& state) const;
};

} // namespace app
