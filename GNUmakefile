B	:= Release

all: dobuild

clean:
	rm -rf build

dobuild: | build
	cmake --build build

config:
	cmake -B build -DCMAKE_BUILD_TYPE=$(B)
	
vs2022: # make .sln  
	cmake -G "Visual Studio 17 2022" -B vs2022

run:
	./build/vAmigaSDL -2 HDD.hdf HDD.retrosh

.PHONY: all clean config dobuild run vs2022
