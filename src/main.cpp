#include <QApplication>
#include <QCommandLineParser>

#include "main_window.hpp"
#include "run_manager.hpp"
#include "translator.hpp"
#include "tray_icon.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("kisel");
    QApplication::setApplicationVersion(APP_VERSION);

    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << ":/icons/thirdparty");
    QIcon::setFallbackThemeName("Papirus");

    kisel::TRANSLATOR->setLocaleFromSettings();

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("cli", "Efficient launch of Windows programs"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Path to the Windows executable file (.exe)");
    QCommandLineOption prefixOption({ "p", "prefix" }, QCoreApplication::translate("cli", "Run immediately in <PrefixName>"), "PrefixName", "");
    parser.addOption(prefixOption);
    parser.process(app);

    kisel::TrayIcon trayIcon(kisel::RUN_MANAGER);

    const QStringList positionalArgs = parser.positionalArguments();
    if (positionalArgs.isEmpty()) {
        auto* mainWindow = new kisel::MainWindow();
        mainWindow->show();
    } else if (parser.isSet(prefixOption)) {
        kisel::RunConfig runConfig;
        runConfig.setExecutablePath(positionalArgs.first());
        runConfig.setPrefixName(parser.value(prefixOption));
        kisel::RUN_MANAGER->run(&runConfig);
    } else {
        auto* mainWindow = new kisel::MainWindow(positionalArgs.first());
        mainWindow->show();
    }

    return QApplication::exec();
}
