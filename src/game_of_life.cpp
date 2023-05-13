#include <iostream>
#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include "engine/engine.h"


struct Options {
  std::string input_filename = "";
  size_t num_iterations = 0;
  bool all = false;
};


std::tuple<Options, bool> processOptions(int argc, char **argv) {
  Options opts = {};
  bool is_ok = false;
  boost::program_options::options_description options_description("Allowed options");
  options_description.add_options()
    ("help", "see help message")
    ("input", boost::program_options::value(&opts.input_filename), "A string representing the input file path. This parameter is mandatory.")
    ("iterations", boost::program_options::value(&opts.num_iterations), "A positivie integer representing the number of iterations to apply the rules.")
    ("all", "Print all the iterations. This parameter is optional. If absent, only the last step is printed.");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_description).run(), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << options_description;
  } else if ( !vm.count("input") || !vm.count("iterations")) {
    std::cerr << options_description;
  } else {
    is_ok = true;
  }
  opts.all = vm.count("all");
  return {opts, is_ok};
};


game_of_life::Board loadBoardFromFile(const std::string& input_filename) {
  game_of_life::Board board;
  if (!std::filesystem::exists(input_filename) || std::filesystem::is_directory(input_filename)) {
    throw std::runtime_error(input_filename + " is not a valid path to an input file");
  }
  std::ifstream input_file(input_filename, std::ios::in | std::ios::binary);
  board.load(input_file);
  return board;
}


void runGame(game_of_life::Board board, const Options& opts) {
  game_of_life::Engine game_engine(std::move(board));

  auto input_path = std::filesystem::path(opts.input_filename);
  auto parent_path = input_path.parent_path();
  std::string stem = input_path.stem();
  std::string extension = input_path.extension();

  for (size_t it = 1; it <= opts.num_iterations; it++) {
    game_engine.next();
    if (it == opts.num_iterations || opts.all) {
      std::string output_filename = parent_path / (stem + "_" + std::to_string(it) + extension);
      std::ofstream output_file(output_filename, std::ios::out | std::ios::binary);
      auto alive_cell_bounding_rect = game_engine.board().getAliveCellsBoundingRectangle();
      game_engine.board().save(output_file, alive_cell_bounding_rect);
    }
  }
}


int main (int argc, char **argv) {
  auto [opts, is_ok] = processOptions(argc, argv);
  if (!is_ok) return 1;

  game_of_life::Board board;
  try {
    board = loadBoardFromFile(opts.input_filename);
  } catch (const std::exception& e) {
    std::cerr << "Failed to load data from input file: " << e.what() << std::endl;
    return 1;
  }

  runGame(std::move(board), opts);
  return 0;
}