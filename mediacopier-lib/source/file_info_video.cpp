/* Copyright (C) 2020 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/file_info_video.hpp>

#include <date/date.h>
#include <mediacopier/abstract_operation.hpp>
#include <mediacopier/error.hpp>

extern "C"
{
#include <libavformat/avformat.h>
}

#include <sstream>

namespace mediacopier {

FileInfoVideo::FileInfoVideo(std::filesystem::path path) : AbstractFileInfo{path}
{
    AVFormatContext* fmt_ctx = nullptr;
    AVDictionaryEntry* tag = nullptr;
    char errbuf[256];

    int ret = avformat_open_input(&fmt_ctx, path.string().c_str(), nullptr, nullptr);

    if (ret != 0) {
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::ostringstream ss;
        ss << "Could not read metadata: " << errbuf;
        throw FileInfoError{ss.str()};
    }

    tag = av_dict_get(fmt_ctx->metadata, "creation_time", nullptr, AV_DICT_IGNORE_SUFFIX);

    if (!tag) {
        avformat_close_input(&fmt_ctx);
        throw FileInfoError{"'creation_time' not found in metadata"};
    }

    // magic numbers assume the following format: 2018-01-01T01:01:01.000000Z
    std::istringstream ss{tag->value};
    ss >> date::parse("%Y-%m-%dT%H:%M:%S", m_timestamp);

    avformat_close_input(&fmt_ctx);

    if (m_timestamp == std::chrono::system_clock::time_point{}) {
        throw FileInfoError{"No date information found"};
    }
}

auto FileInfoVideo::accept(AbstractFileOperation& operation) const -> void
{
    operation.visit(*this);
}

} // namespace mediacopier
