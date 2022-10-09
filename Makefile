# File              : Makefile
# Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
# Date              : 06.12.2021
# Last Modified Date: 09.10.2022
# Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>

PWD=`pwd`
PROJECT_NAME=gstroybat

all: macTest

macTest:
	mkdir -p build && cd build && cmake .. && make && echo "lldb ${PWD}/$(PROJECT_NAME).app/Contents/MacOS/$(PROJECT_NAME) -o 'r'">run && chmod +x run && open run

mac:
	export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig" && mkdir -p build && cd build && cmake .. -DCMAKE_OSX_ARCHITECTURES="arm64" && make && make install

mac_x86_64:
	export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig" && mkdir -p build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=../macos.cmake  -DCMAKE_OSX_ARCHITECTURES="x86_64" && make install 

source:
	mkdir -p build && cd build && cmake .. && make package_source

win:
	export PKG_CONFIG_PATH="/opt/GTK3SDK-mingw64/lib/pkgconfig" && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw.cmake .. && make && make package

winTest:
	export PKG_CONFIG_PATH="/opt/GTK3SDK-mingw64/lib/pkgconfig" && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw.cmake .. && make

win32:
	export PKG_CONFIG_PATH="/opt/GTK3SDK-mingw32/lib/pkgconfig" && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw32.cmake .. && make && make package

package:
	cd build && make package

clean:
	rm -fr build

.Phony: mac package
