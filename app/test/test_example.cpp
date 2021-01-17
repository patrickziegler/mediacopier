#include <gtest/gtest.h>

#include <mediacopier/cli/run.hpp>

#include <filesystem>

namespace cli = MediaCopier::Cli;
namespace fs = std::filesystem;

TEST(testCopyTestData, completeWorkflowTests)
{
    cli::ConfigManager config;
    cli::FeedbackProxy feedback;

    config.setInputDir("./data/generated");
    config.setOutputDir("./data/copied");
    config.setCommand(cli::Command::COPY);
    cli::run(config, feedback);

    // TODO: verify

    fs::remove_all("./data/copied");
}

TEST(testMoveTestData, completeWorkflowTests)
{
    cli::ConfigManager config;
    cli::FeedbackProxy feedback;

    fs::copy("./data/generated", "./data/copied");

    config.setInputDir("./data/copied");
    config.setOutputDir("./data/moved");
    config.setCommand(cli::Command::MOVE);
    cli::run(config, feedback);

    // TODO: verify

    fs::remove_all("./data/copied");
    fs::remove_all("./data/moved");
}
