cmake_minimum_required (VERSION 3.8)

add_subdirectory(core)
find_package(Boost 1.70 REQUIRED COMPONENTS program_options REQUIRED)

add_executable(game_of_life "game_of_life.cpp")
target_link_libraries(game_of_life PRIVATE game_of_life_core Boost::program_options)
set_target_properties(game_of_life PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")