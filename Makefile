CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -g

# Executables
CLIENT = client
SERVER = server

# Common sources (all shared modules go here)
COMMON_SRC = buf_utils.cpp log.cpp net_utils.cpp Request.cpp Response.cpp
COMMON_OBJ = $(COMMON_SRC:.cpp=.o)

# Unique sources
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp

CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Rules
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER): $(SERVER_OBJ) $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(CLIENT) $(SERVER) *.o

.PHONY: all clean
