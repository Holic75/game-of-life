#pragma once

#include <deque>
#include <tuple>
#include <ostream>
#include <istream>
#include <algorithm>

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

struct CellDescription {
  size_t y;
  size_t x;
  CellState state;
};


/// @brief Class representing current state of the game of life.
class Board {
private:
  std::deque<std::deque<CellState>> _cells;
  std::deque<size_t> _alive_cells_count_by_row;
  std::deque<size_t> _alive_cells_count_by_col;

public:
  Board() {};
  /// @brief Get state of the cell at row y and column x.
  /// No boundary checks are performed on cell coordinates.
  CellState getCellState(size_t y, size_t x) const { return _cells[y][x];};
  /// @brief Get number of rows on the board
  size_t height() const { return _cells.size(); }
  /// @brief Get number of columns on the board
  size_t length() const { return _cells.size() == 0 ? 0 : _cells[0].size(); }
  /// @brief Read board from specified stream
  void load(std::istream& is, CellEncoding cell_encoding = {});
  /// @brief Write board to specified stream
  void save(std::ostream& os, CellEncoding cell_encoding = {}) const;
  /// @brief  Remove all cells from the board setting its size to 0 x 0
  void clear();
  /// @brief Reduce the size of the board to the minimum required to fit all living cells
  void shrink();
  /// @brief Add row of dead cells to the top of the board
  void extendUp();
  /// @brief Add row of dead cells to the bottom of the board
  void extendDown();
  /// @brief Add column of dead cells to the left of the board
  void extendLeft();
  /// @brief Add column of dead cells to the right of the board
  void extendRight();
  /// @brief Replace cell state at position specified by cell_description with the state specified by cell_description. 
  /// No boundary checks are performed on cell coordinates.
  void replaceCell(const CellDescription& cell_description);
  /// @brief Replace states of the range of cells. 
  /// No boundary checks are performed on cell coordinates.
  /// If a cell with the same coordinates is listed more than once, its state will change to the state of the last entry.
  /// @tparam It Forward Iterator dereferenceable to CellDescription
  template<class It>
  void replaceCells(It begin, It end) { 
    std::for_each(begin, end, [this](const CellDescription& c) { replaceCell(c); });
  };
  /// @brief Get number of neighbors of the cell with specified coordinates, that are the state equal to cell_state. 
  /// No boundary checks are performed on cell coordinates.
  size_t getNeighborsCount(size_t y, size_t x, CellState cell_state = CellState::ALIVE) const;
};

/// @brief Class summarizing rules of the game of life.
struct Rules {
  size_t min_neighbors_to_survive = 2;
  size_t max_neighbors_to_survive = 3;
  size_t min_neighbors_to_spawn = 3;
  size_t max_neighbors_to_spawn = 3;
  /// @brief Check if the rules do not contradict
  /// @return True if the rules do not contradict, false otherwise
  bool isValid() { 
    return max_neighbors_to_survive >= min_neighbors_to_survive && min_neighbors_to_spawn >= min_neighbors_to_spawn;
  }
  /// @brief Check if the living cell should die, based on the number of neighbors
  /// @return True if the cell should die, false otherwise
  bool cellShouldDie(size_t neighbors_count) const { 
    return neighbors_count < min_neighbors_to_survive || neighbors_count > max_neighbors_to_survive;
  }
  /// @brief Check if the living cell should spawn, based on the number of neighbors
  /// @return True if the cell should spawn, false otherwise
  bool cellShouldSpawn(size_t neighbors_count) const { 
    return neighbors_count >= min_neighbors_to_spawn && neighbors_count <= max_neighbors_to_spawn;
  }
};

/// @brief Class running iterations of the game of life.
class Engine {
private:
  Board _board;
  Rules _rules;
  std::vector<CellDescription> _cells_to_modify_buffer; // to reduce time spent on memory allocations
public:
  /// @brief Construct from board describing initial state and rules
  Engine(Board board, const Rules& rules = {});
  /// @brief Return the board corresponding to the current state of the game
  const Board& board() { return _board; };
  /// @brief Transition to the next state of the game
  void next();
};

}