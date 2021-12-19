cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(21Z-EADS-TASK2-MILLER-ARTUR)

add_compile_options(-Wall -Wextra -pedantic -Werror -g)

include(FetchContent)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC bi_ring)

target_link_libraries(${PROJECT_NAME} PUBLIC gtest_main)
