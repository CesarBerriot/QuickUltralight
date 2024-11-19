# setup assets folder, if required
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/assets)
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/assets)
    file(DOWNLOAD https://raw.githubusercontent.com/CesarBerriot/QuickUltralight/refs/heads/master/app.html ${CMAKE_SOURCE_DIR}/assets/app.html)
endif()

# download QuickUltralight
file(DOWNLOAD https://raw.githubusercontent.com/CesarBerriot/QuickUltralight/refs/heads/master/quick_ultralight.h ${CMAKE_BINARY_DIR}/QuickUltralight/api/quick_ultralight.h)
file(DOWNLOAD https://raw.githubusercontent.com/CesarBerriot/QuickUltralight/refs/heads/master/quick_ultralight.c ${CMAKE_BINARY_DIR}/QuickUltralight/api/quick_ultralight.c)

# download Ultralight
file(DOWNLOAD https://github.com/ultralight-ux/Ultralight/releases/download/v1.4.0-beta/ultralight-sdk-1.4.0b-win-x64.7z ${CMAKE_BINARY_DIR}/Ultralight.7z)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/ultralight.7z DESTINATION ${CMAKE_BINARY_DIR}/Ultralight)
file(REMOVE ${CMAKE_BINARY_DIR}/Ultralight.7z)

# target setup macro
macro(QuickUltralightSetupTarget TARGET)
    # include QuickUltralight API
    target_include_directories(${TARGET} PRIVATE ${CMAKE_BINARY_DIR}/QuickUltralight/api)
    target_sources(${TARGET} PRIVATE ${CMAKE_BINARY_DIR}/QuickUltralight/api/quick_ultralight.c)

    # include & link Ultralight
    target_include_directories(${TARGET} PRIVATE "${CMAKE_BINARY_DIR}/Ultralight/include")
    target_link_directories(${TARGET} PRIVATE "${CMAKE_BINARY_DIR}/Ultralight/lib")
    target_link_libraries(${TARGET} PRIVATE UltralightCore AppCore Ultralight WebCore)

    # do the required file operations post-build for ultralight to work
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets")
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/Ultralight/bin" "${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/Ultralight/resources" "${CMAKE_CURRENT_BINARY_DIR}/assets/resources")
endmacro()
