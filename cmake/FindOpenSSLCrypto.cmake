# A Modified Version of FindOpenSSL.cmake from CMake 2.6.2
# Because we only need the crypto library for QTerm

# - Try to find the OpenSSL encryption library
# Once done this will define
#
#  OPENSSL_FOUND - system has the OpenSSL library
#  OPENSSL_INCLUDE_DIR - the OpenSSL include directory
#  OPENSSL_LIBRARIES - The libraries needed to use OpenSSL

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF(OPENSSLCRYPTO_LIBRARIES)
   SET(OpenSSLCRYPTO_FIND_QUIETLY TRUE)
ENDIF(OPENSSLCRYPTO_LIBRARIES)

IF(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)
   SET(LIB_FOUND 1)
ENDIF(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)

# Support preference of static libs by adjusting CMAKE_FIND_LIBRARY_SUFFIXES
if(OPENSSL_USE_STATIC_LIBS)
  set(_openssl_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
  if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a )
  endif()
endif()

if (WIN32)
  # http://www.slproweb.com/products/Win32OpenSSL.html
  set(_OPENSSL_ROOT_HINTS
    ${OPENSSL_ROOT_DIR}
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL (32-bit)_is1;Inno Setup: App Path]"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL (64-bit)_is1;Inno Setup: App Path]"
    ENV OPENSSL_ROOT_DIR
    )
  file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _programfiles)
  set(_OPENSSL_ROOT_PATHS
    "${_programfiles}/OpenSSL"
    "${_programfiles}/OpenSSL-Win32"
    "${_programfiles}/OpenSSL-Win64"
    "C:/OpenSSL/"
    "C:/OpenSSL-Win32/"
    "C:/OpenSSL-Win64/"
    )
  unset(_programfiles)
else ()
  set(_OPENSSL_ROOT_HINTS
    /opt/local/ # MacPorts
    /usr/local/opt/openssl # HomeBrew
    ${OPENSSL_ROOT_DIR}
    ENV OPENSSL_ROOT_DIR
    )
endif ()

set(_OPENSSL_ROOT_HINTS_AND_PATHS
    HINTS ${_OPENSSL_ROOT_HINTS}
    PATHS ${_OPENSSL_ROOT_PATHS}
    )

find_path(OPENSSLCRYPTO_INCLUDE_DIR
  NAMES
    openssl/ssl.h
  ${_OPENSSL_ROOT_HINTS_AND_PATHS}
  HINTS
    ${_OPENSSL_INCLUDEDIR}
  PATH_SUFFIXES
    include
)

#FIND_PATH(OPENSSLCRYPTO_INCLUDE_DIR openssl/ssl.h )

