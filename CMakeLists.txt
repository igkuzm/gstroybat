cmake_minimum_required(VERSION 3.2)

set(TARGET gstroybat)

set(VERSION 1.0)

project(${TARGET} 
	VERSION ${VERSION} 
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
#pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
#pkg_check_modules(GTK REQUIRED gtk4)

if(ANDROID)
elseif(APPLE)
elseif(WIN32)
	#add icon
	set(ADDSRC AppIcons/AppIcon.rc)
	#add WebView2
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/winapi/WebView2/include)
	SET(ADDLIBS WebView2Loader shlwapi)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit	
		SET(ADDDIRS ${CMAKE_SOURCE_DIR}/winapi/WebView2/x64)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
		SET(ADDDIRS ${CMAKE_SOURCE_DIR}/winapi/WebView2/x86)
		#SET_TARGET_PROPERTIES(shlwapi PROPERTIES IMPORTED_LOCATION {CMAKE_SOURCE_DIR}/winapi/WebView2/x86/shlwapi.lib)
		#SET(ADDSRC shlwapi)
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
	AppIcons/AppIcon.png
	AppIcons/icon.png
	License.md	
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
	config.c
	toast.c
	YDConnect.cc
	${ADDSRC}
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

#include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.md")
set(CPACK_PACKAGE_FILE_NAME "${TARGET}-${VERSION}")

