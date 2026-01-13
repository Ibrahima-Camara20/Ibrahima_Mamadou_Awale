CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
AWALE_DIR = ../version-finale/src


AWALE_SRCS = $(AWALE_DIR)/game/Game.cpp \
             $(AWALE_DIR)/game/Bot.cpp \
             $(AWALE_DIR)/game/AlphaBetaBot.cpp \
             $(AWALE_DIR)/io/Parser.cpp

all: super_bot

# ============================================================================
# Bots
# ============================================================================

super_bot: main.cpp protocol.hpp
	$(CXX) $(CXXFLAGS) -I$(AWALE_DIR)/game -I$(AWALE_DIR)/io -I. main.cpp $(AWALE_SRCS) -o super_bot

clean:
	rm -f super_bot

.PHONY: all clean
