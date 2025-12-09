cmake_minimum_required(VERSION 3.2)
project(pybind_test VERSION 0.1.0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(pybind11)

pybind11_add_module(Cal Val.cpp FEM.cpp NonLinear.cpp GenMatrix.cpp toolbox.cpp)
target_link_libraries(Cal PRIVATE)