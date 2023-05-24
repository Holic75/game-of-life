
#include <string>
#include <gtest/gtest.h>
#include "../src/core/engine.h"
#include <sstream>

using namespace game_of_life;

const std::string BOARD_EMPTY = "";
const std::string BOARD_EMPTY_ROW = "\n";
const std::string BOARD_EMPTY_ROWS = "\n\n\n";
const std::string BOARD_DEAD = "___\n___\n___\n";
const std::string BOARD_ALIVE = "***_\n_*__\n____\n";
const std::string BOARD_NO_LAST_SEPATOR = "*\n_\n_";
const std::string BOARD_ROWS_DIFFERENT_SIZE = "***\n**\n*\n";
const std::string BOARD_ROWS_BAD_CHAR = "***\n**X\n*XX\n";
const std::string BOARD_UNTERMINATED = "***\n**";

CellEncoding CELL_ENCODING;
auto ENCODE = [](CellState cell) { return CELL_ENCODING.encode(cell); };
auto DECODE = [](char c) { return CELL_ENCODING.decode(c); };


std::stringstream getStream(const std::string& s) {
  std::stringstream ss(s, std::stringstream::in | std::stringstream::out | std::stringstream::binary);
  return ss;
};


std::string convertGameBoardToString(const GameBoard& board) {
  std::stringstream ss;
  board.save(ss, board.getOccupiedCellsBoundingRectangle(), ENCODE);
  return ss.str();
}


TEST(GameBoard, create) {
  GameBoard board;
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  GameBoard board2(2, 3);
  ASSERT_EQ(board2.length(), 2);
  ASSERT_EQ(board2.height(), 3);
  ASSERT_EQ(board2.getCell(0, 0), CellState::DEAD);
  ASSERT_EQ(board2.getCell(1, 0), CellState::DEAD);
  ASSERT_EQ(board2.getCell(0, 1), CellState::DEAD);
  ASSERT_EQ(board2.getCell(1, 1), CellState::DEAD);
  ASSERT_EQ(board2.getCell(0, 2), CellState::DEAD);
  ASSERT_EQ(board2.getCell(1, 2), CellState::DEAD);
}


TEST(GameBoard, load) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_EMPTY);
  board.load(ss, DECODE);
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  ss = getStream(BOARD_EMPTY_ROW);
  board.load(ss, DECODE);
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  ss = getStream(BOARD_EMPTY_ROWS);
  board.load(ss, DECODE);
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  ss = getStream(BOARD_DEAD);
  board.load(ss, DECODE);
  ASSERT_EQ(board.length(), 3);
  ASSERT_EQ(board.height(), 3);
  ASSERT_EQ(board.getCell(0, 0), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 0), CellState::DEAD);
  ASSERT_EQ(board.getCell(2, 0), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(2, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(2, 2), CellState::DEAD);

  ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);
  ASSERT_EQ(board.length(), 4);
  ASSERT_EQ(board.height(), 3);
  ASSERT_EQ(board.getCell(0, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCell(1, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCell(2, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCell(3, 0), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 1), CellState::ALIVE);
  ASSERT_EQ(board.getCell(2, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(3, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(2, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(3, 2), CellState::DEAD);

  ss = getStream(BOARD_NO_LAST_SEPATOR);
  board.load(ss, DECODE);
  ASSERT_EQ(board.length(), 1);
  ASSERT_EQ(board.height(), 3);
  ASSERT_EQ(board.getCell(0, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCell(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 2), CellState::DEAD);
}


TEST(GameBoard, load_throw_exception) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_ROWS_DIFFERENT_SIZE);
  ASSERT_THROW(board.load(ss, DECODE), std::runtime_error);
  ss = getStream(BOARD_ROWS_BAD_CHAR);
  ASSERT_THROW(board.load(ss, DECODE), std::runtime_error);
  ss = getStream(BOARD_UNTERMINATED);
  ASSERT_THROW(board.load(ss, DECODE), std::runtime_error);
}


TEST(GameBoard, setCell) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);

  board.setCell(0, 0, CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 0), CellState::DEAD);

  board.setCell(3, 2, CellState::ALIVE);
  ASSERT_EQ(board.getCell(3, 2), CellState::ALIVE);
}


TEST(GameBoard, getOccupiedCellsBoundingRectangle) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_EMPTY);
  board.load(ss, DECODE);
  auto rect = board.getOccupiedCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 0);
  ASSERT_EQ(rect.top, 0);
  ASSERT_EQ(rect.bottom, 0);

  ss = getStream(BOARD_DEAD);
  board.load(ss, DECODE);
  rect = board.getOccupiedCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 0);
  ASSERT_EQ(rect.top, 0);
  ASSERT_EQ(rect.bottom, 0);

  // check that setting cells affects the bounding rectangle
  board.setCell(0, 1, CellState::ALIVE);
  board.setCell(1, 2, CellState::ALIVE);
  rect = board.getOccupiedCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 2);
  ASSERT_EQ(rect.top, 1);
  ASSERT_EQ(rect.bottom, 3);

  ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);
  rect = board.getOccupiedCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 3);
  ASSERT_EQ(rect.top, 0);
  ASSERT_EQ(rect.bottom, 2);
}


TEST(GameBoard, save) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_EMPTY);
  board.load(ss, DECODE);

  std::stringstream out_empty;
  board.save(out_empty, board.getOccupiedCellsBoundingRectangle(), ENCODE);
  ASSERT_EQ(out_empty.str(), BOARD_EMPTY);

  ss = getStream(BOARD_DEAD);
  board.load(ss, DECODE);
  std::stringstream out_dead;
  board.save(out_dead, {0, 0, 3, 3}, ENCODE);

  ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);
  std::stringstream out_alive;
  board.save(out_alive, {1, 0, 3, 2}, ENCODE);
  ASSERT_EQ(out_alive.str(), "**\n*_\n");
}


