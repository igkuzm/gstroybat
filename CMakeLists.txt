cmake_minimum_required(VERSION 3.0)
project(gstroybat VERSION 1.0 
	DESCRIPTION "GTK GUI application for Smeta"
				  HOMEPAGE_URL ""
				  LANGUAGES C CXX )


set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

#add_subdirectory(libxlsxwriter) #library to work with XLSX
#add_subdirectory(OpenXLSX) #library to work with XLSX
#add_subdirectory(SQLiteConnect) #library to work with SQLite

add_executable(gstroybat main.c mainWindow.c smetaView.c)
target_link_libraries(gstroybat PRIVATE ${GTK_LIBRARIES})


# Add other flags to the compiler
target_compile_definitions(gstroybat PRIVATE ${GTK_CFLAGS_OTHER})


# Setup CMake to use GTK4, tell the compiler where to look for headers
# and to the linker where to look for libraries
target_include_directories(gstroybat PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(gstroybat PRIVATE ${GTK_LIBRARY_DIRS})

#target_link_libraries(stroybat xlsxwriter OpenXLSX SQLiteConnect)

#configure_file(stroybat.db stroybat.db COPYONLY)
#configure_file(stroybat_data.db stroybat_data.db COPYONLY)

