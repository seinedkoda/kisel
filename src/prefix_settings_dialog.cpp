#include "prefix_settings_dialog.hpp"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStandardPaths>

#include "app_settings.hpp"

using namespace kisel;

PrefixSettingsDialog::PrefixSettingsDialog(Prefix& prefix, QWidget* parent)
    : QDialog(parent)
    , m_prefix(prefix)
{
    setWindowTitle(tr("Kisel — Prefix Settings"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* prefixTitleLabel = new QLabel(tr("<h3>Settings for \"%1\"</h3>").arg(prefix.name()));
    layout->addWidget(prefixTitleLabel);

    auto* serviceBox = new QGroupBox(tr("Services"), this);
    auto* serviceBoxLayout = new QVBoxLayout(serviceBox);
    layout->addWidget(serviceBox);

    auto* mangohudCheckBox = new QCheckBox("MangoHud", this);
    mangohudCheckBox->setEnabled(!QStandardPaths::findExecutable("mangohud").isEmpty());
    mangohudCheckBox->setToolTip(tr("Enable Performance Monitor (requires mangohud to be installed)"));
    mangohudCheckBox->setChecked(m_prefix.settings()->mangoHudEnabled());
    connect(mangohudCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setMangoHudEnabled(checked);
    });
    serviceBoxLayout->addWidget(mangohudCheckBox);

    auto* obsVkCaptureCheckBox = new QCheckBox("OBS Vulkan Capture", this);
    obsVkCaptureCheckBox->setEnabled(!QStandardPaths::findExecutable("obs-vkcapture").isEmpty());
    obsVkCaptureCheckBox->setToolTip(tr("Enable Vulkan app screen capture for OBS (requires obs-vkcapture to be installed)"));
    obsVkCaptureCheckBox->setChecked(m_prefix.settings()->obsVkCaptureEnabled());
    connect(obsVkCaptureCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setObsVkCaptureEnabled(checked);
    });
    serviceBoxLayout->addWidget(obsVkCaptureCheckBox);

    auto* xaliaCheckBox = new QCheckBox("Xalia", this);
    xaliaCheckBox->setToolTip(tr("Enable accessibility controls, such as controlling the application interface with a gamepad"));
    xaliaCheckBox->setChecked(m_prefix.settings()->xaliaEnabled());
    connect(xaliaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setXaliaEnabled(checked);
    });
    serviceBoxLayout->addWidget(xaliaCheckBox);

    auto* compatibilityBox = new QGroupBox(tr("Compatibility"), this);
    auto* compatibilityBoxLayout = new QVBoxLayout(compatibilityBox);
    layout->addWidget(compatibilityBox);

    auto* waylandCheckBox = new QCheckBox(tr("Enable Wayland driver"), this);
    waylandCheckBox->setChecked(m_prefix.settings()->waylandEnabled());
    connect(waylandCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setWaylandEnabled(checked);
    });
    compatibilityBoxLayout->addWidget(waylandCheckBox);

    auto* wow64CheckBox = new QCheckBox(tr("Enable WOW64"));
    wow64CheckBox->setChecked(m_prefix.settings()->wow64Enabled());
    connect(wow64CheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setWow64Enabled(checked);
    });
    compatibilityBoxLayout->addWidget(wow64CheckBox);

    auto* dontUseSteamBox = new QGroupBox(tr("Don't use Steam"), this);
    dontUseSteamBox->setCheckable(true);
    dontUseSteamBox->setChecked(!m_prefix.settings()->steamEnabled());
    auto* dontUseSteamBoxLayout = new QVBoxLayout(dontUseSteamBox);
    layout->addWidget(dontUseSteamBox);

    auto* steamEnvCheckBox = new QCheckBox(tr("Steam Environment"), this);
    steamEnvCheckBox->setToolTip(tr("Using the Steam environment for better compatibility with some games"));
    steamEnvCheckBox->setChecked(m_prefix.settings()->steamEnvEnabled());
    connect(steamEnvCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setSteamEnvEnabled(checked);
    });
    dontUseSteamBoxLayout->addWidget(steamEnvCheckBox);

    auto* useSteamBox = new QGroupBox(tr("Use Steam"), this);
    useSteamBox->setEnabled(APP_SETTINGS->steamExists());
    useSteamBox->setCheckable(true);
    useSteamBox->setChecked(m_prefix.settings()->steamEnabled());
    auto* useSteamBoxLayout = new QVBoxLayout(useSteamBox);
    layout->addWidget(useSteamBox);

    auto* onlineFixCheckBox = new QCheckBox(tr("Enable OnlineFix"));
    onlineFixCheckBox->setChecked(m_prefix.settings()->onlineFixEnabled());
    connect(onlineFixCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setOnlineFixEnabled(checked);
    });
    useSteamBoxLayout->addWidget(onlineFixCheckBox);

    connect(dontUseSteamBox, &QGroupBox::toggled, this, [this, useSteamBox](bool checked) {
        m_prefix.settings()->setSteamEnabled(!checked);
        useSteamBox->setChecked(!checked);
    });

    connect(useSteamBox, &QGroupBox::toggled, this, [this, dontUseSteamBox](bool checked) {
        m_prefix.settings()->setSteamEnabled(checked);
        dontUseSteamBox->setChecked(!checked);
    });
}
