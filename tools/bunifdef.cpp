
#include "bunifdef/backend/evaluator.hpp"
#include "bunifdef/frontend/driver.hpp"
#include "bunifdef/frontend/dumper.hpp"
#include "bunifdef/frontend/expr_expander.hpp"

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
#include <fmt/core.h>

namespace po = boost::program_options;
int main(int argc, char **argv) try {
  std::string input_file_name;
  auto desc = po::options_description{"Allowed options"};
  desc.add_options()("help", "Produce help message");
  desc.add_options()("input-file", po::value(&input_file_name), "Input file name");

  po::positional_options_description pos_desc;
  pos_desc.add("input-file", -1);

  auto vm = po::variables_map{};
  po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return EXIT_SUCCESS;
  }

  po::notify(vm);

  if (input_file_name.empty()) {
    fmt::println(stderr, "Input file must be specified");
    return EXIT_FAILURE;
  }
  bunifdef::frontend::ast::ast_container parsed_tree;
  bunifdef::frontend::frontend_driver drv{input_file_name, parsed_tree};
  drv.parse();
  bunifdef::frontend::expand_directive_expressions(parsed_tree);
  ast_dump(parsed_tree.get_root_ptr(), std::cout);
  bunifdef::backend::process_text(parsed_tree, std::cerr, {});

} catch (bunifdef::frontend::internal_error &e) {
  fmt::println("INTERNAL ERROR: {}", e.what());
  return EXIT_FAILURE;
} catch (std::exception &e) {
  fmt::println("ERROR: {}", e.what());
  return EXIT_FAILURE;
}
