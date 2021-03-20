/* Copyright (C) 2021 Patrick Ziegler
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

#include "feedback.hpp"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

namespace cli = MediaCopier::Cli;

void cli::FeedbackProxy::log(LogLevel level, std::string message)
{
    static auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("log"));

    switch (level)
    {
    case LogLevel::INFO:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT(message));
        break;

    case LogLevel::WARNING:
        LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(message));
        break;

    default:
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(message));
    }
}

void cli::FeedbackProxy::progress(size_t value)
{
    static auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("progress"));

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Progress " << value << " %"));
}
