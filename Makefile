CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -g

# Executables
CLIENT = client
SERVER = server

# Collect all source files automatically
COMMON_SRC := $(filter-out client.cpp server.cpp, \
              $(wildcard *.cpp) \
			  $(wildcard buffer/*.cpp) \
              $(wildcard hashmap/*.cpp) \
              $(wildcard hashmap/components/*.cpp) \
              $(wildcard request/*.cpp) \
              $(wildcard response/*.cpp) \
              $(wildcard response/types/*.cpp) \
			  $(wildcard utils/*.cpp))

# Object files
COMMON_OBJ := $(COMMON_SRC:.cpp=.o)

# Unique sources
CLIENT_SRC := client.cpp
SERVER_SRC := server.cpp

CLIENT_OBJ := $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ := $(SERVER_SRC:.cpp=.o)

# Rules
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER): $(SERVER_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(CLIENT) $(SERVER) $(COMMON_OBJ) $(CLIENT_OBJ) $(SERVER_OBJ)

.PHONY: all clean
