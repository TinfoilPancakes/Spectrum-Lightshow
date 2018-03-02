all: main.cpp SharedMemoryObject.hpp SoftPWMControl.hpp GPIOInterface.hpp
	g++ -std=c++14 main.cpp -o lightshow -lrt -lpthread -O3 -ffast-math

r2: Sources/pa_main.cpp Sources/PulseAudioSource.cpp
	g++ -std=c++14 \
	./Sources/pa_main.cpp ./Sources/PulseAudioSource.cpp \
	-o pulse_test \
	-lm -lpulse -lpulse-simple -lfftw3 \
	-O2 -ffast-math \
	-I./Includes

r3: test_pulse.cpp Sources/PulseAudioSource.cpp
	g++ -std=c++14 \
	test_pulse.cpp ./Sources/PulseAudioSource.cpp \
	-o pulse_test_orig \
	-lm -lpulse -lpulse-simple -lfftw3 \
	-O2 -ffast-math \
	-I./Includes

clean:
	rm -f lightshow

re: clean all

.PHONY: clean re
	
