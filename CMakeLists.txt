cmake_minimum_required(VERSION 2.6)

project(loris)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADERS 
	include/loris/assembly.hpp
	include/loris/ast.hpp
	include/loris/compiler.hpp
	include/loris/error.hpp
	include/loris/lexer.hpp
	include/loris/loris.hpp
	include/loris/parser.hpp
	include/loris/virtualmachine.hpp
	include/loris/bind.hpp

	include/loris/libs/math.hpp
	include/loris/libs/utils.hpp
    )

set(SRCS
	src/assembly.cpp 
	src/compiler.cpp 
	src/lexer.cpp 
	src/parser.cpp 
	src/virtualmachine.cpp 
	src/loris.cpp
    )

add_library(loris STATIC ${SRCS} ${HEADERS})