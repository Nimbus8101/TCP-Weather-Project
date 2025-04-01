CXX = g++

CXXFLAGS = -Wall -g

TARGET = httpClient.exe httpServer.exe
TARGET_DEL = httpClient.exe httpServer.exe

OBJS = $(SRCS:.cpp=.o)

# Define source files and executable names
SRC_EXEC1 = httpClient.cpp
SRC_EXEC2 = httpServer.cpp
EXEC1 = httpClient
EXEC2 = httpServer

all: $(EXEC1) $(EXEC2)

httpServer: httpServer.cpp
	g++ -Wall -g -o httpServer httpServer.cpp -lws2_32

httpClient: httpClient.cpp
	g++ -Wall -g -o httpClient httpClient.cpp -lws2_32

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lws2_32
# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
# Clean rule to remove generated files
clean:
	del $(TARGET_DEL) $(OBJS)