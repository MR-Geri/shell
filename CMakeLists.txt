cmake_minimum_required(VERSION 2.8)

# ADD_SUBDIRECTORY(googletest)

SET(CMAKE_CXX_STANDARD 11)

SET(MYCOMPILE_FLAGS "-Wextra -Wall -Wno-gnu-folding-constant")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MYCOMPILE_FLAGS}")

ADD_EXECUTABLE(rshell
    src/main.cpp
    src/commands.cpp
    src/connectors.cpp
    src/parser.cpp
    src/integrator.cpp
)

# ADD_EXECUTABLE(test
#     unit_tests/tests.cpp
#     src/commands.cpp
#     src/connectors.cpp
#     src/parser.cpp
#     src/integrator.cpp
# )

# TARGET_LINK_LIBRARIES(test gtest)
# TARGET_COMPILE_DEFINITIONS(test PRIVATE gtest_disable_pthreads=ON)
