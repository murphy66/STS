cmake_minimum_required(VERSION 3.10)


# set the project name
project(ToyProj)
include_directories(${PROJECT_SOURCE_DIR}/src)

# add the executable
add_executable(ToyProj src/main.cpp src/card.cpp src/entity.cpp src/floor.cpp)
set_property(TARGET ToyProj PROPERTY CXX_STANDARD 17)

# tests
add_executable(Tests src/tests/tests.cpp src/card.cpp src/entity.cpp src/floor.cpp)
set_property(TARGET Tests PROPERTY CXX_STANDARD 17)
