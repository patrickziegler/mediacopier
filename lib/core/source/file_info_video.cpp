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

#include <mediacopier/abstract_operation.hpp>
#include <mediacopier/error.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

#include <chrono>
#include <sstream>

namespace mediacopier {

FileInfoVideo::FileInfoVideo(std::filesystem::path path)
    : AbstractFileInfo { path }
{
    AVFormatContext* fmt_ctx = nullptr;
    AVDictionaryEntry* tag = nullptr;
    std::array<char, 256> errbuf {}; // NOLINT(cppcoreguidelines-avoid-magic-numbers)

    int ret = avformat_open_input(&fmt_ctx, path.string().c_str(), nullptr, nullptr);

    if (ret != 0) {
        av_strerror(ret, errbuf.data(), sizeof(errbuf));
        std::ostringstream oss;
        oss << "Could not read metadata: " << errbuf.data();
        throw FileInfoError { oss.str() };
    }

    std::string timestamp;

    // 'creation_time' is the safe fallback that should be available everywhere, but it's utc
    tag = av_dict_get(fmt_ctx->metadata, "creation_time", nullptr, AV_DICT_IGNORE_SUFFIX);
    if (tag) {
        timestamp = tag->value;
    }

    // apple stores timezone offsets along withe timestamp, we use this info if it is available
    tag = av_dict_get(fmt_ctx->metadata, "com.apple.quicktime.creationdate", nullptr, AV_DICT_IGNORE_SUFFIX);
    if (tag) {
        timestamp = tag->value;
    }

    avformat_close_input(&fmt_ctx);

    if (timestamp.empty()) {
        throw FileInfoError { "No date information found" };
    }

    std::chrono::system_clock::time_point tp;

    std::istringstream iss { timestamp };
    iss >> std::chrono::parse("%FT%T", tp); // parse into local time (without timezone offset)
    if (iss.fail()) {
        throw FileInfoError { "Invalid date information found" };
    }

    m_timestamp = tp;

    iss.seekg(0, std::ios::beg); // we want to parse the timestamp once more
    iss.clear();
    iss >> std::chrono::parse("%FT%T%z", tp); // parse into utc
    if (!iss.fail()) {
        m_offset = std::chrono::duration_cast<std::chrono::minutes>(m_timestamp - tp);
    }
}

auto FileInfoVideo::accept(AbstractFileOperation& operation) const -> void
{
    operation.visit(*this);
}

} // namespace mediacopier
