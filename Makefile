CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -g

# Source files
COMMON_SRC := $(filter-out client.cpp server.cpp, \
              $(wildcard *.cpp) \
              $(wildcard avl-tree/*.cpp) \
              $(wildcard avl-tree/components/*.cpp) \
			  $(wildcard buffer/*.cpp) \
              $(wildcard hashmap/*.cpp) \
              $(wildcard hashmap/components/*.cpp) \
              $(wildcard request/*.cpp) \
              $(wildcard response/*.cpp) \
              $(wildcard response/types/*.cpp) \
			  $(wildcard utils/*.cpp))
CLIENT_SRC := client.cpp
SERVER_SRC := server.cpp
TEST_SRC := $(shell find . -type f -path "*/tests/test_*.cpp")

# Object files
COMMON_OBJ := $(COMMON_SRC:.cpp=.o)
CLIENT_OBJ := $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ := $(SERVER_SRC:.cpp=.o)
TEST_OBJ := $(TEST_SRC:.cpp=.o)

# Executables
CLIENT = client
SERVER = server
TEST_BIN := $(notdir $(TEST_SRC:.cpp=))

# Rules
all: $(CLIENT) $(SERVER) $(TEST_BIN)

$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER): $(SERVER_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_BIN): %: $(TEST_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(filter %/$@.o,$(TEST_OBJ)) $(COMMON_OBJ)

test: $(TEST_BIN)
	@for t in $(TEST_BIN); do \
		echo "Running $$t..."; \
		./$$t || exit 1; \
		echo "Finished $$t..."; \
	done

clean:
	rm -f $(CLIENT) $(SERVER) $(TEST_BIN) $(COMMON_OBJ) $(CLIENT_OBJ) $(SERVER_OBJ) $(TEST_OBJ)

.PHONY: all clean
