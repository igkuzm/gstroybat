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
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
#pkg_check_modules(GTK REQUIRED gtk4)

if(ANDROID)
elseif(APPLE)
elseif(WIN32)
	#add WebView2
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/winapi/WebView2/include)
	SET(ADDLIBS WebView2Loader shlwapi)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit	
		SET(ADDDIRS ${CMAKE_SOURCE_DIR}/winapi/WebView2/x64)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
		SET(ADDDIRS ${CMAKE_SOURCE_DIR}/winapi/WebView2/x86)
	endif()	
else()
	#add webkit2gtk
	pkg_check_modules(WEBKITGTK REQUIRED webkit2gtk-4.0)
	SET(ADDLIBS ${WEBKITGTK_LIBRARIES})
	SET(ADDINCL ${WEBKITGTK_INCLUDE_DIRS})
	SET(ADDDIRS ${WEBKITGTK_LIBRARY_DIRS})
endif()

#######
set(STROYBAT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/stroybat)
add_subdirectory(${STROYBAT_SOURCE_DIR})
#######

# Files to copy to bundle
set(RESOURCE_FILES 
	stroybat/stroybat.db 
	stroybat/Template.xlsx
)

# SOURCES
add_executable(${TARGET} 
	main.c 
	mainWindow.c 
	smetaView.c 
	smetaEdit.c
	materialsView.c 
	servicesView.c 
	itemsListView.c 
	itemsTableModel.c
	mainMenu.c
	YDConnect.cc
	${RESOURCE_FILES}
)
target_link_libraries(${TARGET} PRIVATE 
	${GTK_LIBRARIES} 
	stroybat 
	${ADDLIBS}
)

#WinAPI DIRS
if (WIN32)
	set(WINAPI ${CMAKE_SOURCE_DIR}/winapi)
	set(WINAPI_GTK ${WINAPI}/gtk3)
	set(WINAPI_WEBVIEW2 ${WINAPI}/WebView2)
endif()

# Add other flags to the compiler
#target_compile_definitions(${TARGET} PRIVATE ${GTK_CFLAGS_OTHER})

# Setup CMake to use GTK, tell the compiler where to look for headers
# and to the linker where to look for libraries
target_include_directories(${TARGET} PRIVATE 
	${GTK_INCLUDE_DIRS} 
	${ADDINCL}
)
target_link_directories(${TARGET} PRIVATE 
	${GTK_LIBRARY_DIRS} 
	${CMAKE_PREFIX_PATH}/lib 
	${ADDDIRS}
)

if (APPLE)
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}  -framework WebKit -ObjC -lstdc++")
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
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-allow-multiple-definition -lstdc++")
	
	#Make directory
	set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${TARGET})
	file(MAKE_DIRECTORY ${INSTALL_DIR})
	
	#Copy DLLs
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit	
		file(GLOB files 
			"${WINAPI_GTK}/x64/*.dll"
			"${WINAPI_WEBVIEW2}/x64/*.dll"
			"${CMAKE_BINARY_DIR}/*.dll"
			)	
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
		file(GLOB files 
			"${WINAPI_GTK}/x86/*.dll"
			"${WINAPI_WEBVIEW2}/x86/*.dll"
			"${CMAKE_BINARY_DIR}/*.dll"
			)	
	endif()	
	
	#Copy
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


