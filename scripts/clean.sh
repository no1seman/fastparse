#!/bin/bash

cd $(dirname $(readlink -f $0))/..

# cleaning cmake files
rm -f -R ./CMakeFiles
rm -f cmake_install.cmake
rm -f CMakeCache.txt
rm -f Makefile
rm -f -R ./src/ast/__pycache__
rm -f ./*.so
rm -f ./*.a

# cleaning generated files
rm -f ./src/Ast.h
rm -f ./src/AstVisitor.h
rm -f ./src/Ast.cpp
rm -f ./src/GraphQLAst.h
rm -f ./src/GraphQLAst.cpp
rm -f ./src/GraphQLAstForEachConcreteType.h
rm -f ./src/JsonVisitor.h.inc
rm -f ./src/JsonVisitor.cpp.inc
rm -f ./src/LuaVisitor.h.inc
rm -f ./src/LuaVisitorStruct.cpp.inc
rm -f ./src/LuaVisitor.cpp.inc

# cleaning flex builts
rm -f ./src/parser/lexer.cpp
rm -f ./src/parser/lexer.h

# cleaning bison builts
rm -f ./src/parser/parser.tab.cpp
rm -f ./src/parser/parser.tab.hpp

# compile_commands.json
rm -f ./compile_commands.json

# test tmp cleanup
rm -rf ./test/tmp

# build rock cleanup
rm -rf ./.rocks
rm -rf ./build.luarocks
rm -f ./*.rock

# clean clang tidy
rm -rf clang.log

# clean static analisys
rm -rf ./static
