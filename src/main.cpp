#include <QApplication>

#include "core/app/app.hpp"
#include "core/settings/app_settings.hpp"
#include "core/commandline/command_line_utils.hpp"
#include "core/logging/logging.hpp"
#include "core/run/run_config.hpp"
#include "ui/mainwindow/main_window.hpp"
#include "ui/trayicon/tray_icon.hpp"

kisel::Prefix* instantRunPrefix(const kisel::RunConfig& runConfig)
{
    // 1. Check the individual prefix
    kisel::Prefix* individualPrefix = kisel::PREFIX_MODEL->getByName(runConfig.exeFile()->id());
    if (individualPrefix != nullptr) {
        return individualPrefix;
    }

    // 2. Check the portable prefix
    QString portablePrefixPath = runConfig.exeFile()->dir().absoluteFilePath(kisel::AppSettings::portablePrefixName());
    if (QFileInfo::exists(portablePrefixPath)) {
        return new kisel::Prefix(portablePrefixPath, kisel::PREFIX_MODEL);
    }

    // 3. If the executable file is inside the prefix, then prefer it
    const QString& exePath = runConfig.exePath();
    for (auto* prefix : kisel::PREFIX_MODEL->list()) {
        if (exePath.startsWith(prefix->path())) {
            return prefix;
        }
    }

    // 4. Default prefix from settings
    switch (kisel::APP_SETTINGS->prefixType()) {
    case kisel::AppSettings::PrefixType::Individual:
        return new kisel::Prefix(runConfig.exeFile()->id(), kisel::PREFIX_MODEL);
    case kisel::AppSettings::PrefixType::Portable:
        return new kisel::Prefix(portablePrefixPath, kisel::PREFIX_MODEL);
    default:
        return kisel::PREFIX_MODEL->defaultPrefix();
    }
}

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
        runConfig.setPrefix(instantRunPrefix(runConfig));
        kisel::RUN_MANAGER->runExe(&runConfig);
    } else {
        auto* mainWindow = new kisel::MainWindow(&runConfig);
        mainWindow->show();
    }

    return QApplication::exec();
}
