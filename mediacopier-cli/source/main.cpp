#include <CLI/CLI.hpp>

#include <iostream>

int main(int argc, char** argv) {
    CLI::App app{"App description"};
    //argv = app.ensure_utf8(argv);

    app.set_version_flag("-v,--version", "1.0.0");

    std::string pattern = "default";
    std::string filename = "default file";
    std::string command = "none";
    bool slim = false;

    auto copyapp = app.add_subcommand("copy", "Copy some files");
    copyapp->add_option("-p,--pattern", pattern, "A help string");
    copyapp->callback([&command]() { command = "copy"; });
    auto opt = copyapp->add_option("filename", filename);
    opt->required();

    auto moveapp = app.add_subcommand("move", "Move some files");
    moveapp->add_option("-p,--pattern", pattern, "A help string");

    auto simapp = app.add_subcommand("simulate", "Do nothing, just simulate what would happen");
    simapp->add_option("-p,--pattern", pattern, "A help string");

    auto guiapp = app.add_subcommand("gui");
    guiapp->add_flag("--slim", slim, "Instantiate the slim version");

    CLI11_PARSE(app, argc, argv);

    std::cout << pattern << std::endl
              << filename << std::endl
              << command << std::endl
              << slim << std::endl;
    return 0;
}
