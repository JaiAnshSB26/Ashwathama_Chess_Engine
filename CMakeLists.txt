#target_link_libraries(Ashwathama PRIVATE Ashwathama_lib)

cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Ashwathama
  VERSION 1.0
  LANGUAGES CXX)

# Include the current directory for header files
include_directories(.)

# Create the library from the implementation files


# Add the executable
add_executable(Ashwathama 
  board.hpp 
  file_interpreter.hpp
  main.cpp
  utils.hpp
  evaluate.hpp
  moves.hpp
  attacks.hpp
)

# Link the library to the executable
#target_link_libraries(Ashwathama PRIVATE Ashwathama_lib)