if (APPLE)
	SET(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}  -framework WebKit -ObjC -lstdc++ ${ADDFLAGS}")

	#add icons
	target_sources(${TARGET} PRIVATE AppIcons/Assets.xcassets)
	set_source_files_properties(AppIcons/Assets.xcassets PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	set_target_properties(${TARGET} PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")

	
	set(BUNDLENAME ${TARGET})
	set(PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
	set_target_properties(${TARGET} PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_ICON_FILE AppIcon.icns
		MACOSX_BUNDLE_INFO_PLIST ${PLIST}
		MACOSX_BUNDLE_BUNDLE_NAME ${BUNDLENAME}
		MACOSX_BUNDLE_GUI_IDENTIFIER kuzm.ig.${TARGET}
		MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
		RESOURCE "${RESOURCE_FILES}"
	)

	set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ${CPACK_RESOURCE_FILE_LICENSE})
	set(CPACK_DMG_VOLUME_NAME ${TARGET})
	#set(CPACK_DMG_FORMAT UDBZ)
	#set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/CMakeDMGSetup.scpt")
	set (CPACK_DMG_BACKGROUND_IMAGE ${CMAKE_SOURCE_DIR}/AppIcons/CMakeDMGBackground.tif)
	set(CPACK_PACKAGE_EXECUTABLES "${TARGET}" "${TARGET}")
	#set (CPACK_CPACK_DMG_DS_STORE ${PROJECT_SOURCE_DIR}/macInstallerFiles/DS_Store)

	#generate icons, copy GTK libs to bundle and code sign
	SET(IDENTITY "Apple Development: Igor Sementsov (58E746N546)")
	
	#make script to sign files
	execute_process(
		COMMAND echo 
		"#/bin/bash
		codesign --force --timestamp=none --sign \"${IDENTITY}\" ${CMAKE_BINARY_DIR}/${TARGET}.app/Contents/Resources/lib/*
		codesign --force --timestamp=none --sign \"${IDENTITY}\" ${CMAKE_BINARY_DIR}/${TARGET}.app/Contents/Frameworks/*
		codesign --force --timestamp=none --sign \"${IDENTITY}\" ${CMAKE_BINARY_DIR}/${TARGET}.app
		" 
		OUTPUT_FILE codesign.sh 
	)
	
	INSTALL(CODE 
		"
			include(InstallRequiredSystemLibraries)
			include(BundleUtilities) 
			fixup_bundle(
				\"${CMAKE_BINARY_DIR}/${TARGET}.app\" 
				\"\" 
				\"${GTK_LIBRARY_DIRS}\"
			)
			execute_process(COMMAND 
				actool 
				--output-format human-readable-text
				--app-icon AppIcon 
				--output-partial-info-plist assetcatalog_generated_info.plist 
				--compress-pngs --enable-on-demand-resources YES
				--platform macosx
				--minimum-deployment-target \"${CMAKE_OSX_DEPLOYMENT_TARGET}\" 
				--compile . 
				\"${CMAKE_CURRENT_SOURCE_DIR}/AppIcons/Assets.xcassets\" 
			)			
			execute_process(COMMAND
				cp AppIcon.icns \"${CMAKE_BINARY_DIR}/${TARGET}.app/Contents/Resources\" 
			)		
			execute_process(COMMAND
				cp -rf \"${CMAKE_CURRENT_SOURCE_DIR}/macos/lib\"
				\"${CMAKE_BINARY_DIR}/${TARGET}.app/Contents/Resources/.\" 
			)		
			execute_process(COMMAND
				cp -rf \"${CMAKE_CURRENT_SOURCE_DIR}/macos/share\"
				\"${CMAKE_BINARY_DIR}/${TARGET}.app/Contents/Resources/.\" 
			)		
			execute_process(COMMAND
				chmod +x codesign.sh
			)
			execute_process(COMMAND
				./codesign.sh
			)		
		" 
		COMPONENT application
	)
	INSTALL(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_BINARY_DIR} BUNDLE DESTINATION ${CMAKE_BINARY_DIR})
	include(CPack)
elseif (WIN32)
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -mwindows -Wl,-allow-multiple-definition -lstdc++")
	#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-allow-multiple-definition -lstdc++")
	
	#Copy DLLs
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit	
		SET(ARCHDIR "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
		SET(ARCHDIR "x86")
	endif()	
	
	file(GLOB WebKitEngine
		"${WINAPI_WEBVIEW2}/MicrosoftEdgeWebview2Setup.exe"
	)
	file(GLOB GTKAPI
		"${WINAPI_GTK}/${ARCHDIR}/*.dll"
	)
	file(GLOB WEBVIEW2 
		"${WINAPI_WEBVIEW2}/${ARCHDIR}/*.dll"
	)
	file(GLOB CURL 
		"${CMAKE_BINARY_DIR}/*.dll"
	)	

	install(FILES ${WebKitEngine} DESTINATION . COMPONENT WebKitEngineInstaller)	
	install(FILES ${GTKAPI} DESTINATION bin COMPONENT GTK3_runtime)	
	install(DIRECTORY ${WINAPI_GTK}/${ARCHDIR}/gdk-pixbuf-2.0 DESTINATION lib COMPONENT GTK3_runtime)	
	install(DIRECTORY ${WINAPI_GTK}/share DESTINATION . COMPONENT GTK3_runtime)	
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/AppIcons/icons DESTINATION share COMPONENT GTK3_runtime)	
	install(FILES ${WEBVIEW2} DESTINATION bin COMPONENT WebView2_runtime)	
	install(FILES ${CURL} DESTINATION bin COMPONENT libcurl_runtime)	
	install(FILES ${RESOURCE_FILES} DESTINATION bin COMPONENT resources)	
	install(TARGETS ${TARGET} RUNTIME DESTINATION bin COMPONENT gstroybat)
	
	set(CPACK_GENERATOR NSIS)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "${TARGET}")
	set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/AppIcons/icon.ico")
	set(CPACK_NSIS_MUI_UNIICON  "${CMAKE_SOURCE_DIR}/AppIcons/icon.ico")
	set(CPACK_NSIS_PACKAGE_NAME "${TARGET}")
	set(CPACK_NSIS_DISPLAY_NAME "${TARGET}")
	set(CPACK_NSIS_CONTACT "ig.kuzm@gmail.com")
	set(CPACK_PACKAGE_EXECUTABLES "${TARGET}" "${TARGET}")
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${VERSION}")
	set(CPACK_CREATE_DESKTOP_LINKS "${TARGET}")
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL TRUE)

	# Icon in the add/remove control panel. Must be an .exe file 
	set(CPACK_NSIS_INSTALLED_ICON_NAME bin\\\\${TARGET}.exe)

	set(CPACK_NSIS_URL_INFO_ABOUT "ig.kuzm@gmail.com")
	set(CPACK_NSIS_HELP_LINK "ig.kuzm@gmail.com")
	
	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
		ExecWait '\\\"$INSTDIR\\\\MicrosoftEdgeWebview2Setup.exe\\\" /install'\n		
		CreateShortCut \\\"$DESKTOP\\\\${TARGET}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${TARGET}.exe\\\"
	")

	set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
		Delete \\\"$DESKTOP\\\\${TARGET}.lnk\\\"
	")
	include(CPack)
	
	cpack_add_component(WebKitEngineInstaller
		DISPLAY_NAME  "WebKit Engine (Edge) Installer"
		DESCRIPTION   "Need to instal for Windows 7 to Windows 10. No need to install for Windows 11"
		GROUP WebView2
	)
	cpack_add_component(WebView2_runtime
		DISPLAY_NAME  "WebView2 runtime libraries"
		DESCRIPTION   "The Microsoft Edge WebView2 control allows you to embed web technologies (HTML, CSS, and JavaScript) in your native apps. The WebView2 control uses Microsoft Edge as the rendering engine to display the web content in native apps."
		GROUP WebView2
	)
	cpack_add_component(GTK3_runtime
		DISPLAY_NAME  "GTK+ runtime libraries"
		DESCRIPTION   "GTK+ runtime libraries for Windows (version 3.24)"
	)
	cpack_add_component(libcurl_runtime
		DISPLAY_NAME  "libcurl runtime libraries"
		DESCRIPTION   "libcurl runtime libraries for Windows"
	)
	cpack_add_component(resources
		DISPLAY_NAME  "Resource files for gstroybat"
		DESCRIPTION   "database files, templates"
	)
	cpack_add_component(gstroybat
		DISPLAY_NAME  "Application gstroybat"
		DESCRIPTION   "Executable application gstroybat"
		DEPENDS WebView2_runtime GTK3_runtime libcurl_runtime
	)
	
else()
	#Linux and Unix systems
	if (NOT ANDROID)
		SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread -ldl")
	endif()	
	install(TARGETS ${TARGET} RUNTIME DESTINATION bin)	
	install(FILES ${RESOURCE_FILES} DESTINATION share/${TARGET})	
	#icon and desktop file
	INSTALL (FILES ${TARGET}.desktop DESTINATION /usr/share/applications/)
	INSTALL (FILES ${TARGET}.xml DESTINATION /usr/share/applications/)
	INSTALL (FILES AppIcons/${TARGET}.png DESTINATION /usr/share/icons/)
	include(CPack)
endif()

