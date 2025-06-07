cmake_minimum_required(VERSION 3.11)

project(
  Zust
  VERSION 0.1
  DESCRIPTION "Zust"
)

add_executable(
  Zust
  zust.cpp
  ParserExpression.cpp
  ParserStatement.cpp
  ParserDeclaration.cpp
  CodeGeneratorExpression.cpp
  CodeGeneratorStatement.cpp
)