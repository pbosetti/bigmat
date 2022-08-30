cmake_minimum_required(VERSION 3.10)
project(BIGMAT VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Native targets default to Debug" FORCE)
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
file(GLOB LIB_SOURCES_CPP "${SOURCE_DIR}/[^_]*.cpp")
file(GLOB LIB_SOURCES "${SOURCE_DIR}/[^_]*.c")


add_library(bigmat STATIC ${LIB_SOURCES})
add_executable(test ${SOURCE_DIR}/main/test.c)
target_link_libraries(test bigmat)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR})
install(TARGETS bigmat test
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)