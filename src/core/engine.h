#pragma once

#include <array>
#include "board.h"

namespace game_of_life {

enum class CellState {
  DEAD = 0,
  ALIVE = 1
};

typedef Board<CellState> GameBoard;

/// @brief Class providing encoding and decoding cell functionality.
class CellEncoding {
  char _alive_cell;
  char _dead_cell;
public:
  /// @brief  Construct by specifying characters to represent alive and dead cells.
  CellEncoding(char alive_cell = '*', char dead_cell = '_');
  /// @brief Encode cell state as a character
  char encode(CellState cell) const;
  /// @brief Decode character into a cell state.
  /// Throws std::runtime_error if the character can not be decoded.
  CellState decode(char encoded_cell) const;
};


/// @brief Class summarizing rules of the game of life.
class GameRules {
  size_t _min_neighbors_to_survive = 2;
  size_t _max_neighbors_to_survive = 3;
  size_t _min_neighbors_to_spawn = 3;
  size_t _max_neighbors_to_spawn = 3;
public:
  /// @brief Construct from inclusive bounds on number of neighboring living cells to survive and on number of living cells to spawn a new one.
  /// Throws std::runtime_error if the rules are contradictory.
  GameRules(
    size_t min_neighbors_to_survive = 2, size_t max_neighbors_to_survive = 3, 
    size_t min_neighbors_to_spawn = 3, size_t max_neighbors_to_spawn = 3
  );
  /// @brief Check if the living cell should die, based on the number of neighbors.
  /// @return True if the cell should die, false otherwise.
  bool cellShouldDie(size_t neighbors_count) const { 
    return neighbors_count < _min_neighbors_to_survive || neighbors_count > _max_neighbors_to_survive;
  }
  /// @brief Check if the living cell should spawn, based on the number of neighbors.
  /// @return True if the cell should spawn, false otherwise.
  bool cellShouldSpawn(size_t neighbors_count) const { 
    return neighbors_count >= _min_neighbors_to_spawn && neighbors_count <= _max_neighbors_to_spawn;
  }
};

/// @brief Class running iterations of the game of life.
class Engine {
private:
  std::array<GameBoard, 2> _boards;
  GameRules _rules;
  size_t _current_board_idx;
public:
  /// @brief Construct from board describing initial state and rules.
  Engine(GameBoard board, GameRules rules);
  /// @brief Return the board corresponding to the current state of the game.
  // The board size is undefined, but is guaranteed to fit all living cells
  const GameBoard& board() { return _boards[_current_board_idx]; };
  /// @brief Transition to the next state of the game.
  void next();
};

}