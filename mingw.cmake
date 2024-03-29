set(CMAKE_SYSTEM_NAME Windows)

SET(CMAKE_PREFIX_PATH  "/opt/GTK3SDK-mingw64")
#SET(SYSROOT  "/opt/GTK3SDK-mingw64")
#SET(ENV{PKG_CONFIG_SYSROOT_DIR} /opt/GTK3SDK-mingw64)
SET(ENV{PKG_CONFIG_PATH} "/opt/GTK3SDK-mingw64/lib/pkgconfig")
#SET(ENV{PKG_CONFIG_LIBDIR} /opt/GTK3SDK-mingw64/lib/pkgconfig)

# cross compilers to use for C and C++
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

