#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QToolButton>

#include "core/run/run_config.hpp"
#include "core/shortcuts/shortcut.hpp"

namespace kisel {
class ShortcutDialog : public QDialog {
    Q_OBJECT
public:
    ShortcutDialog(RunConfig* runConfig, QWidget* parent = nullptr);

private slots:
    void onAccepted();

private:
    static const QMap<QString, QString>& categoryMap();

    void setDefaultParameters();
    void setIconSizes(const QIcon& icon);

    const ExecutableFile* m_exeFile;
    Prefix* m_currentPrefix;
    Prefix* m_individualPrefix = nullptr;
    Shortcut* m_menuShortcut;
    Shortcut* m_desktopShortcut;
    QList<QSize> m_iconSizes;
    QSize m_currentIconSize;
    QCheckBox* m_individualPrefixCheckBox;
    QComboBox* m_prefixComboBox;
    QLineEdit* m_nameEdit;
    QToolButton* m_iconToolButton;
    QMenu* m_iconMenu;
    QCheckBox* m_menuCheckBox;
    QCheckBox* m_desktopCheckbox;
    QComboBox* m_categoryComboBox;
};
}