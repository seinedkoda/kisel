target_sources(kisel PRIVATE
    app/app.hpp app/app.cpp
    settings/base_settings.hpp settings/base_settings.cpp
    settings/app_settings.hpp settings/app_settings.cpp
    settings/prefix_settings.hpp settings/prefix_settings.cpp
    commandline/command_line_utils.hpp commandline/command_line_utils.cpp
    logging/logging.hpp logging/logging.cpp
    executablefile/executable_file.hpp executablefile/executable_file.cpp
    prefix/prefix.hpp prefix/prefix.cpp
    prefix/prefix_model.hpp prefix/prefix_model.cpp
    compatibilitytools/ct.hpp compatibilitytools/ct.cpp
    compatibilitytools/ct_model.hpp compatibilitytools/ct_model.cpp
    compatibilitytools/ct_installer.hpp compatibilitytools/ct_installer.cpp
    run/run_config.hpp run/run_config.cpp
    run/run_manager.hpp run/run_manager.cpp
    shortcuts/shortcut.hpp shortcuts/shortcut.cpp
    shortcuts/shortcut_model.hpp shortcuts/shortcut_model.cpp
)
