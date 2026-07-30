#include "prefix_settings_dialog.hpp"

#include <QGroupBox>
#include <QLabel>
#include <QStandardPaths>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTabBar>

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

    auto* helpLabel = new QLabel(tr("<i>Hover over the option to learn more</i>"), this);
    layout->addWidget(helpLabel);

    auto* tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget);

    auto* serviceTab = new QWidget(this);
    auto* serviceTabLayout = new QVBoxLayout(serviceTab);
    serviceTabLayout->setAlignment(Qt::AlignTop);
    tabWidget->addTab(serviceTab, QIcon::fromTheme("services"), tr("Services"));

    auto* mangohudCheckBox = new QCheckBox("MangoHud", this);
    mangohudCheckBox->setEnabled(!QStandardPaths::findExecutable("mangohud").isEmpty());
    mangohudCheckBox->setToolTip(tr("Enable Performance Monitor (requires mangohud to be installed)"));
    mangohudCheckBox->setChecked(m_prefix.settings()->mangoHudEnabled());
    connect(mangohudCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setMangoHudEnabled(checked);
    });
    serviceTabLayout->addWidget(mangohudCheckBox);

    auto* obsVkCaptureCheckBox = new QCheckBox("OBS Vulkan Capture", this);
    obsVkCaptureCheckBox->setEnabled(!QStandardPaths::findExecutable("obs-vkcapture").isEmpty());
    obsVkCaptureCheckBox->setToolTip(tr("Enable Vulkan app screen capture for OBS (requires obs-vkcapture to be installed)"));
    obsVkCaptureCheckBox->setChecked(m_prefix.settings()->obsVkCaptureEnabled());
    connect(obsVkCaptureCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setObsVkCaptureEnabled(checked);
    });
    serviceTabLayout->addWidget(obsVkCaptureCheckBox);

    auto* xaliaCheckBox = new QCheckBox("Xalia", this);
    xaliaCheckBox->setToolTip(tr("Enable accessibility controls, such as controlling the application interface with a gamepad"));
    xaliaCheckBox->setChecked(m_prefix.settings()->xaliaEnabled());
    connect(xaliaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setXaliaEnabled(checked);
    });
    serviceTabLayout->addWidget(xaliaCheckBox);

    auto* compatibilityTab = new QWidget(this);
    auto* compatibilityTabLayout = new QVBoxLayout(compatibilityTab);
    compatibilityTabLayout->setAlignment(Qt::AlignTop);
    tabWidget->addTab(compatibilityTab, QIcon::fromTheme("tools-wizard"), tr("Compatibility"));

    auto* nvapiCheckBox = new QCheckBox(tr("NVAPI"), this);
    nvapiCheckBox->setToolTip(tr("Enable NVIDIA's NVAPI GPU support library"));
    nvapiCheckBox->setChecked(m_prefix.settings()->nvapiEnabled());
    connect(nvapiCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setNvapiEnabled(checked);
    });
    compatibilityTabLayout->addWidget(nvapiCheckBox);

    auto* waylandCheckBox = new QCheckBox(tr("Enable Wayland driver"), this);
    waylandCheckBox->setChecked(m_prefix.settings()->waylandEnabled());
    connect(waylandCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setWaylandEnabled(checked);
    });
    compatibilityTabLayout->addWidget(waylandCheckBox);

    auto* hdrCheckBox = new QCheckBox(tr("HDR"), this);
    hdrCheckBox->setToolTip(tr("Enabling HDR auto-enables the wine-wayland driver as it is a requirement"));
    hdrCheckBox->setChecked(m_prefix.settings()->hdrEnabled());
    connect(hdrCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setHdrEnabled(checked);
    });
    compatibilityTabLayout->addWidget(hdrCheckBox);

    auto* wow64CheckBox = new QCheckBox(tr("Enable WOW64"));
    wow64CheckBox->setChecked(m_prefix.settings()->wow64Enabled());
    connect(wow64CheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setWow64Enabled(checked);
    });
    compatibilityTabLayout->addWidget(wow64CheckBox);

    auto* sdlInputCheckBox = new QCheckBox(tr("SDL input instead of HIDRAW/Steam Input"));
    sdlInputCheckBox->setChecked(m_prefix.settings()->sdlInputEnabled());
    connect(sdlInputCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setSdlInputEnabled(checked);
    });
    compatibilityTabLayout->addWidget(sdlInputCheckBox);

    auto* openglCheckBox = new QCheckBox(tr("OpenGL instead of Vulkan"));
    openglCheckBox->setChecked(m_prefix.settings()->openglEnabled());
    connect(openglCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setOpenglEnabled(checked);
    });
    compatibilityTabLayout->addWidget(openglCheckBox);

    auto* steamTab = new QWidget(this);
    auto* steamTabLayout = new QVBoxLayout(steamTab);
    tabWidget->addTab(steamTab, QIcon::fromTheme("steam"), tr("Using Steam"));

    auto* dontUseSteamBox = new QGroupBox(tr("Don't use Steam"), this);
    dontUseSteamBox->setCheckable(true);
    dontUseSteamBox->setChecked(!m_prefix.settings()->steamEnabled());
    auto* dontUseSteamBoxLayout = new QVBoxLayout(dontUseSteamBox);
    dontUseSteamBoxLayout->setAlignment(Qt::AlignTop);
    steamTabLayout->addWidget(dontUseSteamBox);

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
    useSteamBoxLayout->setAlignment(Qt::AlignTop);
    steamTabLayout->addWidget(useSteamBox);

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

    tabWidget->setMinimumWidth(tabWidget->tabBar()->sizeHint().width() + 2);
}
