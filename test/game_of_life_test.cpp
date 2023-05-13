
#include <string>
#include <gtest/gtest.h>
#include "../src/engine/engine.h"
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


std::stringstream getStream(const std::string& s) {
  std::stringstream ss(s, std::stringstream::in | std::stringstream::out | std::stringstream::binary);
  return ss;
};


std::string convertBoardToString(const Board& board) {
  std::stringstream ss;
  board.save(ss, board.getAliveCellsBoundingRectangle());
  return ss.str();
}


TEST(Board, create) {
  Board board;
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  Board board2(2, 3);
  ASSERT_EQ(board2.length(), 2);
  ASSERT_EQ(board2.height(), 3);
  ASSERT_EQ(board2.getCellState(0, 0), CellState::DEAD);
  ASSERT_EQ(board2.getCellState(1, 0), CellState::DEAD);
  ASSERT_EQ(board2.getCellState(0, 1), CellState::DEAD);
  ASSERT_EQ(board2.getCellState(1, 1), CellState::DEAD);
  ASSERT_EQ(board2.getCellState(0, 2), CellState::DEAD);
  ASSERT_EQ(board2.getCellState(1, 2), CellState::DEAD);
}


TEST(Board, load) {
  Board board;
  std::stringstream ss = getStream(BOARD_EMPTY);
  board.load(ss);
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  ss = getStream(BOARD_EMPTY_ROW);
  board.load(ss);
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  ss = getStream(BOARD_EMPTY_ROWS);
  board.load(ss);
  ASSERT_EQ(board.length(), 0);
  ASSERT_EQ(board.height(), 0);

  ss = getStream(BOARD_DEAD);
  board.load(ss);
  ASSERT_EQ(board.length(), 3);
  ASSERT_EQ(board.height(), 3);
  ASSERT_EQ(board.getCellState(0, 0), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 0), CellState::DEAD);
  ASSERT_EQ(board.getCellState(2, 0), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(2, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(2, 2), CellState::DEAD);

  ss = getStream(BOARD_ALIVE);
  board.load(ss);
  ASSERT_EQ(board.length(), 4);
  ASSERT_EQ(board.height(), 3);
  ASSERT_EQ(board.getCellState(0, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCellState(1, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCellState(2, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCellState(3, 0), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 1), CellState::ALIVE);
  ASSERT_EQ(board.getCellState(2, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(3, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(2, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(3, 2), CellState::DEAD);

  ss = getStream(BOARD_NO_LAST_SEPATOR);
  board.load(ss);
  ASSERT_EQ(board.length(), 1);
  ASSERT_EQ(board.height(), 3);
  ASSERT_EQ(board.getCellState(0, 0), CellState::ALIVE);
  ASSERT_EQ(board.getCellState(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 2), CellState::DEAD);
}


TEST(Board, load_throw_exception) {
  Board board;
  std::stringstream ss = getStream(BOARD_ROWS_DIFFERENT_SIZE);
  ASSERT_THROW(board.load(ss), std::runtime_error);
  ss = getStream(BOARD_ROWS_BAD_CHAR);
  ASSERT_THROW(board.load(ss), std::runtime_error);
  ss = getStream(BOARD_UNTERMINATED);
  ASSERT_THROW(board.load(ss), std::runtime_error);
}


TEST(Board, setCellState) {
  Board board;
  std::stringstream ss = getStream(BOARD_ALIVE);
  board.load(ss);

  board.setCellState(0, 0, CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 0), CellState::DEAD);

  board.setCellState(3, 2, CellState::ALIVE);
  ASSERT_EQ(board.getCellState(3, 2), CellState::ALIVE);
}


TEST(Board, getAliveCellsBoundingRect) {
  Board board;
  std::stringstream ss = getStream(BOARD_EMPTY);
  board.load(ss);
  auto rect = board.getAliveCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 0);
  ASSERT_EQ(rect.top, 0);
  ASSERT_EQ(rect.bottom, 0);


  ss = getStream(BOARD_DEAD);
  board.load(ss);
  rect = board.getAliveCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 0);
  ASSERT_EQ(rect.top, 0);
  ASSERT_EQ(rect.bottom, 0);

  ss = getStream(BOARD_ALIVE);
  board.load(ss);
  rect = board.getAliveCellsBoundingRectangle();
  ASSERT_EQ(rect.left, 0);
  ASSERT_EQ(rect.right, 3);
  ASSERT_EQ(rect.top, 0);
  ASSERT_EQ(rect.bottom, 2);
}


TEST(Board, save) {
  Board board;
  std::stringstream ss = getStream(BOARD_EMPTY);
  board.load(ss);

  std::stringstream out_empty;
  board.save(out_empty, board.getAliveCellsBoundingRectangle());
  ASSERT_EQ(out_empty.str(), BOARD_EMPTY);

  ss = getStream(BOARD_DEAD);
  board.load(ss);
  std::stringstream out_dead;
  board.save(out_dead, {0, 0, 3, 3});

  ss = getStream(BOARD_ALIVE);
  board.load(ss);
  std::stringstream out_alive;
  board.save(out_alive, {1, 0, 3, 2});
  ASSERT_EQ(out_alive.str(), "**\n*_\n");
}


TEST(Board, reset) {
  Board board;
  std::stringstream ss = getStream(BOARD_ALIVE);
  board.load(ss);

  board.reset();
  ASSERT_EQ(board.height(), 0);
  ASSERT_EQ(board.length(), 0);
  board.reset(2, 3);
  ASSERT_EQ(board.getCellState(0, 0), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 0), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 1), CellState::DEAD);
  ASSERT_EQ(board.getCellState(0, 2), CellState::DEAD);
  ASSERT_EQ(board.getCellState(1, 2), CellState::DEAD);
}


