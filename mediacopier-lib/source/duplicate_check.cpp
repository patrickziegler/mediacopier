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

#include <vector>
#include <fstream>

namespace fs = std::filesystem;

constexpr static const size_t BUFFER_SIZE = 64;
constexpr static const size_t CHUNKS_MAX = 128;

static auto seek_jpeg_data(std::ifstream* input) noexcept -> bool
{
    uint8_t s1=0, s2=0;
    uint16_t buf=0;

    input->seekg(2, std::ios_base::beg);

    while (input->good()) {
        input->read(reinterpret_cast<char*>(&buf), 2);
        /* data begins after section code 0xdaff was found,
         * https://en.wikipedia.org/wiki/JPEG_File_Interchange_Format
         * */
        if (buf == 0xdaff) {
            return true;
        }
        input->read(reinterpret_cast<char*>(&s1), 1);
        input->read(reinterpret_cast<char*>(&s2), 1);
        input->seekg(256 * s1 + s2 - 2, std::ios_base::cur);
    }

    return false;
}

static auto prepare_input_stream(const fs::path& file) -> std::unique_ptr<std::ifstream>
{
    if (!fs::exists(file)) {
        throw std::runtime_error(file.string() + " does not exist");
    }
    auto input = std::make_unique<std::ifstream>(file,  std::ios_base::in | std::ios_base::binary);

    uint16_t magic;
    input->read(reinterpret_cast<char*>(&magic), 2);

    if (magic == 0xd8ff) { // is jpeg file
        seek_jpeg_data(input.get());
    }
    return std::move(input);
}

namespace mediacopier {

auto is_duplicate(const fs::path& file1, const fs::path& file2) -> bool
{
    const auto input1 = prepare_input_stream(file1);
    const auto input2 = prepare_input_stream(file2);

    std::vector<char> buffer(BUFFER_SIZE, '\0');

    std::string chunk1, chunk2;
    size_t chunks_left = CHUNKS_MAX;

    while (chunks_left > 0) {
        if (input1->eof() && input2->eof()) {
            break;
        } else if (!input1->good() || !input2->good()) {
            return false;
        }
        input1->read(buffer.data(), BUFFER_SIZE);
        chunk1 = {buffer.begin(), buffer.begin() + input1->gcount()};

        input2->read(buffer.data(), BUFFER_SIZE);
        chunk2 = {buffer.begin(), buffer.begin() + input2->gcount()};

        if (chunk1 != chunk2) {
            return false;
        }
        --chunks_left;
    }
    return true;
}

} // namespace mediacopier
