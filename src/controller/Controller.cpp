#include "controller/Controller.hpp"
#include "model/AppState.hpp"
#include "view/View.hpp"
#include <CLI/CLI.hpp>

namespace app {

Controller::Controller(AppState& model, View& view)
    : model_(&model), view_(&view) {}

int Controller::run(int argc, char** argv) {
    CLI::App cli{"App", "app"};
    cli.set_version_flag("--version,-V", model_->version);

    // Global option (available for all subcommands)
    cli.add_flag("--verbose,-v", model_->verbose, "Verbose output");

    // --- Default: greet (no subcommand) ---
    cli.add_option("-n,--name", model_->name, "Name to greet")->capture_default_str();

    // --- Subcommand: build ---
    CLI::App* build = cli.add_subcommand("build", "Build the project");
    build->add_option("-t,--type", model_->build_type, "Build type (Debug|Release)")
         ->check(CLI::IsMember({"Debug", "Release"}))
         ->capture_default_str();
    build->add_flag("-c,--clean", model_->build_clean, "Clean before build");

    // --- Subcommand: run ---
    CLI::App* run_cmd = cli.add_subcommand("run", "Run a script");
    run_cmd->add_option("script", model_->run_script, "Script to run")->required();
    run_cmd->add_option("-a,--argument", model_->run_arguments, "Pass argument to script (repeatable)");

    // --- Subcommand: cache (with sub-subcommands) ---
    CLI::App* cache = cli.add_subcommand("cache", "Cache management");
    CLI::App* cache_clear = cache->add_subcommand("clear", "Clear the cache");
    cache_clear->add_option("--path", model_->cache_path, "Cache path to clear");
    cache_clear->add_option("-a,--argument", model_->cache_arguments, "Extra arguments (repeatable)");

    CLI::App* cache_list = cache->add_subcommand("list", "List cache entries");
    cache_list->add_option("--path", model_->cache_path, "Cache path to list");
    cache_list->add_option("-a,--argument", model_->cache_arguments, "Extra arguments (repeatable)");

    cache->require_subcommand(1);  // require exactly one: clear or list

    CLI11_PARSE(cli, argc, argv);

    // Dispatch by subcommand
    if (build->parsed()) {
        view_->showBuild(*model_);
        return 0;
    }
    if (run_cmd->parsed()) {
        view_->showRun(*model_);
        return 0;
    }
    if (cache->parsed()) {
        if (cache_clear->parsed()) {
            model_->cache_sub = CacheSubcommand::Clear;
            view_->showCacheClear(*model_);
        } else {
            model_->cache_sub = CacheSubcommand::List;
            view_->showCacheList(*model_);
        }
        return 0;
    }

    // No subcommand: default greet
    view_->greet(*model_);
    return 0;
}

} // namespace app
