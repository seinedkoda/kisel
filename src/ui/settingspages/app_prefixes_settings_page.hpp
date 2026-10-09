#pragma once

#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

#include "core/settings/app_settings.hpp"

namespace kisel {
class AppPrefixesSettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit AppPrefixesSettingsPage(AppSettings* settings, QWidget* parent = nullptr);

private slots:
    void onPrefixesDirSelectClicked();
    void onPrefixesDirResetClicked();

private:
    AppSettings* m_settings;
    QLineEdit* m_sharedPrefixesDirLineEdit;
    QToolButton* m_sharedPrefixesDirSelectButton;
    QToolButton* m_sharedPrefixesDirResetButton;
};
}