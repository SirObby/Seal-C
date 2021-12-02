cmake_minimum_required (VERSION 3.12)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(SEALC_BUILD_TEST "Build the test program" ON)

add_compile_definitions(SEAL_BUILD)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/sealc/version.h" version_h)

if(NOT version_h MATCHES "SEALC_VERSION_SHORT ([0-9][0-9])([0-9][0-9])([0-9][0-9])")
	message(FATAL_ERROR "Cannot get SEALC_VERSION_SHORT from version.h")
endif()

math(EXPR SEALC_VERSION_MAJOR ${CMAKE_MATCH_1})
math(EXPR SEALC_VERSION_MINOR ${CMAKE_MATCH_2})
math(EXPR SEALC_VERSION_PATCH ${CMAKE_MATCH_3})

string(CONCAT SEALC_VERSION ${SEALC_VERSION_MAJOR}.${SEALC_VERSION_MINOR}.${SEALC_VERSION_PATCH})

project(libsealc VERSION ${SEALC_VERSION} LANGUAGES C)

if(NOT BUILD_SHARED_LIBS)
	if(UNIX)
		message("-- Building static library.")
		
		if(UNIX AND NOT APPLE)
			set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
		endif()
		
		set(OPENSSL_USE_STATIC_LIBS ON)
#		set(sodium_USE_STATIC_LIBS ON)  We do not yet support voice.
#		set(OPUS_USE_STATIC_LIBS TRUE) We do not yet support voice.
	else()
		message(WARNING "-- Building of static library not supported on non UNIX systems.")
	endif()
endif()

include("cmake/colour.cmake")

#if (MINGW OR NOT WIN32)
#	include("cmake/FindSodium.cmake")
#	include("cmake/FindOpus.cmake")
#endif()

aux_source_directory("src" coresrc)

string(ASCII 27 Esc)

set(THREADS_PREFER_PTHREAD_FLAG OFF)

find_package(Threads REQUIRED)
if(MINGW OR NOT WIN32)
	find_package(ZLIB REQUIRED)
endif(MINGW OR NOT WIN32)

if(APPLE)
	set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")

	find_package(OpenSSL REQUIRED)
else()
	if(MINGW OR NOT WIN32)
		find_package(OpenSSL REQUIRED)
	endif()
endif()

include_directories(${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})

include_directories("include")

find_package(Git QUIET)

if(NOT GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message(FATAL_ERROR "You are using a git version of SEALC but do not have git installed. Install git (not 'gh') and try again.")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message("-- Building ${Green}git${ColourReset} version. ${Green}Be aware git versions may be unstable!${ColourReset}")
else()
	message("-- Building ${Green}stable${ColourReset} version ${Green}${SEALC_VERSION}${ColourReset}")
endif()

if(UNIX OR MSYS)
	find_program(LDCONFIG_EXECUTABLE "ldconfig")
endif()

set(modules_dir "src")

file(GLOB subdirlist ${modules_dir}/sealc)

foreach (fullmodname ${subdirlist})
    get_filename_component(modname ${fullmodname} NAME)
    set (modsrc "")
	
	file(GLOB modsrc "${modules_dir}/sealc/*.c" "${modules_dir}/sealc/events/*.c" "${modules_dir}/sealc/cluster/*.c")
	
	if(BUILD_SHARED_LIBS)
        add_library(${modname} SHARED ${modsrc})
	else()
		add_library(${modname} STATIC ${modsrc})
	endif()

	if (WIN32 AND NOT MINGW)
		if (NOT WINDOWS_32_BIT)
			target_link_libraries(${modname} PUBLIC "${PROJECT_SOURCE_DIR}/win32/lib/libssl.lib"
				"${PROJECT_SOURCE_DIR}/win32/lib/libcrypto.lib"
				"${PROJECT_SOURCE_DIR}/win32/lib/libsodium.lib"
				"${PROJECT_SOURCE_DIR}/win32/lib/opus.lib"
				"${PROJECT_SOURCE_DIR}/win32/lib/zlib.lib")
		else()
			target_link_libraries(${modname} PUBLIC "${PROJECT_SOURCE_DIR}/win32/32/lib/libssl.lib"
				"${PROJECT_SOURCE_DIR}/win32/32/lib/libcrypto.lib"
				"${PROJECT_SOURCE_DIR}/win32/32/lib/libsodium.lib"
				"${PROJECT_SOURCE_DIR}/win32/32/lib/opus.lib"
				"${PROJECT_SOURCE_DIR}/win32/32/lib/zlib.lib")
		endif()

	else()
		target_link_libraries(${modname} PUBLIC ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${ZLIB_LIBRARIES})
		if (MINGW)
			target_link_libraries(${modname} PUBLIC wsock32 ws2_32 crypt32)
		endif ()
	endif()

	if (HAVE_VOICE)
		target_link_libraries(${modname} PUBLIC ${sodium_LIBRARY_RELEASE} ${OPUS_LIBRARIES})
		
		include_directories(${OPUS_INCLUDE_DIRS} ${sodium_INCLUDE_DIR})
	endif()
endforeach()

#target_compile_features(sealc PRIVATE cxx_std_17)

set_property(TARGET sealc PROPERTY C_STANDARD 11)

if (SEALC_BUILD_TEST)
	add_executable(test ${coresrc})
	set_property(TARGET test PROPERTY C_STANDARD 11)
#	target_compile_features(test PRIVATE cxx_std_17)
	target_link_libraries(test PUBLIC ${modname})
endif()

if(NOT WIN32)
	# Installation

	include(GNUInstallDirs)
	install(TARGETS sealc LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -DLDCONFIG_EXECUTABLE=${LDCONFIG_EXECUTABLE} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PostInstall.cmake)")

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sealc.pc.in" "${CMAKE_BINARY_DIR}/sealc.pc" @ONLY)
	install(FILES "${CMAKE_BINARY_DIR}/sealc.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
elseif(MINGW)
	install(TARGETS sealc LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
endif()

include("cmake/CPackSetup.cmake")						# Setup information for packaging and distribution

# CPack initialization for distribution
include(CPack)