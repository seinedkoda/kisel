#pragma once

#include <QObject>
#include <QPointer>

#include "core/compatibilitytools/ct.hpp"
#include "core/executablefile/executable_file.hpp"
#include "core/prefix/prefix.hpp"

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
    [[nodiscard]] bool exeIsValid() const;
    void setPrefix(Prefix* prefix);
    [[nodiscard]] Prefix* prefix() const;
    [[nodiscard]] bool prefixIsValid() const;
    void setCt(Ct* ct);
    [[nodiscard]] Ct* ct() const;
    [[nodiscard]] QString workingDirPath() const;
    [[nodiscard]] bool isUsingSteam() const;

private:
    ExecutableFile* m_exeFile;
    QPointer<Prefix> m_prefix;
    QPointer<Ct> m_ct;
};
}