TEST(GameBoard, reset) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);

  board.reset();
  ASSERT_EQ(board.height(), 0);
  ASSERT_EQ(board.length(), 0);
  board.reset(2, 3);
  ASSERT_EQ(board.getCell(0, 0), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 0), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 1), CellState::DEAD);
  ASSERT_EQ(board.getCell(0, 2), CellState::DEAD);
  ASSERT_EQ(board.getCell(1, 2), CellState::DEAD);
}


TEST(GameBoard, getNeighborsCount) {
  GameBoard board;
  std::stringstream ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);

  ASSERT_EQ(board.getNeighborsCount(0, 0, CellState::ALIVE), 2);
  ASSERT_EQ(board.getNeighborsCount(1, 0, CellState::ALIVE), 3);
  ASSERT_EQ(board.getNeighborsCount(2, 0, CellState::ALIVE), 2);
  ASSERT_EQ(board.getNeighborsCount(3, 0, CellState::ALIVE), 1);
  ASSERT_EQ(board.getNeighborsCount(0, 1, CellState::ALIVE), 3);
  ASSERT_EQ(board.getNeighborsCount(1, 1, CellState::ALIVE), 3);
  ASSERT_EQ(board.getNeighborsCount(2, 1, CellState::ALIVE), 3);
  ASSERT_EQ(board.getNeighborsCount(3, 1, CellState::ALIVE), 1);
  ASSERT_EQ(board.getNeighborsCount(0, 2, CellState::ALIVE), 1);
  ASSERT_EQ(board.getNeighborsCount(1, 2, CellState::ALIVE), 1);
  ASSERT_EQ(board.getNeighborsCount(2, 2, CellState::ALIVE), 1);
  ASSERT_EQ(board.getNeighborsCount(3, 2, CellState::ALIVE), 0);

  ASSERT_EQ(board.getNeighborsCount(1, 1, CellState::DEAD), 5);

  // cells outside of the board
  ASSERT_EQ(board.getNeighborsCount(-1, -1, CellState::ALIVE), 1);
  ASSERT_EQ(board.getNeighborsCount(10, 10, CellState::ALIVE), 0);

  ASSERT_EQ(board.getNeighborsCount(-1, -1, CellState::DEAD), 7);
  ASSERT_EQ(board.getNeighborsCount(10, 10, CellState::DEAD), 8);

}

TEST(Rules, ConstructorThrow) {
  ASSERT_THROW(GameRules(2, 1), std::runtime_error);
  ASSERT_THROW(GameRules(2, 2, 3, 2), std::runtime_error);
}

TEST(Rules, cellShouldDie) {
  GameRules rules;
  ASSERT_EQ(rules.cellShouldDie(0), true);
  ASSERT_EQ(rules.cellShouldDie(1), true);
  ASSERT_EQ(rules.cellShouldDie(2), false);
  ASSERT_EQ(rules.cellShouldDie(3), false);
  ASSERT_EQ(rules.cellShouldDie(4), true);
  ASSERT_EQ(rules.cellShouldDie(5), true);
  ASSERT_EQ(rules.cellShouldDie(6), true);
  ASSERT_EQ(rules.cellShouldDie(7), true);
  ASSERT_EQ(rules.cellShouldDie(8), true);
}


TEST(Rules, cellShouldSpawn) {
  GameRules rules;
  ASSERT_EQ(rules.cellShouldSpawn(0), false);
  ASSERT_EQ(rules.cellShouldSpawn(1), false);
  ASSERT_EQ(rules.cellShouldSpawn(2), false);
  ASSERT_EQ(rules.cellShouldSpawn(3), true);
  ASSERT_EQ(rules.cellShouldSpawn(4), false);
  ASSERT_EQ(rules.cellShouldSpawn(5), false);
  ASSERT_EQ(rules.cellShouldSpawn(6), false);
  ASSERT_EQ(rules.cellShouldSpawn(7), false);
  ASSERT_EQ(rules.cellShouldSpawn(8), false);
}


TEST(Engine, next) {
  // applying next to empty board does not change it
  Engine e1 = Engine(GameBoard(), GameRules());
  e1.next();
  ASSERT_EQ(convertGameBoardToString(e1.board()), "");

  GameBoard board;
  std::stringstream ss = getStream(BOARD_DEAD);
  board.load(ss, DECODE);
  Engine e2 = Engine(std::move(board), GameRules());
  e2.next();
  ASSERT_EQ(convertGameBoardToString(e2.board()), "");

  // test non empty board
  ss = getStream(BOARD_ALIVE);
  board.load(ss, DECODE);
  Engine e3 = Engine(std::move(board), GameRules());
  e3.next();
  ASSERT_EQ(convertGameBoardToString(e3.board()), "_*_\n***\n***\n");

  e3.next();
  ASSERT_EQ(convertGameBoardToString(e3.board()), "***\n___\n*_*\n_*_\n");

  e3.next();
  ASSERT_EQ(convertGameBoardToString(e3.board()), "_*_\n_*_\n*_*\n_*_\n_*_\n");

  e3.next();
  ASSERT_EQ(convertGameBoardToString(e3.board()), "***\n*_*\n***\n");

  e3.next();const std::string BOARD_ALIVE = "***_\n_*__\n____\n";
  ASSERT_EQ(convertGameBoardToString(e3.board()), "__*__\n_*_*_\n*___*\n_*_*_\n__*__\n");
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

