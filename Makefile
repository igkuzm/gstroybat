# File              : Makefile
# Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
# Date              : 06.12.2021
# Last Modified Date: 12.02.2022
# Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>


all:
	mkdir -p build && cd build && cmake .. && make && open gstroybat.app/Contents/MacOS/gstroybat


clean:
	rm -fr build
