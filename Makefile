CXX = g++            
CXXFLAGS = -std=c++11 

TARGET = 312510191    
SOURCE = 312510191.cpp 

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)