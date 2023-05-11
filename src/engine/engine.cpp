#include "engine.h"
#include <assert.h>
#include <optional>
#include <exception>

namespace game_of_life {
  
void Board::load(std::istream& is, CellEncoding cell_encoding /* = {} */) {
  assert(cell_encoding.is_valid());
  clear();
  std::optional<size_t> row_length;
  size_t row_cell_count = 0;
  char c;
  while (is.get(c)) {
    if (row_cell_count == 0) {
      _cells.push_back({});
      _alive_cells_count_by_row.push_back(0);
    }
    if (c == cell_encoding.row_separator) {
      if (row_length.has_value() && row_length != row_cell_count) {
        throw std::runtime_error(
          "game_of_life::Board::load: row " + std::to_string(_cells.size()) + " has length different from previous one"
        );
      }
      row_length = row_cell_count;
      row_cell_count = 0;
    } else if (c == cell_encoding.alive_cell) {
      _cells.back().push_back(CellState::ALIVE);
      _alive_cells_count_by_row.back()++;
      if (_alive_cells_count_by_col.size() <= row_cell_count) _alive_cells_count_by_col.push_back(0);
      _alive_cells_count_by_col[row_cell_count]++;
      row_cell_count++;
    } else if (c == cell_encoding.dead_cell) {
      _cells.back().push_back(CellState::DEAD);
      row_cell_count++;
    } else {
      throw std::runtime_error("game_of_life::Board::load: unsupported character: " + c);
    }
  }
  if (row_cell_count != 0 && row_length.has_value() && row_length != row_cell_count) {
    // check the case when last line is missing line separator and has different number of characters from other lines
    throw std::runtime_error("game_of_life::Board::load: row " + std::to_string(_cells.size()) + " has length different from previous one");
  }
  if (length() == 0) clear(); // remove 0-length rows
}


void Board::save(std::ostream& os, CellEncoding cell_encoding /* = {} */) const {
  assert(cell_encoding.is_valid());
  for (const auto& row: _cells) {
    for (auto cell_state: row) {
      if (cell_state == CellState::ALIVE) {
        os.write(&cell_encoding.alive_cell, 1);
      } else if (cell_state == CellState::DEAD) {
        os.write(&cell_encoding.dead_cell, 1);
      }
    }
    os.write(&cell_encoding.row_separator, 1);
  }
}


void Board::clear() {
  _cells.clear();
  _alive_cells_count_by_col.clear();
  _alive_cells_count_by_row.clear();
}


void Board::replaceCell(const CellDescription& cell_description) {
  auto& cell = _cells[cell_description.y][cell_description.x];
  if (cell == cell_description.state) return;
  cell = cell_description.state;
  if (cell == CellState::ALIVE) {
    _alive_cells_count_by_col[cell_description.x]++;
    _alive_cells_count_by_row[cell_description.y]++;
  } else if (cell == CellState::DEAD) {
    _alive_cells_count_by_col[cell_description.x]--;
    _alive_cells_count_by_row[cell_description.y]--;
  }
}


void Board::shrink() {
  size_t i = 0;
  for (i = 0; i < _alive_cells_count_by_row.size(); i++) {
    if (_alive_cells_count_by_row[i] != 0) break;;
  }
  _alive_cells_count_by_row.erase(_alive_cells_count_by_row.begin(), _alive_cells_count_by_row.begin() + i);
  _cells.erase(_cells.begin(), _cells.begin() + i);

  for (i = _alive_cells_count_by_row.size(); i != 0; i--) {
    if (_alive_cells_count_by_row[i - 1] != 0) break;
  }
  _alive_cells_count_by_row.erase(_alive_cells_count_by_row.begin() + i, _alive_cells_count_by_row.end());
  _cells.erase(_cells.begin() + i, _cells.end());

  for (i = 0; i < _alive_cells_count_by_col.size(); i++) {
    if (_alive_cells_count_by_col[i] != 0) break;
  }
  _alive_cells_count_by_col.erase(_alive_cells_count_by_col.begin(), _alive_cells_count_by_col.begin() + i);
  for (auto& row: _cells) {
    row.erase(row.begin(), row.begin() + i);
  }

  for (i = _alive_cells_count_by_col.size(); i != 0; i--) {
    if (_alive_cells_count_by_col[i - 1] != 0) break;
  }
  _alive_cells_count_by_col.erase(_alive_cells_count_by_col.begin() + i, _alive_cells_count_by_col.end());
  for (auto& row: _cells) {
    row.erase(row.begin() + i, row.end());
  }

  if (length() == 0) clear();
}


void Board::extendUp() {
  if (length() == 0) return;
  _cells.push_front(std::deque<CellState>(length(), CellState::DEAD));
  _alive_cells_count_by_row.push_front(0);
}


void Board::extendDown() {
  if (length() == 0) return;
  _cells.push_back(std::deque<CellState>(length(), CellState::DEAD));
  _alive_cells_count_by_row.push_back(0);
}


void Board::extendLeft() {
  if (height() == 0) return;
  _alive_cells_count_by_col.push_front(0);
  for (auto& row: _cells) {
    row.push_front(CellState::DEAD);
  }
}


void Board::extendRight() {
  if (height() == 0) return;
  _alive_cells_count_by_col.push_back(0);
  for (auto& row: _cells) {
    row.push_back(CellState::DEAD);
  }
}


size_t Board::getNeighborsCount(size_t y, size_t x, CellState cell_state /* = CellState::ALIVE */) const {
  assert(y < height() && x < length());
  size_t y_min = y == 0 ? 0 : y - 1;
  size_t y_max = y + 1 == height() ? y : y + 1;
  size_t x_min = x == 0 ? 0 : x - 1;
  size_t x_max = x + 1 == length() ? x : x + 1;
  size_t count = 0;
  for (size_t i = y_min; i <= y_max; i++) {
    for (size_t j = x_min; j <= x_max; j++) {
      if (i == y && j == x) continue;
      if (_cells[i][j] == cell_state) count++;
    }
  }
  return count;
}


Engine::Engine(Board board, const Rules& rules /* = {} */) 
  : _board(std::move(board)), _rules(rules) {
  assert(_rules.isValid());
}


void Engine::next() {
  _board.extendDown();
  _board.extendUp();
  _board.extendLeft();
  _board.extendRight();
  _cells_to_modify_buffer.clear();
  for (size_t i = 0; i < _board.height(); i++) {
    for (size_t j = 0; j < _board.length(); j++) {
      size_t neighbors_count = _board.getNeighborsCount(i, j);
      if (_board.getCellState(i, j) == CellState::ALIVE && _rules.cellShouldDie(neighbors_count)) {
        _cells_to_modify_buffer.push_back({i, j, CellState::DEAD});
      } else if (_board.getCellState(i, j) == CellState::DEAD && _rules.cellShouldSpawn(neighbors_count)) {
        _cells_to_modify_buffer.push_back({i, j, CellState::ALIVE});
      }
    }
  }
  _board.replaceCells(_cells_to_modify_buffer.begin(), _cells_to_modify_buffer.end());
  _board.shrink();
}


}