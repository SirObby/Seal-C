cmake_minimum_required (VERSION 3.12)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(SEALC_BUILD_TEST "Build the test program" ON)

add_compile_definitions(SEAL_BUILD)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

# Dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/cwire)

if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message("-- Building ${Green}git${ColourReset} version. ${Green}Be aware git versions may be unstable!${ColourReset}")
else()
	message("-- Building ${Green}stable${ColourReset} version ${Green}${SEALC_VERSION}${ColourReset}")
endif()

add_library(sealc
	${CMAKE_CURRENT_SOURCE_DIR}/src/sealc/cluster.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/sealc/sealc.c
)

set_target_properties(sealc PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
)

target_include_directories(sealc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${UV_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
)

target_link_libraries(sealc PUBLIC
    # Event Loop
    ${UV_LIBRARIES} 
    cwire
    # Math
    m
    ${ZLIB_LIBRARIES}
)

target_compile_options(sealc PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>
)

if (SEALC_BUILD_TEST)
	add_executable(test 
		${CMAKE_CURRENT_SOURCE_DIR}/src/test.c
	)
	set_target_properties(test PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
    )
	target_link_libraries(test PUBLIC sealc)
endif()

if(UNIX OR MSYS)
	find_program(LDCONFIG_EXECUTABLE "ldconfig")
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