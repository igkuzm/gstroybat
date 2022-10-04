# File              : Makefile
# Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
# Date              : 06.12.2021
# Last Modified Date: 04.10.2022
# Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>

PWD=`pwd`
PROJECT_NAME=gstroybat

all:
	mkdir -p build && cd build && cmake .. && make && echo "lldb ${PWD}/$(PROJECT_NAME).app/Contents/MacOS/$(PROJECT_NAME) -o 'r'">run && chmod +x run && open run

mac:
	mkdir -p build && cd build && cmake .. && make install

win:
	export PKG_CONFIG_PATH="/opt/GTK3SDK-mingw64/lib/pkgconfig" && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw.cmake .. && make && open $(PROJECT_NAME)/$(PROJECT_NAME).exe

clean:
	rm -fr build

.Phony: mac
