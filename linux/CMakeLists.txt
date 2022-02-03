cmake_minimum_required(VERSION 3.18)
include("../../../project.cmake")
project(${project_name} VERSION ${project_version})
enable_testing()

string(TOLOWER ${CMAKE_PROJECT_NAME} app_name)
set(CPACK_PACKAGE_VENDOR ${project_identifier})
set(CPACK_PACKAGE_FILE_NAME ${app_name})
set(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_BINARY_DIR}/icon/app.ico)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtk-3-0,libwebkit2gtk-4.0-37")
include(CPack)

find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
pkg_check_modules(WEBKIT2GTK REQUIRED webkit2gtk-4.0)
link_directories(${WEBKIT2GTK_LIBRARY_DIRS})
include_directories(${WEBKIT2GTK_INCLUDE_DIRS})

add_subdirectory("icon")

add_custom_target("desktop" DEPENDS "${CMAKE_PROJECT_NAME}.desktop")
add_custom_command(OUTPUT "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo [Desktop Entry] > "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo Name=${cross_target} >> "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo Exec=${CMAKE_PROJECT_NAME} >> "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo Icon=${CMAKE_PROJECT_NAME} >> "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo Type=Application >> "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo Categories=Utility >> "${CMAKE_PROJECT_NAME}.desktop"
    COMMAND ${CMAKE_COMMAND} -E echo Terminal=false >> "${CMAKE_PROJECT_NAME}.desktop"
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable("${app_name}"
    "main.cpp"
    "web_view.cpp"
)
add_dependencies("${app_name}" "desktop")
set_property(TARGET "${app_name}" PROPERTY CXX_STANDARD 17)
target_link_libraries("${app_name}"
    ${GTKMM_LIBRARIES} ${WEBKIT2GTK_LIBRARIES} pthread)
target_link_options("${app_name}" PRIVATE -no-pie)
target_compile_definitions("${app_name}" PRIVATE
    PROJECT_NAME="${app_name}"
    PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
    APPLICATION_ID="${CPACK_PACKAGE_VENDOR}"
    PROJECT_DOMAIN="${project_domain}"
    PROJECT_LANDING_PAGE="${project_landing_page}"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.desktop" DESTINATION "share/applications")
install(TARGETS "${app_name}" DESTINATION "bin")
install(DIRECTORY "../../assets" DESTINATION "share/${CMAKE_PROJECT_NAME}")

