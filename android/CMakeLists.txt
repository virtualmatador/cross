cmake_minimum_required(VERSION 3.18)
include("../../../project.cmake")
project(${project_name} LANGUAGES NONE VERSION ${project_version})
enable_testing()


file(GLOB bundle_tool "/usr/local/dev/android-sdk/bundletool-all-*")
list(SORT bundle_tool)
list(GET bundle_tool -1 bundle_tool)

file(GLOB sdk_tools_dir "/usr/local/dev/android-sdk/build-tools/*")
list(SORT sdk_tools_dir)
list(GET sdk_tools_dir -1 sdk_tools_dir)

file(GLOB sdk_platform_dir "/usr/local/dev/android-sdk/platforms/*")
list(SORT sdk_platform_dir)
list(GET sdk_platform_dir -1 sdk_platform_dir)

file(GLOB ndk_tools_dir "/usr/local/dev/android-sdk/ndk/*")
list(SORT ndk_tools_dir)
list(GET ndk_tools_dir -1 ndk_tools_dir)

string(REGEX MATCH "[0123456789]+$" sdk_version ${sdk_platform_dir})

file(GLOB_RECURSE asset_list "../../../html/*")
list(SORT asset_list)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(ndk_debug 1)
    set(jdk_debug "true")
else()
    set(ndk_debug 0)
    set(jdk_debug "false")
endif()

list(APPEND icon_dirs mipmap-mdpi mipmap-hdpi mipmap-xhdpi mipmap-xxhdpi mipmap-xxxhdpi)
foreach(icon_dir ${icon_dirs})
    list(APPEND resources_list "${CMAKE_CURRENT_BINARY_DIR}/res/${icon_dir}/ic_launcher.png")
    if(icon_dir STREQUAL mipmap-mdpi)
        set(RESOLUTION 48)
    elseif(icon_dir STREQUAL mipmap-hdpi)
        set(RESOLUTION 72)
    elseif(icon_dir STREQUAL mipmap-xhdpi)
        set(RESOLUTION 96)
    elseif(icon_dir STREQUAL mipmap-xxhdpi)
        set(RESOLUTION 144)
    elseif(icon_dir STREQUAL mipmap-xxxhdpi)
        set(RESOLUTION 192)
    endif()
    add_custom_command(OUTPUT "res/${icon_dir}/ic_launcher.png"
        DEPENDS "../../icon.svg"
        COMMAND ${CMAKE_COMMAND} -E make_directory "res/${icon_dir}"
        COMMAND inkscape -o "res/${icon_dir}/ic_launcher.png" -w ${RESOLUTION} -h ${RESOLUTION} "${CMAKE_CURRENT_SOURCE_DIR}/../../icon.svg"
    )
endforeach()
list(APPEND resources_list "${CMAKE_CURRENT_BINARY_DIR}/res/layout/main.xml")

add_custom_target(app_png ALL DEPENDS "app.png")
add_custom_command(OUTPUT "app.png"
    DEPENDS "../../icon.svg"
    COMMAND inkscape -o "app.png" -w 512 -h 512 "${CMAKE_CURRENT_SOURCE_DIR}/../../icon.svg"
)

add_custom_target("${project_identifier}.aab" ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${project_identifier}.aab")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${project_identifier}.aab"
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/res-linked.apk
        ${CMAKE_CURRENT_BINARY_DIR}/dex/classes.dex
    COMMAND ${CMAKE_COMMAND} -E make_directory "obj/manifest"
    COMMAND cd obj && jar xf "../res-linked.apk" resources.pb AndroidManifest.xml res
    COMMAND ${CMAKE_COMMAND} -E rename "obj/AndroidManifest.xml" "obj/manifest/AndroidManifest.xml"
    COMMAND cd obj && jar cMf ../base.zip ../dex ../lib "manifest" "res" "resources.pb" "html"
    COMMAND ${CMAKE_COMMAND} -E remove "${project_identifier}.aab"
    COMMAND java -jar "${bundle_tool}" build-bundle --modules=base.zip --output="${project_identifier}.aab"
)
add_custom_command(OUTPUT "res-linked.apk" "java/com/shaidin/cross/R.java"
    DEPENDS
        ${resources_list}
        "${CMAKE_CURRENT_BINARY_DIR}/resources.txt"
        "${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml"
    COMMAND "${sdk_tools_dir}/aapt2" compile --dir "res" -o "res.zip"
    COMMAND ${CMAKE_COMMAND} -E make_directory "java"
    COMMAND "${sdk_tools_dir}/aapt2" link --proto-format -o "res-linked.apk" -I "${sdk_platform_dir}/android.jar" --java "java/" --custom-package "com.shaidin.cross" --manifest "AndroidManifest.xml" "res.zip" --auto-add-overlay
)
add_custom_command(OUTPUT "dex/classes.dex"
    DEPENDS
        "src/java/com/shaidin/cross/MainActivity.java"
        "${CMAKE_CURRENT_BINARY_DIR}/java/com/shaidin/cross/R.java"
    COMMAND ${CMAKE_COMMAND} -E make_directory "java-o"
    COMMAND javac --release 8 -encoding utf8 -Xlint:deprecation -d "java-o" -classpath "${sdk_platform_dir}/android.jar" "${CMAKE_CURRENT_SOURCE_DIR}/src/java/com/shaidin/cross/MainActivity.java" "java/com/shaidin/cross/R.java"
    COMMAND ${CMAKE_COMMAND} -E make_directory "dex"
    COMMAND "${sdk_tools_dir}/d8" --output "dex" --classpath "java-o" --lib "${sdk_platform_dir}/android.jar" "java-o/com/shaidin/cross/*.class"
)
list(APPEND xml_commands
    -u /manifest/@package -v ${project_identifier}
	-u /manifest/@android:versionCode -v ${CMAKE_PROJECT_VERSION_PATCH}
	-u /manifest/@android:versionName -v ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}
	-u /manifest/application/@android:label -v ${project_name}
    -u /manifest/application/@android:debuggable -v ${jdk_debug}
    -u /manifest/uses-sdk/@android:targetSdkVersion -v ${sdk_version}
)
add_custom_command(OUTPUT "AndroidManifest.xml"
    DEPENDS
        "AndroidManifest.xml.in"
        "../../manifest.txt"
        COMMAND xmlstarlet edit ${xml_commands} "${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml.in" > "AndroidManifest.xml"
)
configure_file("res/layout/main.xml" "res/layout/main.xml" COPYONLY)
