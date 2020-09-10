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

#include <date/date.h>
#include <mediacopier/AbstractFileOperation.hpp>
#include <mediacopier/FileInfoVideo.hpp>

extern "C"
{
#include <libavformat/avformat.h>
}

#include <sstream>

namespace mc = MediaCopier;

mc::FileInfoVideo::FileInfoVideo(std::filesystem::path path) : AbstractFileInfo{path}
{
    AVFormatContext* fmt_ctx = nullptr;
    AVDictionaryEntry* tag = nullptr;

    if (!avformat_open_input(&fmt_ctx, path.c_str(), nullptr, nullptr)) {
        if ((tag = av_dict_get(fmt_ctx->metadata, "creation_time", nullptr, AV_DICT_IGNORE_SUFFIX))) {

            // magic numbers assume the following format: 2018-01-01T01:01:01.000000Z
            std::string datetime{tag->value};

            std::istringstream ss{datetime.substr(0, 19)};
            ss >> date::parse("%Y-%m-%dT%H:%M:%S", m_timestamp);

            auto us = std::stol(datetime.substr(20, 6));
            m_timestamp += std::chrono::microseconds{us};
        }
    }

    avformat_close_input(&fmt_ctx);
}

int mc::FileInfoVideo::accept(const AbstractFileOperation& operation) const
{
    return operation.visit(*this);
}