IF(WIN32 AND MSVC)
   # /MD and /MDd are the standard values - if someone wants to use
   # others, the libnames have to change here too
   # use also ssl and ssleay32 in debug as fallback for openssl < 0.9.8b
   # enable OPENSSL_MSVC_STATIC_RT to get the libs build /MT (Multithreaded no-DLL)
   # In Visual C++ naming convention each of these four kinds of Windows libraries has it's standard suffix:
   #   * MD for dynamic-release
   #   * MDd for dynamic-debug
   #   * MT for static-release
   #   * MTd for static-debug

   # Implementation details:
   # We are using the libraries located in the VC subdir instead of the parent directory eventhough :
   # libeay32MD.lib is identical to ../libeay32.lib, and
   # ssleay32MD.lib is identical to ../ssleay32.lib
   # enable OPENSSL_USE_STATIC_LIBS to use the static libs located in lib/VC/static

   if (OPENSSL_MSVC_STATIC_RT)
     set(_OPENSSL_MSVC_RT_MODE "MT")
   else ()
     set(_OPENSSL_MSVC_RT_MODE "MD")
   endif ()

   # Since OpenSSL 1.1, lib names are like libcrypto32MTd.lib and libssl32MTd.lib
   if( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )
       set(_OPENSSL_MSVC_ARCH_SUFFIX "64")
   else()
       set(_OPENSSL_MSVC_ARCH_SUFFIX "32")
   endif()

   if(OPENSSL_USE_STATIC_LIBS)
     set(_OPENSSL_PATH_SUFFIXES
       "lib/VC/static"
       "VC/static"
       "lib"
       )
   else()
     set(_OPENSSL_PATH_SUFFIXES
       "lib/VC"
       "VC"
       "lib"
       )
   endif ()

   find_library(LIB_EAY_DEBUG
     NAMES
       libcrypto${_OPENSSL_MSVC_ARCH_SUFFIX}${_OPENSSL_MSVC_RT_MODE}d
       libcryptod
       libeay32${_OPENSSL_MSVC_RT_MODE}d
       libeay32d
     NAMES_PER_DIR
     ${_OPENSSL_ROOT_HINTS_AND_PATHS}
     PATH_SUFFIXES
       ${_OPENSSL_PATH_SUFFIXES}
   )

   find_library(LIB_EAY_RELEASE
     NAMES
       libcrypto${_OPENSSL_MSVC_ARCH_SUFFIX}${_OPENSSL_MSVC_RT_MODE}
       libcrypto
       libeay32${_OPENSSL_MSVC_RT_MODE}
       libeay32
     NAMES_PER_DIR
     ${_OPENSSL_ROOT_HINTS_AND_PATHS}
     PATH_SUFFIXES
       ${_OPENSSL_PATH_SUFFIXES}
   )

   IF(MSVC_IDE)
      IF(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)
         SET(OPENSSLCRYPTO_LIBRARIES optimized ${LIB_EAY_RELEASE} debug ${LIB_EAY_DEBUG})
      ELSE(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)
         SET(OPENSSLCRYPTO_LIBRARIES NOTFOUND)
         MESSAGE(STATUS "Could not find the debug and release version of openssl")
      ENDIF(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)
   ELSE(MSVC_IDE)
      STRING(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
      IF(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
         SET(OPENSSLCRYPTO_LIBRARIES ${LIB_EAY_DEBUG})
      ELSE(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
         SET(OPENSSLCRYPTO_LIBRARIES ${LIB_EAY_RELEASE})
      ENDIF(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
   ENDIF(MSVC_IDE)
   MARK_AS_ADVANCED(LIB_EAY_DEBUG LIB_EAY_RELEASE)
ELSE(WIN32 AND MSVC)
    FIND_LIBRARY(OPENSSLCRYPTO_LIBRARIES
      NAMES
        crypto
        libeay32
      NAMES_PER_DIR
      ${_OPENSSL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
    )
ENDIF(WIN32 AND MSVC)

IF(OPENSSLCRYPTO_INCLUDE_DIR AND OPENSSLCRYPTO_LIBRARIES)
   SET(OPENSSLCRYPTO_FOUND TRUE)
ELSE(OPENSSLCRYPTO_INCLUDE_DIR AND OPENSSLCRYPTO_LIBRARIES)
   SET(OPENSSLCRYPTO_FOUND FALSE)
ENDIF (OPENSSLCRYPTO_INCLUDE_DIR AND OPENSSLCRYPTO_LIBRARIES)

IF (OPENSSLCRYPTO_FOUND)
   IF (NOT OpenSSLCRYPTO_FIND_QUIETLY)
      MESSAGE(STATUS "Found OpenSSL Crypto Library: ${OPENSSLCRYPTO_LIBRARIES}")
   ENDIF (NOT OpenSSLCRYPTO_FIND_QUIETLY)
ELSE (OPENSSLCRYPTO_FOUND)
   IF (OpenSSLCRYPTO_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find OpenSSL")
   ENDIF (OpenSSLCRYPTO_FIND_REQUIRED)
ENDIF (OPENSSLCRYPTO_FOUND)

MARK_AS_ADVANCED(OPENSSLCRYPTO_INCLUDE_DIR OPENSSLCRYPTO_LIBRARIES)

