#include "prefix_settings_dialog.hpp"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>

using namespace kisel;

PrefixSettingsDialog::PrefixSettingsDialog(Prefix& prefix, QWidget* parent)
    : QDialog(parent)
    , m_prefix(prefix)
{
    setWindowTitle(tr("Kisel — Prefix Settings"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);

    auto* prefixTitleLabel = new QLabel(tr("<h3>Settings for \"%1\"</h3>").arg(prefix.name()));
    layout->addWidget(prefixTitleLabel);

    auto* generalTab = new QWidget(this);
    auto* advancedTabLayout = new QGridLayout(generalTab);
    advancedTabLayout->setAlignment(Qt::AlignTop);

    auto* tabWidget = new QTabWidget(this);
    tabWidget->addTab(generalTab, QIcon::fromTheme("user-home"), tr("General"));
    layout->addWidget(tabWidget);

    auto* mangohudCheckBox = new QCheckBox("MangoHud", this);
    mangohudCheckBox->setToolTip(tr("Enable Performance Monitor"));
    mangohudCheckBox->setChecked(m_prefix.settings()->mangoHudEnabled());
    connect(mangohudCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setMangoHudEnabled(checked);
    });
    advancedTabLayout->addWidget(mangohudCheckBox);

    auto* obsVkCaptureCheckBox = new QCheckBox("OBS Vulkan Capture", this);
    obsVkCaptureCheckBox->setToolTip(tr("Enable Vulkan app screen capture for OBS"));
    obsVkCaptureCheckBox->setChecked(m_prefix.settings()->obsVkCaptureEnabled());
    connect(obsVkCaptureCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setObsVkCaptureEnabled(checked);
    });
    advancedTabLayout->addWidget(obsVkCaptureCheckBox);

    auto* xaliaCheckBox = new QCheckBox("Xalia", this);
    xaliaCheckBox->setToolTip(tr("Enable accessibility controls, such as controlling the application interface with a gamepad"));
    xaliaCheckBox->setChecked(m_prefix.settings()->xaliaEnabled());
    connect(xaliaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setXaliaEnabled(checked);
    });
    advancedTabLayout->addWidget(xaliaCheckBox);

    auto* waylandCheckBox = new QCheckBox(tr("Enable Wayland driver"), this);
    waylandCheckBox->setChecked(m_prefix.settings()->waylandEnabled());
    connect(waylandCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setWaylandEnabled(checked);
    });
    advancedTabLayout->addWidget(waylandCheckBox);

    auto* useSteamCheckBox = new QCheckBox(tr("Steam Environment"), this);
    useSteamCheckBox->setToolTip(tr("Using the Steam environment for better compatibility with some games"));
    useSteamCheckBox->setChecked(m_prefix.settings()->steamEnabled());
    connect(useSteamCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setSteamEnabled(checked);
    });
    advancedTabLayout->addWidget(useSteamCheckBox);

    auto* wow64CheckBox = new QCheckBox(tr("Enable WOW64"));
    wow64CheckBox->setChecked(m_prefix.settings()->wow64Enabled());
    connect(wow64CheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefix.settings()->setWow64Enabled(checked);
    });
    advancedTabLayout->addWidget(wow64CheckBox);
}
