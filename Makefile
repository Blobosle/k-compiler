all:
	g++ -o compiler std=c++20 src/lex.cpp src/parser.cpp
