
#include "bunifdef/backend/evaluator.hpp"
#include "bunifdef/frontend/defs.hpp"
#include "bunifdef/frontend/driver.hpp"
#include "bunifdef/frontend/dumper.hpp"
#include "bunifdef/frontend/expr_expander.hpp"
#include "bunifdef/frontend/semantic_analyzer.hpp"
#include "bunifdef/frontend/source.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
#include <fmt/core.h>

namespace po = boost::program_options;
int main(int argc, char **argv) try {
  std::string input_file_name, output_file;
  std::vector<std::string> defs;
  auto desc = po::options_description{"Allowed options"};
  desc.add_options()("help", "Produce help message");
  desc.add_options()("input-file", po::value(&input_file_name), "Input file name");
  desc.add_options()("def,D", po::value(&defs)->composing(), "Definitions to use");
  desc.add_options()("dump-ast", "Dump AST");
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
  std::vector<bunifdef::frontend::error_report> error_queue;
  bunifdef::frontend::source_input source(input_file_name);
  bunifdef::frontend::error_reporter reporter(source);
  bunifdef::frontend::ast::ast_container parsed_tree;
  bunifdef::frontend::frontend_driver drv{source, parsed_tree, error_queue};
  drv.parse();
  for (auto &err : error_queue)
    reporter.report_pretty_error(err);
  if (!error_queue.empty()) return EXIT_FAILURE;
  bunifdef::frontend::expand_directive_expressions(parsed_tree, error_queue);
  for (auto &err : error_queue)
    reporter.report_pretty_error(err);
  if (!error_queue.empty()) return EXIT_FAILURE;
  if (vm.count("dump-ast")) ast_dump(parsed_tree.get_root_ptr(), std::cout);
  bunifdef::frontend::semantic_analyzer analyzer(error_queue);

  analyzer.analyze(parsed_tree);
  for (auto &err : error_queue)
    reporter.report_pretty_error(err);
  if (!error_queue.empty()) return EXIT_FAILURE;
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
