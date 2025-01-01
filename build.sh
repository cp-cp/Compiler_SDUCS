#! /bin/bash
# g++ -c lexer.cpp -o lexer.o -O2
# g++ -c parser.cpp -o parser.o -O2
# g++ -c main.cpp -o main.o -O2

g++ ./common/main.cpp ./common/parser.cpp ./common/Lexer.cpp ./common/Objector.cpp ./common/CodeGenerator.cpp ./common/Tables.cpp -o Main -std=c++14 -O2

# g++ ./common/main.cpp ./common/Objector.cpp -o Main -std=c++14 -O2
