cmake_minimum_required(VERSION 3.0)

set(TARGET gstroybat)

project(${TARGET} 
	VERSION 1.0 
	DESCRIPTION "GTK GUI application for Smeta"
	HOMEPAGE_URL ""
	LANGUAGES C CXX 
)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Use pkg_config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

add_subdirectory(stroybat) #library

# Files to copy to bundle
set(RESOURCE_FILES 
	stroybat/stroybat.db 
	stroybat/stroybat_data.db 
	stroybat/Template.xlsx
)

# SOURCES
add_executable(${TARGET} 
	main.c 
	mainWindow.c 
	smetaView.c 
	itemsView.c 
	itemsListView.c 
	mainMenu.c
	${RESOURCE_FILES}
)
target_link_libraries(${TARGET} PRIVATE ${GTK_LIBRARIES} stroybat)

# Add other flags to the compiler
#target_compile_definitions(${TARGET} PRIVATE ${GTK_CFLAGS_OTHER})

# Setup CMake to use GTK4, tell the compiler where to look for headers
# and to the linker where to look for libraries
target_include_directories(${TARGET} PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(${TARGET} PRIVATE ${GTK_LIBRARY_DIRS} ${CMAKE_PREFIX_PATH}/lib)

if (APPLE)
	set_target_properties(${TARGET} PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
		MACOSX_BUNDLE_BUNDLE_NAME ${TARGET}
		MACOSX_BUNDLE_GUI_IDENTIFIER kuzm.ig.${TARGET}
		MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
		RESOURCE "${RESOURCE_FILES}"
	)

	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
	set(APP ${CMAKE_INSTALL_PREFIX}/${TARGET}.app)
	set(DIRS ${GTK_LIBRARY_DIRS})
	INSTALL(CODE "
		include(BundleUtilities)
		fixup_bundle(\"${APP}\" \"\" \"${DIRS}\")
		" COMPONENT Runtime)
endif()

if (WIN32)
	#Make directory
	set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${TARGET})
	file(MAKE_DIRECTORY ${INSTALL_DIR})
	
	#Copy DLLs
	#file(GLOB files "${GTK_LIBRARY_DIRS}/../bin/*.dll")
	file(GLOB files "${CMAKE_BINARY_DIR}/../winlibs/*")	
	foreach(_file ${files})
			file(INSTALL
				DESTINATION "${CMAKE_BINARY_DIR}/${TARGET}"
				FILES "${_file}"
			)
	endforeach()

	#Copy resources
	foreach(_file ${RESOURCE_FILES})
			file(INSTALL
				DESTINATION "${CMAKE_BINARY_DIR}/${TARGET}"
				FILES "${_file}"
			)
	endforeach()	

	#Copy EXE file
	add_custom_command(TARGET ${TARGET} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${TARGET}.exe ${INSTALL_DIR}/${TARGET}.exe
	)
endif()


