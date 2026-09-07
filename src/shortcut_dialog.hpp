#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QToolButton>

#include "run_config.hpp"

namespace kisel {
class ShortcutDialog : public QDialog {
    Q_OBJECT
public:
    ShortcutDialog(RunConfig* runConfig, QWidget* parent = nullptr);

private slots:
    void onAccepted();

private:
    static const QMap<QString, QString>& categoryMap();

    void setIconSizes(const QIcon& icon);

    const ExecutableFile* m_exeFile;
    const Prefix* m_currentPrefix;
    const Prefix* m_individualPrefix = nullptr;
    QList<QSize> m_iconSizes;
    QSize m_currentSize;
    QComboBox* m_prefixComboBox;
    QLineEdit* m_nameEdit;
    QToolButton* m_iconToolButton;
    QCheckBox* m_menuCheckBox;
    QCheckBox* m_desktopCheckbox;
    QComboBox* m_categoryComboBox;
};
}