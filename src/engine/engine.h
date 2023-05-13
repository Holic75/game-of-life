#pragma once

#include <vector>
#include <tuple>
#include <ostream>
#include <istream>
#include <algorithm>
#include <array>

namespace game_of_life {

struct CellEncoding {
  char alive_cell = '*';
  char dead_cell = '_';
  char row_separator = '\n';
  bool is_valid() { return alive_cell != dead_cell && alive_cell != row_separator && dead_cell != row_separator; }
};


enum class CellState {
  ALIVE,
  DEAD
};


struct Rectangle {
  size_t left;
  size_t top;
  size_t right;
  size_t bottom;
  size_t height() const { return bottom > top ? bottom - top : 0; }
  size_t length() const { return right > left ? right - left : 0; }
};


/// @brief Class representing current state of the game of life.
class Board {
private:
  std::vector<CellState> _cells;
  std::vector<size_t> _alive_cells_count_by_row;
  std::vector<size_t> _alive_cells_count_by_col;

public:
  /// @brief Construct zero-size board
  Board() {};
  /// @brief Construct the board with the specified size. All the board cells will initially be set to CellState::DEAD
  Board(size_t length, size_t height);
  /// @brief Reset the size to height x length. All the cells will be set to CellState::DEAD
  void reset(size_t length = 0, size_t height = 0);
  /// @brief Get state of the cell at row y and column x.
  CellState getCellState(int x, int y) const;
  /// @brief Get number of rows on the board
  size_t height() const { return _alive_cells_count_by_row.size(); }
  /// @brief Get number of columns on the board
  size_t length() const { return _alive_cells_count_by_col.size(); }
  /// @brief Read board from specified stream
  void load(std::istream& is, CellEncoding cell_encoding = {});
  /// @brief Write board area delimited by bounding_rect to specified stream. No boundary checks are performed
  void save(std::ostream& os, const Rectangle& bounding_rect, CellEncoding cell_encoding = {}) const;
  /// @brief Replace cell state at position specified by cell_description with the state specified by cell_description. 
  /// No boundary checks are performed on cell coordinates.
  void setCellState(size_t x, size_t y, CellState state);
  /// @brief Get number of alive neighbors of the cell with specified coordinates.
  /// Note coodrinates can be outside of the board bounds. All the cells outside of the board bounds
  /// are assumed to be dead
  size_t getAliveNeighborsCount(int x, int y) const;
  /// @brief Get rectangle coordinates, delimiting minimal board area necessary to fit all the living cells.
  Rectangle getAliveCellsBoundingRectangle() const;
};

/// @brief Class summarizing rules of the game of life.
struct Rules {
  size_t min_neighbors_to_survive = 2;
  size_t max_neighbors_to_survive = 3;
  size_t min_neighbors_to_spawn = 3;
  size_t max_neighbors_to_spawn = 3;
  /// @brief Check if the rules are not contradictory.
  /// @return True if the rules do not contradict, false otherwise.
  bool isValid() { 
    return max_neighbors_to_survive >= min_neighbors_to_survive && min_neighbors_to_spawn >= min_neighbors_to_spawn;
  }
  /// @brief Check if the living cell should die, based on the number of neighbors.
  /// @return True if the cell should die, false otherwise.
  bool cellShouldDie(size_t neighbors_count) const { 
    return neighbors_count < min_neighbors_to_survive || neighbors_count > max_neighbors_to_survive;
  }
  /// @brief Check if the living cell should spawn, based on the number of neighbors.
  /// @return True if the cell should spawn, false otherwise.
  bool cellShouldSpawn(size_t neighbors_count) const { 
    return neighbors_count >= min_neighbors_to_spawn && neighbors_count <= max_neighbors_to_spawn;
  }
};

/// @brief Class running iterations of the game of life.
class Engine {
private:
  std::array<Board, 2> _boards;
  Rules _rules;
  size_t _current_board_idx;
public:
  /// @brief Construct from board describing initial state and rules.
  Engine(Board board, const Rules& rules = {});
  /// @brief Return the board corresponding to the current state of the game.
  // The board size is undefined, but is guaranteed to fit all living cells
  const Board& board() { return _boards[_current_board_idx]; };
  /// @brief Transition to the next state of the game.
  void next();
};

}