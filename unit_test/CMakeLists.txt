CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (minicore_test)

SET (CMAKE_VERBOSE_MAKEFILE false)

SET (CMAKE_C_COMPILER gcc)

ADD_COMPILE_OPTIONS (-Wall -O)

INCLUDE_DIRECTORIES (..)

#LINK_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}/..)
LINK_DIRECTORIES (..)
LINK_LIBRARIES (minicore pthread dl)

SET (SRCS string_test.c properties_test.c test_main.c)

ADD_EXECUTABLE (test ${SRCS})
