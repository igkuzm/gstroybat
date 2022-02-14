cmake_minimum_required(VERSION 3.0)
project(gstroybat VERSION 1.0 
	DESCRIPTION "GTK GUI application for Smeta"
				  HOMEPAGE_URL ""
				  LANGUAGES C CXX )


set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)

set(TARGET gstroybat)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

add_subdirectory(stroybat) #library

add_executable(${TARGET} main.c mainWindow.c smetaView.c itemsView.c cp.c chworkdir.c openfile.c
	itemsListView.c stroybat/stroybat.db stroybat/stroybat_data.db)
target_link_libraries(${TARGET} PRIVATE ${GTK_LIBRARIES} stroybat)

# Add other flags to the compiler
target_compile_definitions(${TARGET} PRIVATE ${GTK_CFLAGS_OTHER})

# Setup CMake to use GTK4, tell the compiler where to look for headers
# and to the linker where to look for libraries
target_include_directories(${TARGET} PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(${TARGET} PRIVATE ${GTK_LIBRARY_DIRS})

set(RESOURCE_FILES stroybat/stroybat.db stroybat/stroybat_data.db)

set_target_properties(${TARGET} PROPERTIES
	
	MACOSX_BUNDLE TRUE
	MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
	MACOSX_BUNDLE_BUNDLE_NAME ${TARGET}
	MACOSX_BUNDLE_GUI_IDENTIFIER kuzm.ig.${TARGET}
	MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}

	RESOURCE "${RESOURCE_FILES}"
)

