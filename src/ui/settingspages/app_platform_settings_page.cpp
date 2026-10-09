#include "app_platform_settings_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>

using namespace Qt::StringLiterals;
using namespace kisel;

AppPlatformSettingsPage::AppPlatformSettingsPage(AppSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* umuBox = new QGroupBox(tr("Don't use Steam"), this);
    umuBox->setToolTip(tr("Use umu-launcher to launch"));
    umuBox->setCheckable(true);
    umuBox->setChecked(!m_settings->steamEnabled());
    layout->addWidget(umuBox);

    auto* umuBoxLayout = new QVBoxLayout(umuBox);
    umuBoxLayout->setAlignment(Qt::AlignTop);

    auto* steamEnvCheckBox = new QCheckBox(tr("Steam Environment"), this);
    steamEnvCheckBox->setToolTip(tr("Using the Steam environment for better compatibility with some games"));
    steamEnvCheckBox->setChecked(m_settings->steamEnvEnabled());
    connect(steamEnvCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setSteamEnvEnabled(checked);
    });
    umuBoxLayout->addWidget(steamEnvCheckBox);

    auto* useSteamBox = new QGroupBox(tr("Use Steam"), this);
    useSteamBox->setEnabled(AppSettings::steamDirExists());
    useSteamBox->setCheckable(true);
    useSteamBox->setChecked(m_settings->steamEnabled());
    auto* steamLayout = new QVBoxLayout(useSteamBox);
    steamLayout->setAlignment(Qt::AlignTop);
    layout->addWidget(useSteamBox);

    auto* steamOverlayCheckBox = new QCheckBox(tr("Steam Overlay"));
    steamOverlayCheckBox->setChecked(m_settings->steamOverlayEnabled());
    connect(steamOverlayCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setSteamOverlayEnabled(checked);
    });
    steamLayout->addWidget(steamOverlayCheckBox);

    auto* onlineFixCheckBox = new QCheckBox(tr("Enable OnlineFix"));
    onlineFixCheckBox->setChecked(m_settings->onlineFixEnabled());
    connect(onlineFixCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setOnlineFixEnabled(checked);
    });
    steamLayout->addWidget(onlineFixCheckBox);

    connect(umuBox, &QGroupBox::toggled, this, [this, useSteamBox](bool checked) {
        m_settings->setSteamEnabled(!checked);
        useSteamBox->setChecked(!checked);
    });

    connect(useSteamBox, &QGroupBox::toggled, this, [this, umuBox](bool checked) {
        m_settings->setSteamEnabled(checked);
        umuBox->setChecked(!checked);
    });
}