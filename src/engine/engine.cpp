#include "engine.h"
#include <assert.h>
#include <optional>
#include <exception>
#include <algorithm>

namespace game_of_life {

Board::Board(size_t length, size_t height) {
  reset(length, height);
}


void Board::reset(size_t length /* = 0 */, size_t height /* = 0 */) {
  assert(length < std::numeric_limits<int>::max() && height < std::numeric_limits<int>::max());
  _cells.resize(height * length);
  std::fill(_cells.begin(), _cells.end(), CellState::DEAD);
  _alive_cells_count_by_row.resize(height);
  std::fill(_alive_cells_count_by_row.begin(), _alive_cells_count_by_row.end(), 0);
  _alive_cells_count_by_col.resize(length);
  std::fill(_alive_cells_count_by_col.begin(), _alive_cells_count_by_col.end(), 0);
}
  
void Board::load(std::istream& is, CellEncoding cell_encoding /* = {} */) {
  assert(cell_encoding.is_valid());
  reset();
  std::optional<size_t> row_length;
  size_t row_cell_count = 0;
  char c;
  while (is.get(c)) {
    if (row_cell_count == 0) {
      _alive_cells_count_by_row.push_back(0);
    }
    if (c == cell_encoding.row_separator) {
      if (row_length.has_value() && row_length != row_cell_count) {
        throw std::runtime_error(
          "game_of_life::Board::load: row " + std::to_string(height()) + " has length different from previous one"
        );
      }
      row_length = row_cell_count;
      row_cell_count = 0;
    } else if (c == cell_encoding.alive_cell) {
      _cells.push_back(CellState::ALIVE);
      _alive_cells_count_by_row.back()++;
      if (_alive_cells_count_by_col.size() <= row_cell_count) _alive_cells_count_by_col.push_back(0);
      _alive_cells_count_by_col[row_cell_count]++;
      row_cell_count++;
    } else if (c == cell_encoding.dead_cell) {
      if (_alive_cells_count_by_col.size() <= row_cell_count) _alive_cells_count_by_col.push_back(0);
      _cells.push_back(CellState::DEAD);
      row_cell_count++;
    } else {
      throw std::runtime_error("game_of_life::Board::load: unsupported character: " + c);
    }
  }
  if (row_cell_count != 0 && row_length.has_value() && row_length != row_cell_count) {
    // check the case when last line is missing line separator and has different number of characters from other lines
    throw std::runtime_error("game_of_life::Board::load: row " + std::to_string(height()) + " has length different from previous one");
  }
  if (length() == 0) reset(); // remove 0-length rows
}


void Board::save(std::ostream& os,  const Rectangle& bounding_rect, CellEncoding cell_encoding /* = {} */) const {
  assert(cell_encoding.is_valid());
  for (size_t y = bounding_rect.top; y < bounding_rect.bottom; y++) {
    for (size_t x = bounding_rect.left; x < bounding_rect.right; x++) {
      CellState state = getCellState(x, y);
      if (state == CellState::ALIVE) {
        os.write(&cell_encoding.alive_cell, 1);
      } else if (state == CellState::DEAD) {
        os.write(&cell_encoding.dead_cell, 1);
      }
    }
    os.write(&cell_encoding.row_separator, 1);
  }
}


void Board::setCellState(size_t x, size_t y, CellState state) {
  auto& cell = _cells[x + y * length()];
  if (cell == state) return;
  cell = state;
  if (cell == CellState::ALIVE) {
    _alive_cells_count_by_col[x]++;
    _alive_cells_count_by_row[y]++;
  } else if (cell == CellState::DEAD) {
    _alive_cells_count_by_col[x]--;
    _alive_cells_count_by_row[y]--;
  }
}


CellState Board::getCellState(int x, int y) const {
  if (x >= 0 && x < static_cast<int>(length()) && y >= 0 && y < static_cast<int>(height())) {
    return _cells[x + y * length()];
  }
  return CellState::DEAD;
}



size_t Board::getAliveNeighborsCount(int x, int y) const {
  int y_begin = std::max(0, y - 1);
  int y_end = std::min(static_cast<int>(height()), y + 2);
  int x_begin = std::max(0, x - 1);
  int x_end = std::min(static_cast<int>(length()), x + 2);
  size_t count = 0;
  for (int i = y_begin; i < y_end; i++) {
    for (int j = x_begin; j < x_end; j++) {
      if (i == y && j == x) continue;
      if (getCellState(j, i) == CellState::ALIVE) count++;
    }
  }
  return count;
}


Rectangle Board::getAliveCellsBoundingRectangle() const {
  Rectangle rect = {0, 0, 0, 0};

  for (size_t i = 0; i < length(); i++) {
    if (_alive_cells_count_by_col[i] > 0) {
      rect.left = i;
      break;
    }
  }
  for (size_t i = length(); i > 0; i--) {
    if (_alive_cells_count_by_col[i - 1] > 0) {
      rect.right = i;
      break;
    }
  }
  for (size_t i = 0; i < height(); i++) {
    if (_alive_cells_count_by_row[i] > 0) {
      rect.top = i;
      break;
    }
  }
  for (size_t i = height(); i > 0; i--) {
    if (_alive_cells_count_by_row[i - 1] > 0) {
      rect.bottom = i;
      break;
    }
  }

  return rect;
}


Engine::Engine(Board board, const Rules& rules /* = {} */) 
  :_rules(rules), _current_board_idx(0) {
  assert(_rules.isValid());
  _boards[_current_board_idx] = std::move(board);
}


void Engine::next() {
  auto& current_board = _boards[_current_board_idx];
  auto living_cells_bounding_rect = current_board.getAliveCellsBoundingRectangle();
  if (living_cells_bounding_rect.length() == 0 || living_cells_bounding_rect.height() == 0) return;

  size_t next_board_idx = _current_board_idx == 0 ? 1 : 0;
  auto& next_board = _boards[next_board_idx];

  next_board.reset(living_cells_bounding_rect.length() + 2, living_cells_bounding_rect.height() + 2);
  for (size_t new_y = 0; new_y < next_board.height() + 1; new_y++) {
    int y = new_y + static_cast<int>(living_cells_bounding_rect.top) - 1;
    for (size_t new_x = 0; new_x < next_board.length(); new_x++) {
      int x = new_x + static_cast<int>(living_cells_bounding_rect.left) - 1;
      size_t alive_neighbors_count = current_board.getAliveNeighborsCount(x, y);
      bool should_create_living_cell 
        = current_board.getCellState(x, y) == CellState::ALIVE
          ? !_rules.cellShouldDie(alive_neighbors_count) 
          : _rules.cellShouldSpawn(alive_neighbors_count);
      if (should_create_living_cell) next_board.setCellState(new_x, new_y, CellState::ALIVE);
    }
  }
  _current_board_idx = next_board_idx;
}


}