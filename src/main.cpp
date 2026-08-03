#include <QApplication>
#include <QCommandLineParser>

#include "executable_file.hpp"
#include "main_window.hpp"
#include "process_manager.hpp"
#include "run_config.hpp"
#include "translator.hpp"
#include "tray_icon.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("kisel");
    QApplication::setApplicationVersion(APP_VERSION);

    kisel::TRANSLATOR->setLocaleFromSettings();

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("cli", "Efficient launch of Windows programs"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Path to the Windows executable file (.exe)");
    QCommandLineOption prefixOption({ "p", "prefix" }, QCoreApplication::translate("cli", "Run immediately in <PrefixName>"), "PrefixName", "");
    parser.addOption(prefixOption);
    parser.process(app);

    kisel::ProcessManager processManager;
    kisel::TrayIcon trayIcon(&processManager);

    const QStringList positionalArgs = parser.positionalArguments();
    if (positionalArgs.isEmpty()) {
        auto* mainWindow = new kisel::MainWindow(&processManager);
        mainWindow->show();
    } else if (parser.isSet(prefixOption)) {
        kisel::ExecutableFile exeFile(positionalArgs.first());
        kisel::RunConfig runConfig;
        runConfig.setPrefixName(parser.value(prefixOption));
        processManager.run(exeFile, &runConfig);
    } else {
        auto* mainWindow = new kisel::MainWindow(&processManager, positionalArgs.first());
        mainWindow->show();
    }

    return QApplication::exec();
}
