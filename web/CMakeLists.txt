cmake_minimum_required(VERSION 3.18)
include("../../../project.cmake")
project(${project_name} LANGUAGES NONE VERSION ${project_version})
enable_testing()
set(html_dir "${CMAKE_CURRENT_BINARY_DIR}/www/html")
set_property(DIRECTORY APPEND PROPERTY
    CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/../../../html"
)
file(GLOB assets RELATIVE "${CMAKE_CURRENT_LIST_DIR}/../../../html/"
    "${CMAKE_CURRENT_LIST_DIR}/../../../html/*"
)
file(WRITE "${html_dir}/manifest.appcache" "CACHE MANIFEST\nCACHE:\n")
foreach(asset ${assets})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/../../../html/${asset}"
        "${html_dir}/${asset}"
    )
    file(APPEND "${html_dir}/manifest.appcache" "/${asset}\n")
endforeach()
list(APPEND assets "manifest.appcache")
configure_file("${CMAKE_CURRENT_LIST_DIR}/../../../icon.svg"
    "${html_dir}/icon.svg" COPYONLY
)
list(APPEND assets "icon.svg")
file(GLOB extra_assets RELATIVE "${html_dir}" "${html_dir}/*")
list(REMOVE_ITEM extra_assets ${assets})
foreach(asset ${extra_assets})
    file(REMOVE "${html_dir}/${asset}")
endforeach()
