#include "command_line_utils.hpp"

#include <QCommandLineParser>

#include "core/app/app.hpp"
#include "core/prefix/prefix.hpp"

using namespace kisel;

void kisel::parseCommandLine(const QStringList& args, RunConfig* config)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("cli", "Efficient launch of Windows programs"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption prefixOption(
        { "p", "prefix" },
        QCoreApplication::translate("cli", "Run immediately in <PrefixName>"),
        "PrefixName", "");
    parser.addOption(prefixOption);

    parser.addPositionalArgument("file", "Path to the Windows executable file (.exe)");

    parser.process(args);

    if (parser.isSet(prefixOption)) {
        QString value = parser.value(prefixOption);
        if (QFileInfo(value).isAbsolute()) {
            Prefix* prefix = PREFIX_MODEL->getByPath(value);
            if (prefix == nullptr) {
                prefix = new Prefix(value, config);
            }
            config->setPrefix(prefix);
        } else {
            config->setPrefix(PREFIX_MODEL->getByName(value));
        }
    }

    const QStringList positionalArgs = parser.positionalArguments();
    if (!positionalArgs.isEmpty()) {
        config->setExecutablePath(positionalArgs.first());
    }
}