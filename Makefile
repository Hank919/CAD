CXX = g++            
CXXFLAGS = -std=c++11 

TARGET = CAD_tool   
SOURCE = CAD_tool.cpp 

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)