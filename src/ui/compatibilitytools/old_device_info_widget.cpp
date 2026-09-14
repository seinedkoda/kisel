#include "old_device_info_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>

#include "core/appsettings/app_settings.hpp"

using namespace kisel;

OldDeviceInfoWidget::OldDeviceInfoWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);

    auto* infoIcon = new QLabel(this);
    infoIcon->setPixmap(QIcon::fromTheme("help-about").pixmap(16, 16));
    layout->addWidget(infoIcon);

    auto* infoLabel = new QLabel(tr("Your device does not support Vulkan 1.4 or higher. It is recommended to select Proton-CachyOS for compatibility."), this);
    infoLabel->setWordWrap(true);
    infoLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    layout->addWidget(infoLabel);
}

bool OldDeviceInfoWidget::isCompatibleCt(const QString& ctName)
{
    if (APP_SETTINGS->deviceSupportsModernVulkan()) {
        return true;
    }
    return ctName.contains("cachyos", Qt::CaseInsensitive);
}