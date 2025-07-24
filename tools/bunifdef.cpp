
#include "bunifdef/backend/evaluator.hpp"
#include "bunifdef/frontend/defs.hpp"
#include "bunifdef/frontend/driver.hpp"
#include "bunifdef/frontend/dumper.hpp"
#include "bunifdef/frontend/expr_expander.hpp"

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
#include <fmt/core.h>

namespace po = boost::program_options;
int main(int argc, char **argv) try {
  std::string input_file_name, output_file;
  bool dump_ast = false;
  std::vector<std::string> defs;
  auto desc = po::options_description{"Allowed options"};
  desc.add_options()("help", "Produce help message");
  desc.add_options()("input-file", po::value(&input_file_name), "Input file name");
  desc.add_options()("def,D", po::value(&defs)->composing(), "Definitions to use");
  desc.add_options()("dump-ast", po::value(&dump_ast), "Dump AST");
  desc.add_options()(
      "output-file,o", po::value(&output_file)->default_value("-"), "Output file or - for stdout"
  );

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

  auto map = bunifdef::frontend::parse_defs(defs);
  bunifdef::frontend::ast::ast_container parsed_tree;
  bunifdef::frontend::frontend_driver drv{input_file_name, parsed_tree};
  drv.parse();
  bunifdef::frontend::expand_directive_expressions(parsed_tree);
  if (dump_ast) ast_dump(parsed_tree.get_root_ptr(), std::cout);
  if (output_file == "-") {
    bunifdef::backend::process_text(parsed_tree, std::cout, map);
  } else {
    std::fstream outfile(output_file, std::ostream::out);
    if (!outfile.good())
      throw std::runtime_error(fmt::format("Cannot open output file \"{}\"", output_file));
    bunifdef::backend::process_text(parsed_tree, outfile, map);
  }
} catch (bunifdef::frontend::internal_error &e) {
  fmt::println("INTERNAL ERROR: {}", e.what());
  return EXIT_FAILURE;
} catch (std::exception &e) {
  fmt::println("ERROR: {}", e.what());
  return EXIT_FAILURE;
}
