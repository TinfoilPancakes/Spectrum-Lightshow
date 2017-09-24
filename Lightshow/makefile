all: main.cpp SharedMemoryObject.hpp SoftPWMControl.hpp GPIOInterface.hpp
	g++ -std=c++14 main.cpp -o lightshow -lrt -lpthread -O3 -ffast-math

clean:
	rm -f lightshow

re: clean all

.PHONY: clean re
	