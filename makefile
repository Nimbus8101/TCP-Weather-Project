CXX = g++

CXXFLAGS = -Wall -g

TARGET = tcpClient.exe tcpServer.exe
TARGET_DEL = tcpClient.exe tcpServer.exe

OBJS = $(SRCS:.cpp=.o)

# Define source files and executable names
SRC_EXEC1 = tcpClient.cpp
SRC_EXEC2 = tcpServer.cpp
EXEC1 = tcpClient
EXEC2 = tcpServer

all: $(EXEC1) $(EXEC2)

tcpServer: tcpServer.cpp
	g++ -Wall -g -o tcpServer tcpServer.cpp -lws2_32

tcpClient: tcpClient.cpp
	g++ -Wall -g -o tcpClient tcpClient.cpp -lws2_32

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lws2_32
# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
# Clean rule to remove generated files
clean:
	del $(TARGET_DEL) $(OBJS)