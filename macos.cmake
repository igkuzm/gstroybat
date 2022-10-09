if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	SET(CMAKE_PREFIX_PATH  "/usr/local")
	SET(ENV{PKG_CONFIG_PATH} PKG_CONFIG_PATH="/usr/local/lib/pkgconfig")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm64")
	SET(ENV{PKG_CONFIG_PATH} PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig")
	SET(CMAKE_PREFIX_PATH  "/opt/homebrew")
endif()


