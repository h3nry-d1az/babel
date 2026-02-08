CC       ?= gcc
CXX      ?= g++
CFLAGS   ?= -Wall -Wextra -Werror -std=c99
CXXFLAGS ?= -Wall -Wextra -Werror -std=c++11

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(BUILDFLAGS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(BUILDFLAGS)