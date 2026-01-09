/* Copyright (C) 2023 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/duplicate_check.hpp>

#include <array>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

constexpr static const size_t BUFFER_SIZE = 64;
constexpr static const size_t CHUNKS_MAX = 128;

// as defined in https://en.wikipedia.org/wiki/JPEG_File_Interchange_Format
constexpr static const std::array<uint8_t, 2> JPEG_SOS { 0xFF, 0xDA };
constexpr static const std::array<uint8_t, 2> JPEG_SOI { 0xFF, 0xD8 };

static auto seek_jpeg_data(std::ifstream* input) noexcept -> bool
{
    uint16_t s1 = 0, s2 = 0;
    std::array<char, 2> buf {};

    input->seekg(2, std::ios_base::beg);

    while (input->good()) {
        input->read(buf.data(), buf.size());
        if (static_cast<unsigned char>(buf[0]) == JPEG_SOS[0]
            && static_cast<unsigned char>(buf[1]) == JPEG_SOS[1]) {
            return true;
        }
        input->read(buf.data(), buf.size());
        s1 = static_cast<unsigned char>(buf[0]);
        s2 = static_cast<unsigned char>(buf[1]);

        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        auto segment_length = (s1 << 8) | s2;
        input->seekg(segment_length - 2, std::ios_base::cur);
    }

    return false;
}

static auto prepare_input_stream(const fs::path& file) -> std::unique_ptr<std::ifstream>
{
    if (!fs::exists(file)) {
        throw std::runtime_error(file.string() + " does not exist");
    }
    auto input = std::make_unique<std::ifstream>(file, std::ios_base::in | std::ios_base::binary);

    std::array<char, 2> magic {};
    input->read(magic.data(), magic.size());

    if (static_cast<unsigned char>(magic[0]) == JPEG_SOI[0]
        && static_cast<unsigned char>(magic[1]) == JPEG_SOI[1]) { // is jpeg file
        seek_jpeg_data(input.get());
    }
    return input;
}

namespace mediacopier {

auto is_duplicate(const fs::path& file1, const fs::path& file2) -> bool
{
    const auto input1 = prepare_input_stream(file1);
    const auto input2 = prepare_input_stream(file2);

    std::vector<char> buf(BUFFER_SIZE, '\0');

    std::string chunk1, chunk2;
    size_t chunks_left = CHUNKS_MAX;

    while (chunks_left > 0) {
        if (input1->eof() && input2->eof()) {
            break;
        } else if (!input1->good() || !input2->good()) {
            return false;
        }
        input1->read(buf.data(), BUFFER_SIZE);
        chunk1 = { buf.begin(), buf.begin() + input1->gcount() };

        input2->read(buf.data(), BUFFER_SIZE);
        chunk2 = { buf.begin(), buf.begin() + input2->gcount() };

        if (chunk1 != chunk2) {
            return false;
        }
        --chunks_left;
    }
    return true;
}

} // namespace mediacopier
