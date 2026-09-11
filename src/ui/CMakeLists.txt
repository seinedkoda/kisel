target_sources(kisel PRIVATE
    mainwindow/main_window.hpp mainwindow/main_window.cpp
    appsettings/app_settings_window.hpp appsettings/app_settings_window.cpp
    aboutapp/about_app_dialog.hpp aboutapp/about_app_dialog.cpp
    prefix/new_prefix_dialog.hpp prefix/new_prefix_dialog.cpp
    prefix/prefix_list_widget.hpp prefix/prefix_list_widget.cpp
    prefix/prefix_page.hpp prefix/prefix_page.cpp
    prefix/prefix_settings_dialog.hpp prefix/prefix_settings_dialog.cpp
    prefix/prefix_components_dialog.hpp prefix/prefix_components_dialog.cpp
    compatibilitytools/ct_list_widget.hpp compatibilitytools/ct_list_widget.cpp
    compatibilitytools/ct_page.hpp compatibilitytools/ct_page.cpp
    compatibilitytools/new_ct_dialog.hpp compatibilitytools/new_ct_dialog.cpp
    shortcuts/shortcuts_dialog.hpp shortcuts/shortcuts_dialog.cpp
    shortcuts/shortcuts_list_widget.hpp shortcuts/shortcuts_list_widget.cpp
    trayicon/tray_icon.hpp trayicon/tray_icon.cpp
)
