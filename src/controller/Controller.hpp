#pragma once

namespace app {

struct AppState;
class View;

/// Parses CLI (CLI11), updates model, invokes view.
class Controller {
public:
    Controller(AppState& model, View& view);

    /// Parse argv and run. Returns exit code (0 on success).
    /// --help and --version are handled by CLI11 (may exit before returning).
    int run(int argc, char** argv);

private:
    AppState* model_;
    View* view_;
};

} // namespace app
