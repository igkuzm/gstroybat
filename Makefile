# File              : Makefile
# Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
# Date              : 06.12.2021
# Last Modified Date: 05.03.2022
# Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>

all:
	mkdir -p build && cd build && cmake .. && make && open gstroybat.app/Contents/MacOS/gstroybat

mac:
	mkdir -p build && cd build && cmake .. && make && make install && open gstroybat.app/Contents/MacOS/gstroybat

win:
	mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw.cmake .. && make && open gstroybat/gstroybat.exe

clean:
	rm -fr build

.Phony: mac
