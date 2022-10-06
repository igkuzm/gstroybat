# File              : Makefile
# Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
# Date              : 06.12.2021
# Last Modified Date: 06.10.2022
# Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>

PWD=`pwd`
PROJECT_NAME=gstroybat

all:
	mkdir -p build && cd build && cmake .. && make && echo "lldb ${PWD}/$(PROJECT_NAME).app/Contents/MacOS/$(PROJECT_NAME) -o 'r'">run && chmod +x run && open run

mac:
	mkdir -p build && cd build && cmake .. && make install && cpack -G DragNDrop

win:
	export PKG_CONFIG_PATH="/opt/GTK3SDK-mingw64/lib/pkgconfig" && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw.cmake .. && make && make package

win32:
	export PKG_CONFIG_PATH="/opt/GTK3SDK-mingw32/lib/pkgconfig" && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../mingw32.cmake .. && make && make package

package:
	cd build && make package

clean:
	rm -fr build

.Phony: mac package
