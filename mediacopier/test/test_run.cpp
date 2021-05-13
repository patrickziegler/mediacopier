/* Copyright (C) 2020 Patrick Ziegler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <mediacopier/cli/run.hpp>

#include <gtest/gtest.h>
#include <log4cplus/configurator.h>

#include <cstdio>
#include <filesystem>

namespace cli = MediaCopier::Cli;
namespace fs = std::filesystem;

constexpr char test_data_dir[] = {TEST_DATA_DIR};

class RunTest : public ::testing::Test
{
protected:
    cli::ConfigManager f_config;
    cli::FeedbackProxy f_feedback;
    fs::path f_dataDir = std::string{test_data_dir};
    fs::path f_tmpDir;

    virtual void SetUp()
    {
        log4cplus::BasicConfigurator log;
        log.configure();

        f_tmpDir = fs::path(std::tmpnam(nullptr));
        fs::create_directories(f_tmpDir);
    }

    virtual void TearDown()
    {
        if (fs::is_directory(f_tmpDir)) {
            fs::remove_all(f_tmpDir);
        }
    }
};

TEST_F(RunTest, CopyOperationTest)
{
    f_config.setInputDir(f_dataDir);
    f_config.setOutputDir(f_tmpDir);
    f_config.setCommand(cli::Command::COPY);

    cli::run(f_config, f_feedback);

    // TODO: verify
}

TEST_F(RunTest, MoveOperationTest)
{
    auto dir1 = f_tmpDir / "copied";
    auto dir2 = f_tmpDir / "moved";

    fs::copy(f_dataDir, dir1);

    f_config.setInputDir(dir1);
    f_config.setOutputDir(dir2);
    f_config.setCommand(cli::Command::MOVE);

    cli::run(f_config, f_feedback);

    // TODO: verify
}
