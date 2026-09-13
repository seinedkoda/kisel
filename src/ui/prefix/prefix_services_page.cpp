#include "prefix_services_page.hpp"

#include <QCheckBox>
#include <QVBoxLayout>

#include "core/appsettings/app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixServicesPage::PrefixServicesPage(PrefixSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* mangohudCheckBox = new QCheckBox("MangoHud"_L1, this);
    mangohudCheckBox->setDisabled(APP_SETTINGS->mangoHudPath().isEmpty());
    mangohudCheckBox->setToolTip(tr("Enable Performance Monitor (requires mangohud to be installed)"));
    mangohudCheckBox->setChecked(m_settings->mangoHudEnabled());
    connect(mangohudCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setMangoHudEnabled(checked);
    });
    layout->addWidget(mangohudCheckBox);

    auto* obsVkCaptureCheckBox = new QCheckBox("OBS Vulkan Capture"_L1, this);
    obsVkCaptureCheckBox->setDisabled(APP_SETTINGS->obsVkCapturePath().isEmpty());
    obsVkCaptureCheckBox->setToolTip(tr("Enable Vulkan app screen capture for OBS (requires obs-vkcapture to be installed)"));
    obsVkCaptureCheckBox->setChecked(m_settings->obsVkCaptureEnabled());
    connect(obsVkCaptureCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setObsVkCaptureEnabled(checked);
    });
    layout->addWidget(obsVkCaptureCheckBox);

    auto* xaliaCheckBox = new QCheckBox("Xalia"_L1, this);
    xaliaCheckBox->setToolTip(tr("Enable accessibility controls, such as controlling the application interface with a gamepad"));
    xaliaCheckBox->setChecked(m_settings->xaliaEnabled());
    connect(xaliaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setXaliaEnabled(checked);
    });
    layout->addWidget(xaliaCheckBox);
}
