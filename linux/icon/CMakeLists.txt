cmake_minimum_required(VERSION 3.18)
set(BASE_DIR "hicolor")
set(RESOLUTIONS 16 22 32 48 256 512)
foreach(RESOLUTION ${RESOLUTIONS})
    set(OUTPUT_DIR "${BASE_DIR}/${RESOLUTION}x${RESOLUTION}/apps")
    set(OUTPUT_PNG "${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.png")
    add_custom_command(OUTPUT "${OUTPUT_PNG}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
        COMMAND inkscape -o "${OUTPUT_PNG}" -w ${RESOLUTION} -h ${RESOLUTION} "${CMAKE_CURRENT_SOURCE_DIR}/../../../../icon.svg"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/../../../../icon.svg"
    )
    add_custom_target("${RESOLUTION}-png" ALL DEPENDS "${OUTPUT_PNG}")
    list(APPEND png_targets "${RESOLUTION}-png")
    list(APPEND png_files "${OUTPUT_PNG}")
endforeach()
add_custom_target("app_icon" DEPENDS "app.ico")
add_custom_command(OUTPUT "app.ico"
    COMMAND convert ${png_files} "app.ico"
    DEPENDS ${png_targets})
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${BASE_DIR}" DESTINATION "share/icons")
