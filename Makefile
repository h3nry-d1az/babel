CC       ?= gcc
CXX      ?= g++
CFLAGS   ?= -Wall -Wextra -Werror -std=c99
CXXFLAGS ?= -Wall -Wextra -Werror -std=c++11

%: %.c
	$(CC) $(CFLAGS) $(BUILDFLAGS) $< -o $@

%: %.cpp
	$(CXX) $(CXXFLAGS) $(BUILDFLAGS) $< -o $@