target_sources(kisel PRIVATE
    appsettings/app_settings.hpp appsettings/app_settings.cpp
    executablefile/executable_file.hpp executablefile/executable_file.cpp
    prefix/prefix.hpp prefix/prefix.cpp
    prefix/prefix_settings.hpp prefix/prefix_settings.cpp
    prefix/prefix_model.hpp prefix/prefix_model.cpp
    compatibilitytools/ct.hpp compatibilitytools/ct.cpp
    compatibilitytools/ct_model.hpp compatibilitytools/ct_model.cpp
    compatibilitytools/ct_installer.hpp compatibilitytools/ct_installer.cpp
    run/run_config.hpp run/run_config.cpp
    run/run_manager.hpp run/run_manager.cpp
    shortcuts/shortcuts_utils.hpp shortcuts/shortcuts_utils.cpp
)
