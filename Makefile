CC       ?= gcc
CXX      ?= g++
CFLAGS   ?= -Wall -Werror -std=c99
CXXFLAGS ?= -Wall -Werror -std=c++11

%: %.c
	$(CC) $(CFLAGS) $< -o $@

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@