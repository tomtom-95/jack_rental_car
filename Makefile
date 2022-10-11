.RECIPEPREFIX = <
.PHONY = main run clean

CPP = g++
CPPFLAGS = -Wall -std=c++20

jack_rental_car:
< ${CPP} ${CPPFLAGS} -o build/jack_rental_car src/jack_rental_car.cpp

run:
< ./build/jack_rental_car

clean:
< rm -rf ./build/*


