cmake_minimum_required(VERSION 3.16)
project(SES LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(ses src/main.cpp)

# Prefer the modern imported target when available (Ubuntu / most distros).
if(TARGET SDL2::SDL2)
  target_link_libraries(ses PRIVATE SDL2::SDL2)
else()
  target_include_directories(ses PRIVATE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(ses PRIVATE ${SDL2_LIBRARIES})
endif()
