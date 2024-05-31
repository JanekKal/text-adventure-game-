CXX = g++
CXXFLAGS = -O0 -g3 -std=c++17

main: main.cpp json.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o main

deepclean:
	rm -f *~ *.o main *.stackdump

clean:
	rm -f *~ *.o *.stackdump
