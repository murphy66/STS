cmake_minimum_required(VERSION 3.10)


# set the project name
project(ToyProj)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/SFML/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SFML/)

# Find SFML
## If you want to link SFML statically
#set(SFML_STATIC_LIBRARIES TRUE)
#set(SFML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SFML/)
#find_package(SFML REQUIRED COMPONENTS audio graphics window system)


# tests
add_executable(Tests src/tests/tests.cpp src/card.cpp src/entity.cpp src/floor.cpp)
set_property(TARGET Tests PROPERTY CXX_STANDARD 17)


# add the executable
add_executable(ToyProj src/main.cpp src/card.cpp src/entity.cpp src/floor.cpp src/consolerenderer.cpp src/graphicsrenderer.cpp)
set_property(TARGET ToyProj PROPERTY CXX_STANDARD 17)
target_link_libraries(ToyProj sfml-graphics)


#file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_custom_command(TARGET ToyProj POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/BADABB__.TTF 
	$<TARGET_FILE_DIR:${PROJECT_NAME}>/BADABB__.TTF)

add_custom_command(TARGET ToyProj POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/ironclad.png
	$<TARGET_FILE_DIR:${PROJECT_NAME}>/ironclad.png)