#include <GRender/entryPoint.h>

#include "graphDraw.hpp"

GRender::Application* GRender::createApplication(int argc, char** argv) {
    namespace fs = std::filesystem;
    // Processing inputs to use current path
    const fs::path pwd = fs::current_path();
    GRender::dialog::SetDefaultPath(pwd);

    // Setup program to use install path as reference
    const fs::path projPath = fs::canonical(argv[0]).parent_path().parent_path();
    fs::current_path(projPath);

    INFO("Project path: " + fs::current_path().string());
    return new GraphDraw;
}
