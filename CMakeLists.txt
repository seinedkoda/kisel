cmake_minimum_required(VERSION 3.19)
project(Kisel VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network LinguistTools)
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES en ru)

qt_add_executable(kisel
    src/main.cpp
    src/executable_file.hpp src/executable_file.cpp
    src/prefix.hpp src/prefix.cpp
    src/prefix_model.hpp src/prefix_model.cpp
    src/ct.hpp src/ct.cpp
    src/ct_model.hpp src/ct_model.cpp
    src/main_window.hpp src/main_window.cpp
    src/app_settings_window.hpp src/app_settings_window.cpp
    src/prefix_window.hpp src/prefix_window.cpp
    src/ct_window.hpp src/ct_window.cpp
    src/translator.hpp src/translator.cpp
    src/app_settings.hpp src/app_settings.cpp
    src/shortcut_dialog.hpp src/shortcut_dialog.cpp
    src/prefix_settings.hpp src/prefix_settings.cpp
    src/prefix_settings_dialog.hpp src/prefix_settings_dialog.cpp
    src/process_manager.hpp src/process_manager.cpp
    src/tray_icon.hpp src/tray_icon.cpp
    src/prefix_components_dialog.hpp src/prefix_components_dialog.cpp
)

qt_add_resources(kisel "resources"
    PREFIX "/"
    BASE "resources"
    FILES
        "resources/icons/kisel.png"
        "resources/icons/kisel.svg"
)

add_dependencies(kisel update_translations)

qt_add_translations(kisel
    TS_FILE_BASE kisel
    TS_OUTPUT_DIRECTORY "translations/"
    PLURALS_TS_FILE "translations/kisel_en.ts"
    LUPDATE_OPTIONS -no-obsolete
)

target_compile_definitions(kisel
    PUBLIC
        APP_AUTHOR="Seined Koda"
        APP_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(kisel
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

install(TARGETS kisel
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(LINUX)
    include(GNUInstallDirs)

    install(TARGETS kisel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(
        FILES "resources/icons/kisel.svg"
        DESTINATION "share/icons/hicolor/scalable/apps"
        RENAME "kisel.svg"
    )

    install(
        FILES "resources/kisel.desktop"
        DESTINATION "share/applications"
    )
endif()
