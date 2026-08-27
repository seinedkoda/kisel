cmake_minimum_required(VERSION 3.19)
project(Kisel VERSION 1.3.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(INSTALL_UMU "Download and install umu from source" ON)
set(UMU_VERSION "1.4.4")

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
    src/run_manager.hpp src/run_manager.cpp
    src/tray_icon.hpp src/tray_icon.cpp
    src/prefix_components_dialog.hpp src/prefix_components_dialog.cpp
    src/run_config.hpp src/run_config.cpp
    src/about_dialog.hpp src/about_dialog.cpp
    src/ct_installer.hpp src/ct_installer.cpp
)

qt_add_resources(kisel "resources"
    PREFIX "/"
    BASE "resources"
    FILES
        "resources/icons/kisel.svg"
        "resources/icons/kisel-256x256.png"
        "resources/icons/thirdparty/Papirus/index.theme"
        "resources/icons/thirdparty/Papirus/64x64/mimetypes/unknown.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/media-playback-start.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/media-playback-stop.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/open-menu.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/configure.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/document-open.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/document-open-folder.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/document-save.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/browser-download.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/flag.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/link.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/help-about.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/tools.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/plugins.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/tool-wizard.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/view-refresh.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/view-list.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/view-list-text.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/list-add.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/edit-clear.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/entry-delete.svg"
        "resources/icons/thirdparty/Papirus/16x16/actions/window-close.svg"
        "resources/icons/thirdparty/Papirus/16x16/mimetypes/text-x-log.svg"
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

include(GNUInstallDirs)

if (INSTALL_UMU)
    include(FetchContent)

    FetchContent_Declare(
        umu_launcher_zipapp
        URL "https://github.com/Open-Wine-Components/umu-launcher/releases/download/${UMU_VERSION}/umu-launcher-${UMU_VERSION}-zipapp.tar"
        URL_HASH SHA256=eb590691841f7fad3fc3ad8fd5db4ccb87849fe7948e62b28ece7a4ee48cc851
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(umu_launcher_zipapp)

    install(
        PROGRAMS "${umu_launcher_zipapp_SOURCE_DIR}/umu-run"
        DESTINATION libexec/kisel
    )
endif()

install(TARGETS kisel
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES ${CMAKE_SOURCE_DIR}/resources/icons/kisel.svg
    DESTINATION share/icons/hicolor/scalable/apps
    RENAME io.github.seinedkoda.kisel.svg
)

install(
    FILES ${CMAKE_SOURCE_DIR}/resources/kisel.desktop
    DESTINATION share/applications
    RENAME io.github.seinedkoda.kisel.desktop
)

install(
    FILES ${CMAKE_SOURCE_DIR}/resources/kisel.metainfo.xml
    RENAME io.github.seinedkoda.kisel.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
)

set(CPACK_PACKAGE_NAME "kisel")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Efficient launch of Windows programs")
set(CPACK_PACKAGE_CONTACT "Seined Koda <seinedk@gmail.com>")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/seinedkoda/kisel")
set(CPACK_PACKAGE_LICENSE "GPLv3")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

# .rpm (cpack -G RPM)
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, icoutils, python3")
set(CPACK_RPM_PACKAGE_RECOMMENDS "mangohud")
set(CPACK_RPM_PACKAGE_SUGGESTS "winetricks, obs-studio-plugin-vkcapture")

# .deb (cpack -G DEB)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, icoutils, python3")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "mangohud")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "winetricks, obs-vkcapture")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

add_custom_target(flatpak-package
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND flatpak-builder --repo=repo --force-clean
        flatpak-build "${CMAKE_SOURCE_DIR}/misc/flatpak/io.github.seinedkoda.kisel.yml"
    COMMAND flatpak build-bundle repo 
        "${CMAKE_BINARY_DIR}/packages/kisel-${PROJECT_VERSION}-x86_64.flatpak"
        io.github.seinedkoda.kisel
)

include(CPack)
