#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QObject>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>

#include "prefix.hpp"

namespace kisel {
class PrefixComponentsDialog : public QDialog {
    Q_OBJECT
public:
    PrefixComponentsDialog(const Prefix& prefix, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void filterItems(const QString& text);
    void onInstallCancelButtonClicked();
    void onUpdateFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onInstallFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void loadComponents();
    void parseAndAddLine(const QString& line);
    [[nodiscard]] QStringList selectedComponents() const;
    void installSelected();
    void cancelInstallation();

    const Prefix& m_prefix;
    QProcess* m_updateProcess;
    QProcess* m_installProcess;
    QListWidget* m_componentsListWidget;
    QLineEdit* m_searchLineEdit;
    QProgressBar* m_progressBar;
    QPushButton* m_installButton;
    QPushButton* m_closeButton;
};
}