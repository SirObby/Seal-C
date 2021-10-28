include(GNUInstallDirs)
set(SEALC_EXPORT_NAME sealc)
set(SEALC_VERSIONED ${SEALC_EXPORT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
set(SEALC_VERSION_FILE ${PROJECT_BINARY_DIR}/${SEALC_EXPORT_NAME}-config-version.cmake)
set(SEALC_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${SEALC_VERSIONED})
set(SEALC_INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR}/${SEALC_VERSIONED})

## Pack the binary output
install(TARGETS sealc
        EXPORT ${SEALC_EXPORT_NAME}
        LIBRARY DESTINATION  ${SEALC_INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION  ${SEALC_INSTALL_LIBRARY_DIR}
        RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${SEALC_INSTALL_INCLUDE_DIR})

## Allow for a specific version to be chosen in the `find_package` command
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${SEALC_VERSION_FILE}
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

## Package the include headers (the trailing slash is important, otherwise
## the include folder will be copied, instead of it's contents)
install(DIRECTORY include/ DESTINATION ${SEALC_INSTALL_INCLUDE_DIR})

## Include the file which allows `find_package(libdpp)` to function.
install(FILES cmake/libdpp-config.cmake ${SEALC_VERSION_FILE} DESTINATION ${SEALC_INSTALL_LIBRARY_DIR})

## Export the targets to allow other projects to easily include this project
#install(EXPORT ${SEALC_EXPORT_NAME} DESTINATION ${SEALC_INSTALL_LIBRARY_DIR} NAMESPACE dpp::)

# Prepare information for packaging into .zip, .deb, .rpm
## Project installation metadata
set(CPACK_PACKAGE_NAME   libsealc)            # Name of generated file
set(CPACK_PACKAGE_VENDOR seals-are-bouncy)       # Maker of the application

## Select generated based on what operating system
if(WIN32)
    set(CPACK_GENERATOR ZIP)
elseif(UNIX)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_CONTACT "https://discord.gg/XEzUV5t3j9") # Seal.C Development Discord
#    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsodium23 (>= 1.0.17-1), libopus0 (>= 1.3-1)")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "An incredibly lightweight C Discord library")
endif()
