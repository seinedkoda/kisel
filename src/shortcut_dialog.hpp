#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>

#include "executable_file.hpp"

namespace kisel {
class ShortcutDialog : public QDialog {
    Q_OBJECT
public:
    ShortcutDialog(ExecutableFile* exeFile, const Prefix& prefix, QWidget* parent = nullptr);

private:
    static const QMap<QString, QString>& categoryMap();

    const Prefix* m_currentPrefix;
    const Prefix* m_individualPrefix = nullptr;
    QComboBox* m_prefixComboBox;
    QLineEdit* m_nameEdit;
    QCheckBox* m_menuCheckBox;
    QCheckBox* m_desktopCheckbox;
    QComboBox* m_categoryComboBox;
};
}