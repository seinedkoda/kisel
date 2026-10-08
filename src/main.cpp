#include <QApplication>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "core/commandline/command_line_utils.hpp"
#include "core/logging/logging.hpp"
#include "core/run/run_config.hpp"
#include "ui/mainwindow/main_window.hpp"
#include "ui/trayicon/tray_icon.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("kisel");
    QApplication::setApplicationVersion(APP_VERSION);

    kisel::APP_SETTINGS->createAppDirectories();

    if (kisel::APP_SETTINGS->loggingEnabled()) {
        kisel::setupLogging();
    }

    kisel::APP_SETTINGS->installLocale();
    kisel::APP_SETTINGS->applyCurrentStyle();

    kisel::RunConfig runConfig;
    kisel::parseCommandLine(QApplication::arguments(), &runConfig);
    bool exeIsValid = runConfig.exeIsValid();

    kisel::TrayIcon trayIcon(kisel::RUN_MANAGER);

    if (exeIsValid && runConfig.prefixIsValid()) {
        kisel::RUN_MANAGER->runExe(&runConfig);
    } else if (exeIsValid && kisel::APP_SETTINGS->instantRunEnabled()) {
        runConfig.setPrefix(kisel::PREFIX_MODEL->defaultPrefix());
        kisel::RUN_MANAGER->runExe(&runConfig);
    } else {
        auto* mainWindow = new kisel::MainWindow(&runConfig);
        mainWindow->show();
    }

    return QApplication::exec();
}