TEST(Board, getAliveNeighborsCount) {
  Board board;
  std::stringstream ss = getStream(BOARD_ALIVE);
  board.load(ss);

  ASSERT_EQ(board.getAliveNeighborsCount(0, 0), 2);
  ASSERT_EQ(board.getAliveNeighborsCount(1, 0), 3);
  ASSERT_EQ(board.getAliveNeighborsCount(2, 0), 2);
  ASSERT_EQ(board.getAliveNeighborsCount(3, 0), 1);
  ASSERT_EQ(board.getAliveNeighborsCount(0, 1), 3);
  ASSERT_EQ(board.getAliveNeighborsCount(1, 1), 3);
  ASSERT_EQ(board.getAliveNeighborsCount(2, 1), 3);
  ASSERT_EQ(board.getAliveNeighborsCount(3, 1), 1);
  ASSERT_EQ(board.getAliveNeighborsCount(0, 2), 1);
  ASSERT_EQ(board.getAliveNeighborsCount(1, 2), 1);
  ASSERT_EQ(board.getAliveNeighborsCount(2, 2), 1);
  ASSERT_EQ(board.getAliveNeighborsCount(3, 2), 0);

  // cells outside of the board
  ASSERT_EQ(board.getAliveNeighborsCount(-1, -1), 1);
  ASSERT_EQ(board.getAliveNeighborsCount(10, 10), 0);

}


TEST(Rules, cellShouldDie) {
  Rules rules = {};
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
  Rules rules;
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
  Engine e1 = Engine(Board());
  e1.next();
  ASSERT_EQ(convertBoardToString(e1.board()), "");

  Board board;
  std::stringstream ss = getStream(BOARD_DEAD);
  board.load(ss);
  Engine e2 = Engine(std::move(board));
  e2.next();
  ASSERT_EQ(convertBoardToString(e2.board()), "");

  // test non empty board
  ss = getStream(BOARD_ALIVE);
  board.load(ss);
  Engine e3 = Engine(std::move(board));
  e3.next();
  ASSERT_EQ(convertBoardToString(e3.board()), "_*_\n***\n***\n");

  e3.next();
  ASSERT_EQ(convertBoardToString(e3.board()), "***\n___\n*_*\n_*_\n");

  e3.next();
  ASSERT_EQ(convertBoardToString(e3.board()), "_*_\n_*_\n*_*\n_*_\n_*_\n");

  e3.next();
  ASSERT_EQ(convertBoardToString(e3.board()), "***\n*_*\n***\n");

  e3.next();const std::string BOARD_ALIVE = "***_\n_*__\n____\n";
  ASSERT_EQ(convertBoardToString(e3.board()), "__*__\n_*_*_\n*___*\n_*_*_\n__*__\n");
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

