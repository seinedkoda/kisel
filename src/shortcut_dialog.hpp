#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QObject>

#include "executable_file.hpp"

namespace kisel {
class ShortcutDialog : public QDialog {
    Q_OBJECT
public:
    ShortcutDialog(ExecutableFile* exeFile, QWidget* parent = nullptr);

private:
    static const QMap<QString, QString>& categoryMap();

    QLineEdit* m_nameEdit;
    QCheckBox* m_menuCheckBox;
    QCheckBox* m_desktopCheckbox;
    QComboBox* m_categoryComboBox;
};
}