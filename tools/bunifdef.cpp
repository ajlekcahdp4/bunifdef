
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
  std::vector<std::string> undefs;
  auto all_desc = po::options_description{"All options"};
  auto hidden_desc = po::options_description{"Hidden options"};
  hidden_desc.add_options()("input-file", po::value(&input_file_name), "Input file name");
  hidden_desc.add_options()("dump-ast", "Dump AST");
  auto desc = po::options_description{"Allowed options"};
  desc.add_options()("help", "Produce help message");
  desc.add_options()("def,D", po::value(&defs)->composing(), "Define a value");
  desc.add_options()("undef,U", po::value(&undefs)->composing(), "Un-define a value");
  desc.add_options()("select,s", "Remove conditional selectevly instead of removing all of them");
  desc.add_options()(
      "output-file,o", po::value(&output_file)->default_value("-"), "Output file or - for stdout"
  );

  po::positional_options_description pos_desc;
  pos_desc.add("input-file", -1);
  all_desc.add(desc).add(hidden_desc);

  auto vm = po::variables_map{};
  po::store(po::command_line_parser(argc, argv).options(all_desc).positional(pos_desc).run(), vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return EXIT_SUCCESS;
  }

  po::notify(vm);

  if (input_file_name.empty()) {
    fmt::println(stderr, "Input file must be specified");
    return EXIT_FAILURE;
  }

  auto map = bunifdef::frontend::parse_defs(defs, undefs);
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
  if (vm.contains("dump-ast")) ast_dump(parsed_tree.get_root_ptr(), std::cout);
  bunifdef::frontend::semantic_analyzer analyzer(error_queue);

  analyzer.analyze(parsed_tree);
  for (auto &err : error_queue)
    reporter.report_pretty_error(err);
  if (!error_queue.empty()) return EXIT_FAILURE;
  std::stringstream ss;
  bunifdef::backend::process_text(parsed_tree, ss, map, vm.contains("select"));
  auto res = ss.str();
  // FIXME: remove inserting redundant newline character in the first place
  res.pop_back();
  if (output_file == "-") {
    std::cout << res;
  } else {
    std::fstream outfile(output_file, std::ostream::out);
    if (!outfile.good())
      throw std::runtime_error(fmt::format("Cannot open output file \"{}\"", output_file));
    outfile << res;
  }
} catch (bunifdef::frontend::internal_error &e) {
  fmt::println("INTERNAL ERROR: {}", e.what());
  return EXIT_FAILURE;
} catch (std::exception &e) {
  fmt::println("ERROR: {}", e.what());
  return EXIT_FAILURE;
}
