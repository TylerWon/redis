CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -g

# Executables
CLIENT = client
SERVER = server

# Sources
CLIENT_SRC = client.cpp net_utils.cpp log.cpp
SERVER_SRC = server.cpp net_utils.cpp log.cpp

# Objects
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Rules
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(CLIENT) $(SERVER) *.o

.PHONY: all
