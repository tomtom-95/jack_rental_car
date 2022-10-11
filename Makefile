.RECIPEPREFIX = <
.PHONY = main run clean

CPP = g++
CPPFLAGS = -Wall -std=c++20

jack_rental_car:
< ${CPP} ${CPPFLAGS} -o build/jack_rental_car_2 src/jack_rental_car_2.cpp

run:
< ./build/jack_rental_car_2

clean:
< rm -rf ./build/*


