#include "model/AppState.hpp"
#include "view/View.hpp"
#include "controller/Controller.hpp"

int main(int argc, char** argv) {
    app::AppState model;
    app::View view;
    app::Controller controller(model, view);
    return controller.run(argc, argv);
}
