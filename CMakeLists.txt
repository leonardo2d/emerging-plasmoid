project(emerging-plasmoid)
set(VERSION "1.0.3")

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
)
 
# We add our source code here
set(emerging_SRCS emergingplasmoid.cpp)

# Add ui things
kde4_add_ui_files(emerging_SRCS config.ui) 
 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_emerging ${emerging_SRCS})

target_link_libraries(plasma_applet_emerging 
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_emerging
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-emerging.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES getcurrent glogo-small.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/apps/emerging-plasmoid/)

# CPack related stuff
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_GENERATOR TGZ)
		
set(CPACK_PACKAGE_NAME "emerging-plasmoid")
set(CPACK_PACKAGE_VENDOR "Leonardo Guilherme de Freitas")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "emerging-plasmoid")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Emerging Plasmoid")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_SOURCE_IGNORE_FILES "\\\\.git.*;${CMAKE_BINARY_DIR}/.*;.*~;\\\\.kdev4/.*;.*\\\\.kdev4;.*\\\\.swp")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
		
include(CPack)
