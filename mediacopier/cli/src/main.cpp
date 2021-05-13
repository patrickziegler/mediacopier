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

#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <mediacopier/OperationExecutor.hpp>

#include "config.hpp"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    log4cplus::BasicConfigurator log;
    log.configure();

    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    ConfigManager config;
    config.parseArgs(argc, argv);

    MediaCopier::OperationExecutor executor{
        config.command(), config.inputDir(), config.outputDir(), config.baseFormat()
    };

    try {
        executor.run();
    } catch (const std::exception& err) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(err.what()));
        return 1;
    }

    return 0;
}
