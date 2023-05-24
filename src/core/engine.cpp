#include <exception>
#include <assert.h>
#include "engine.h"

namespace game_of_life {

CellEncoding::CellEncoding(char alive_cell /*= '*'*/, char dead_cell /*= '_'*/) 
  :_alive_cell(alive_cell), _dead_cell(dead_cell) {
  if (alive_cell == dead_cell) {
    throw std::runtime_error("CellEncoding::CellEncoding: alive and dead cells are represented by the same character: " + alive_cell);
  }
}

char CellEncoding::encode(CellState cell) const {
  if (cell == CellState::ALIVE) return _alive_cell;
  return _dead_cell;
}


CellState CellEncoding::decode(char encoded_cell) const {
  if (encoded_cell == _alive_cell) return CellState::ALIVE;
  if (encoded_cell == _dead_cell) return CellState::DEAD;
  throw std::runtime_error("CellState::decode: unsupported character: " + encoded_cell);
}




Engine::Engine(GameBoard board, GameRules rules)   
  :_rules(std::move(rules)), _current_board_idx(0) {
  _boards[_current_board_idx] = std::move(board);
}


GameRules::GameRules(
  size_t min_neighbors_to_survive /*= 2*/, size_t max_neighbors_to_survive /*= 3*/, 
  size_t min_neighbors_to_spawn /*= 3*/, size_t max_neighbors_to_spawn /*= 3*/
) 
  :_min_neighbors_to_survive(min_neighbors_to_survive), _max_neighbors_to_survive(max_neighbors_to_survive),
  _min_neighbors_to_spawn(min_neighbors_to_spawn), _max_neighbors_to_spawn(max_neighbors_to_spawn)
{
  if (_max_neighbors_to_survive < _min_neighbors_to_survive) {
    throw std::runtime_error("GameRules::GameRules: rules are contradictory: max_neighbors_to_survive < min_neighbors_to_survive");
  }
  if (_max_neighbors_to_spawn < _min_neighbors_to_spawn) {
    throw std::runtime_error("GameRules::GameRules: rules are contradictory: max_neighbors_to_spawn < min_neighbors_to_spawn");
  }
}


void Engine::next() {
  auto& current_board = _boards[_current_board_idx];
  auto living_cells_bounding_rect = current_board.getOccupiedCellsBoundingRectangle();
  if (living_cells_bounding_rect.length() == 0 || living_cells_bounding_rect.height() == 0) return;

  size_t next_board_idx = _current_board_idx == 0 ? 1 : 0;
  auto& next_board = _boards[next_board_idx];

  next_board.reset(living_cells_bounding_rect.length() + 2, living_cells_bounding_rect.height() + 2);
  for (size_t new_y = 0; new_y < next_board.height() + 1; new_y++) {
    int y = new_y + static_cast<int>(living_cells_bounding_rect.top) - 1;
    for (size_t new_x = 0; new_x < next_board.length(); new_x++) {
      int x = new_x + static_cast<int>(living_cells_bounding_rect.left) - 1;
      size_t alive_neighbors_count = current_board.getNeighborsCount(x, y, CellState::ALIVE);
      bool should_create_living_cell 
        = current_board.getCell(x, y) == CellState::ALIVE
          ? !_rules.cellShouldDie(alive_neighbors_count) 
          : _rules.cellShouldSpawn(alive_neighbors_count);
      if (should_create_living_cell) next_board.setCell(new_x, new_y, CellState::ALIVE);
    }
  }
  _current_board_idx = next_board_idx;
}


}