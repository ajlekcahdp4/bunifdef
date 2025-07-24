#pragma once

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/core.h>

namespace bunifdef::frontend {

inline std::string read_file(const std::string &filename, bool append_newline) {
  std::fstream ifile{filename, std::istream::in};
  if (!ifile.good()) throw std::runtime_error(fmt::format("failed to open file \"{}\"", filename));
  std::istreambuf_iterator<char> begin{ifile}, end;
  if (append_newline) {
    // FIXME: remove \n workaround
    return "\n" + std::string{begin, end};
  }
  return std::string{begin, end};
}

class source_input {
private:
  std::unique_ptr<std::string> m_filename; // Name of the source file
  // Unique pointer to keep the underlying pointer the same, even when the object is moved from.

private:
  std::string m_file_source; // Raw file representation as a string
  using line_vec = std::vector<std::string>;
  line_vec m_file_lines; // Vector of lines split

private:
  void fill_lines() {
    std::istringstream iss{m_file_source};
    for (std::string line; std::getline(iss, line);) {
      m_file_lines.push_back(line);
    }
  }

public:
  source_input(const std::filesystem::path &input_path, bool append_newline = true)
      : m_filename{std::make_unique<std::string>(input_path.string())},
        m_file_source{read_file(input_path, append_newline)} {
    fill_lines();
  }

  std::string_view getline(unsigned i) const & {
    assert(i != 0 && "Line number can't be equal to 1");
    return m_file_lines.at(i - 1); /* Bison lines start with 1, so we have to subtrack */
  }

  // Can't make this const qualified, because bison location requires it be a modifiable pointer for
  // whatever reason. Note that the string is allocated on the heap to avoid issues with default
  // constructors.
  std::string *filename() & { return m_filename.get(); }
  std::string_view get_filename() const { return *m_filename.get(); }
  std::istringstream iss() const & { return std::istringstream{m_file_source}; }
};

} // namespace bunifdef::frontend
