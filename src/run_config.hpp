#pragma once

#include <QObject>
#include <QPointer>
#include <QProcessEnvironment>

#include "ct.hpp"
#include "executable_file.hpp"
#include "prefix.hpp"

namespace kisel {
class RunConfig : public QObject {
    Q_OBJECT

public:
    explicit RunConfig(QObject* parent = nullptr);

    void setExecutablePath(const QString& exePath);
    [[nodiscard]] ExecutableFile* exeFile() const;
    [[nodiscard]] QString exePath() const;
    [[nodiscard]] QString exeName() const;
    [[nodiscard]] const QIcon& exeIcon() const;
    void setPrefix(Prefix* prefix);
    void setPrefixName(const QString& prefixName);
    [[nodiscard]] Prefix* prefix() const;
    void setCt(Ct* ct);
    [[nodiscard]] Ct* ct() const;
    QProcessEnvironment& setNewEnv();
    QProcessEnvironment& env();
    [[nodiscard]] bool isUsingSteam() const;

private:
    ExecutableFile* m_exeFile;
    QPointer<Prefix> m_prefix;
    QPointer<Ct> m_ct;
    QProcessEnvironment m_env;
};
}