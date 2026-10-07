#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QObject>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>

#include "core/prefix/prefix.hpp"

namespace kisel {
class PrefixComponentsDialog : public QDialog {
    Q_OBJECT

public:
    explicit PrefixComponentsDialog(Prefix* prefix, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void filterItems(const QString& text);
    void onInstallCancelButtonClicked();

private:
    void loadComponents();
    void loadInstalledComponents();
    void installSelected();
    void parseAndAddLine(const QString& line);
    void resetWidgetsState();
    void updateSelectedComponents();

    QStringList m_selectedComponents;
    Prefix* m_prefix;
    QComboBox* m_categoryList;
    QListWidget* m_componentsListWidget;
    QLineEdit* m_searchLineEdit;
    QProgressBar* m_progressBar;
    QPushButton* m_installButton;
    QPushButton* m_closeButton;
    bool m_installationCancelled;
};
}