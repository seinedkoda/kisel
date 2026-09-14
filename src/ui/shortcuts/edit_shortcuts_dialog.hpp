#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QToolButton>

#include "core/executablefile/executable_file.hpp"
#include "core/prefix/prefix.hpp"
#include "core/shortcuts/shortcut.hpp"

namespace kisel {
class EditShortcutsDialog : public QDialog {
    Q_OBJECT

public:
    EditShortcutsDialog(const QString& exeFilePath, Prefix* prefix = nullptr, QWidget* parent = nullptr);

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