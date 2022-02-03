cmake_minimum_required(VERSION 3.18)
include("../../../project.cmake")
project(${project_name} LANGUAGES Swift VERSION ${project_version})
enable_testing()

set(CMAKE_Swift_LANGUAGE_VERSION 5.4)
set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 15)
set(CMAKE_MACOSX_BUNDLE ON)

add_executable(${CMAKE_PROJECT_NAME}
    src/CrossUIApp.swift
    src/CrossUIView.swift
    src/WebView.swift
)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${CROSS_TEAM_ID}
    XCODE_ATTRIBUTE_ENABLE_BITCODE OFF
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${project_identifier}
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION ${CMAKE_PROJECT_VERSION_PATCH}
    XCODE_ATTRIBUTE_MARKETING_VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/info.plist"
    XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
    XCODE_ATTRIBUTE_SKIP_INSTALL OFF
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Assets.xcassets")
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/Assets.xcassets" PROPERTIES
    GENERATED true
    MACOSX_PACKAGE_LOCATION Resources
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../html")
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/../../html" PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    "-framework UIKit"
)

list(APPEND resolutions
    "20 1 20"
    "20 2 40"
    "20 3 60"
    "29 1 29"
    "29 2 58"
    "29 3 87"
    "40 1 40"
    "40 2 80"
    "40 3 120"
    "60 2 120"
    "60 3 180"
    "76 1 76"
    "76 2 152"
    "83.5 2 167"
    "1024 1 1024"
)
foreach(resolution ${resolutions})
    string(REPLACE " " ";" resolution ${resolution})
    list(GET resolution 0 icon_size)
    list(GET resolution 1 icon_factor)
    list(GET resolution 2 icon_resolution)
    set(icon_name "icon-${icon_size}@${icon_factor}.png")
    set(icon_path "${CMAKE_CURRENT_BINARY_DIR}/Assets.xcassets/AppIcon.appiconset")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${icon_path}/${icon_size}x${icon_factor}"
        COMMAND qlmanage -t -s ${icon_resolution} -o "${icon_path}/${icon_size}x${icon_factor}" "${CMAKE_CURRENT_SOURCE_DIR}/../../icon.svg"
        COMMAND ${CMAKE_COMMAND} -E rename "${icon_path}/${icon_size}x${icon_factor}/icon.svg.png" "${icon_path}/${icon_name}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${icon_path}/${icon_size}x${icon_factor}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/../../icon.svg"
        BYPRODUCTS "${icon_path}/${icon_name}"
    )
endforeach(resolution in resolutions)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
    COMMAND security unlock-keychain -p \$\(<~/password.txt\) login.keychain-db
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../html" "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/html"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Assets.xcassets" "${CMAKE_CURRENT_BINARY_DIR}/Assets.xcassets"
)

add_custom_target("ipa" ALL
    COMMAND xcodebuild -project "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcodeproj" -scheme "${CMAKE_PROJECT_NAME}" -configuration ${CMAKE_BUILD_TYPE} -archivePath "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcarchive" -destination 'generic/platform=iOS' archive
    COMMAND xcodebuild -exportArchive -archivePath "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcarchive" -exportPath "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp" -exportOptionsPlist "${CMAKE_CURRENT_SOURCE_DIR}/ipa.plist" -allowProvisioningUpdates

    COMMAND ${CMAKE_COMMAND} -E echo '<?xml version="1.0" encoding="UTF-8"?>' > "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '<package xmlns="http://apple.com/itunes/importer" version="software5.4">' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '  <software_assets apple_id="${CROSS_PROJECT_APPLE_ID}" app_platform="ios">' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '    <asset type="bundle">' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '      <data_file>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '        <size>'\$\( stat -f %z "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/${CMAKE_PROJECT_NAME}.ipa" \)'</size>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '        <file_name>${CMAKE_PROJECT_NAME}.ipa</file_name>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '        <checksum type="md5">'\$\( md5 -q "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/${CMAKE_PROJECT_NAME}.ipa" \)'</checksum>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '      </data_file>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '    </asset>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '  </software_assets>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"
    COMMAND ${CMAKE_COMMAND} -E echo '</package>' >> "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/metadata.xml"

    COMMAND ${CMAKE_COMMAND} -E echo "${CROSS_DEVELOPER_APPLE_ID}" > "${CMAKE_CURRENT_BINARY_DIR}/archive.itmsp/apple-id"
)
