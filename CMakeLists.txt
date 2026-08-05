cmake_minimum_required(VERSION 3.19)
project(Kisel VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    src/run_config.hpp src/run_config.cpp
)

qt_add_resources(kisel "resources"
    PREFIX "/"
    BASE "resources"
    FILES
        "resources/icons/kisel.png"
        "resources/icons/kisel.svg"
        "resources/icons/thirdparty/Papirus/index.theme"
        "resources/icons/thirdparty/Papirus/64x64/mimetypes/unknown.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/media-playback-start.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/media-playback-stop.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/open-menu.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/configure.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/document-open.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/document-open-folder.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/browser-download.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/flag.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/link.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/tools.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/plugins.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/tool-wizard.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/view-refresh.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/view-list-text.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/list-add.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/list-remove.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/edit-clear.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/entry-delete.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/window-close.svg"
        "resources/icons/thirdparty/Papirus/16x16/apps/steam.svg"
        "resources/icons/thirdparty/Papirus/16x16/symbolic/apps/wine-symbolic.svg"
)

add_dependencies(kisel update_translations)

qt_add_translations(kisel
    TS_FILES
        translations/kisel_en.ts
        translations/kisel_ru.ts
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
