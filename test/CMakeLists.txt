cmake_minimum_required (VERSION 3.8)
find_package(GTest REQUIRED)

add_executable(game_of_life_test "game_of_life_test.cpp")
target_link_libraries(game_of_life_test PRIVATE GTest::GTest game_of_life_engine)

add_test(NAME game_of_life_test COMMAND game_of_life_test)