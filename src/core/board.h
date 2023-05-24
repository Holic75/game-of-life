#pragma once

#include <vector>
#include <ostream>
#include <istream>
#include <optional>

namespace game_of_life {

struct Rectangle {
  size_t left;
  size_t top;
  size_t right;
  size_t bottom;
  size_t height() const { return bottom > top ? bottom - top : 0; }
  size_t length() const { return right > left ? right - left : 0; }
};

/// @brief Class representing current state of the game of life.
/// @tparam CellT Class representing a cell. Should be default-constructible (which represents an empty cell), copy-constructible and equality-comparable.
template<class CellT>
class Board {
private:
  std::vector<CellT> _cells;
  std::vector<size_t> _occupied_cells_count_by_row;
  std::vector<size_t> _occupied_cells_count_by_col;
  static const CellT _EMPTY_CELL;
public:
  /// @brief Construct zero-size board
  Board() {}

  /// @brief Construct the board with the specified size. All the board cells will initially be set to default-constructed
  Board(size_t length, size_t height) { reset(length, height); }

  /// @brief Reset the size to height x length. All the cells will be set to default-constructed
  void reset(size_t length = 0, size_t height = 0) {
    assert(length < std::numeric_limits<int>::max() && height < std::numeric_limits<int>::max());
    _cells.clear();
    _occupied_cells_count_by_row.clear();
    _occupied_cells_count_by_col.clear();
    _cells.resize(height * length);
    _occupied_cells_count_by_row.resize(height, 0);
    _occupied_cells_count_by_col.resize(length, 0);
  }

  /// @brief Get cell at row y and column x.
  const CellT& getCell(int x, int y) const {
    if (x >= 0 && x < static_cast<int>(length()) && y >= 0 && y < static_cast<int>(height())) {
      return _cells[x + y * length()];
    }
    return _EMPTY_CELL;
  }

  /// @brief Get number of rows on the board
  size_t height() const { return _occupied_cells_count_by_row.size(); }

  /// @brief Get number of columns on the board
  size_t length() const { return _occupied_cells_count_by_col.size(); }

  /// @brief Read board from specified stream
  /// @tparam CellDecoder callable with signature "CellT CellDecoder(char)"", decoding char into a cell
  template<class CellDecoder>
  void load(std::istream& is, CellDecoder cell_decoder = CellDecoder(), char row_separator = '\n') {
    reset();
    std::optional<size_t> row_length;
    size_t row_cell_count = 0;
    char c;
    while (is.get(c)) {
      if (row_cell_count == 0) {
        _occupied_cells_count_by_row.push_back(0);
      }
      if (c == row_separator) {
        if (row_length.has_value() && row_length != row_cell_count) {
          throw std::runtime_error(
            "game_of_life::Board::load: row " + std::to_string(height()) + " has length different from previous one"
          );
        }
        row_length = row_cell_count;
        row_cell_count = 0;
      } else {
        _cells.push_back(cell_decoder(c));
        if (_occupied_cells_count_by_col.size() <= row_cell_count) _occupied_cells_count_by_col.push_back(0);
        if (_cells.back() != _EMPTY_CELL) {
          _occupied_cells_count_by_row.back()++;
          _occupied_cells_count_by_col[row_cell_count]++;
        }
        row_cell_count++;
      }
    }
    if (row_cell_count != 0 && row_length.has_value() && row_length != row_cell_count) {
      // check the case when last line is missing line separator and has different number of characters from other lines
      throw std::runtime_error("game_of_life::Board::load: row " + std::to_string(height()) + " has length different from previous one");
    }
    if (length() == 0) reset(); // remove 0-length rows
  }

  /// @brief Write board area delimited by bounding_rect to specified stream. No boundary checks are performed
  /// @tparam CellEncoder callable with signature "char CellEncoder(CellT)"", encoding cell into a char
  template<class CellEncoder>
  void save(std::ostream& os, const Rectangle& bounding_rect, CellEncoder cell_encoder, char row_separator = '\n') const {
    for (size_t y = bounding_rect.top; y < bounding_rect.bottom; y++) {
      for (size_t x = bounding_rect.left; x < bounding_rect.right; x++) {
        char c = cell_encoder(getCell(x, y));
        os.write(&c, 1);
      }
      os.write(&row_separator, 1);
    }
  }

  /// @brief Replace cell at position specified by newCell with the state specified by cell_description. 
  /// No boundary checks are performed on cell coordinates.
  void setCell(size_t x, size_t y, CellT newCell) {
    assert(x < length() && y < height());
    auto& cell = _cells[x + y * length()];
    if ((cell == _EMPTY_CELL) && (newCell != _EMPTY_CELL)) {
      _occupied_cells_count_by_col[x]++;
      _occupied_cells_count_by_row[y]++;
    } else if ((cell != _EMPTY_CELL) && (newCell == _EMPTY_CELL)) {
      _occupied_cells_count_by_col[x]--;
      _occupied_cells_count_by_row[y]--;
    }
    cell = newCell;
  }

  /// @brief Get number of neighbors that are equal to the specified cell.
  /// Note: coodrinates can be outside of the board bounds. All the cells outside of the board bounds
  /// are assumed to be default-constructed
  size_t getNeighborsCount(int x, int y, const CellT& cell) const {
    size_t count = 0;
    for (int i = y - 1; i < y + 2; i++) {
      for (int j = x - 1; j < x + 2; j++) {
        if (i == y && j == x) continue;
        if (getCell(j, i) == cell) count++;
      }
    }
    return count;
  }

  /// @brief Get rectangle coordinates, delimiting minimal board area necessary to fit all non-default constructed cells.
  Rectangle getOccupiedCellsBoundingRectangle() const {
    Rectangle rect = {0, 0, 0, 0};
    for (size_t i = 0; i < length(); i++) {
      if (_occupied_cells_count_by_col[i] > 0) {
        rect.left = i;
        break;
      }
    }
    for (size_t i = length(); i > 0; i--) {
      if (_occupied_cells_count_by_col[i - 1] > 0) {
        rect.right = i;
        break;
      }
    }
    for (size_t i = 0; i < height(); i++) {
      if (_occupied_cells_count_by_row[i] > 0) {
        rect.top = i;
        break;
      }
    }
    for (size_t i = height(); i > 0; i--) {
      if (_occupied_cells_count_by_row[i - 1] > 0) {
        rect.bottom = i;
        break;
      }
    }
    return rect;
  }
};

template <class CellT>
const CellT Board<CellT>::_EMPTY_CELL = CellT();
}