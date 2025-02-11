B	:= Release
B	:= Debug

all: dobuild

dobuild: | build
	cmake --build build

ifeq ($(OS),Windows_NT)
config:
	cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=$(B) --preset=vcpkg
else
config:
	cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=$(B)
endif
	
vs2022: # make .sln  
	cmake -G "Visual Studio 17 2022" -B vs2022 --preset=vcpkg
#-DWITH_VCPKG=ON
#-DCMAKE_C_FLAGS=-Wno-unused-result

install:
	cmake --install build
	
clean:
	rm -rf build

run:
	./build/vAmiga HDD.hdf HDD.txt

.PHONY: all clean config dobuild install run vs2022
