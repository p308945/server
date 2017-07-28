CXXFLAGS= -Wall -g -Wno-deprecated -std=c++0x -Wno-unused-function -shared -fPIC -static-libasan -fsanitize=leak
TARGET=libCoroutine.so
CXX=g++
INCLUDE_DIR= -I ../../../util

.PHONY : $(TARGET)
OBJS=$(patsubst ./%.cpp, ./%.o, $(wildcard ./*.cpp))

%.o:%.cpp
		$(CXX) $(INCLUDE_DIR) $(CXXFLAGS) -c $< -o $@ $(LIBFLAGS)

$(TARGET) : $(OBJS)
		$(CXX) $(INCLUDE_DIR) $(CXXFLAGS) $(OBJS) -o $@ $(LIBFLAGS)

clean:
		rm -rf *.o $(TARGET)
