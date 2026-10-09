#include "compatibility_settings_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/compatibilitytools/ct_model.hpp"
#include "ui/compatibilitytools/ct_list_widget.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

CompatibilitySettingsPage::CompatibilitySettingsPage(BaseSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_oldDeviceInfoWidget(new OldDeviceInfoWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* ctLabel = new QLabel(tr("Compatibility tool"), this);
    layout->addWidget(ctLabel);

    auto* ctWidget = new QWidget(this);
    layout->addWidget(ctWidget);

    auto* ctLayout = new QHBoxLayout(ctWidget);
    ctLayout->setContentsMargins(0, 0, 0, 0);

    auto* ctComboBox = new QComboBox(this);
    ctComboBox->setPlaceholderText(tr("<No installed>"));
    auto* ctInstalledProxyModel = new CtInstalledProxyModel(this);
    ctInstalledProxyModel->setSourceModel(CT_MODEL);
    ctComboBox->setModel(ctInstalledProxyModel);
    Ct* prefixCt = CT_MODEL->getByPath(m_settings->ctPath());
    if (prefixCt != nullptr) {
        ctComboBox->setCurrentIndex(CT_MODEL->indexOf(prefixCt));
    }
    connect(ctComboBox, &QComboBox::currentIndexChanged, this, [this, ctComboBox](int index) {
        m_settings->setCtPath(CT_MODEL->getByIndex(index)->path());
        m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(ctComboBox->currentText()));
    });
    ctLayout->addWidget(ctComboBox);

    auto* ctWindowButton = new QToolButton(this);
    ctWindowButton->setToolTip(tr("Open the Compatibility Tools window"));
    ctWindowButton->setIcon(QIcon::fromTheme("view-list-details"));
    connect(ctWindowButton, &QToolButton::clicked, this, [] {
        auto* ctListWidget = new CtListWidget();
        ctListWidget->resize(400, ctListWidget->height());
        ctListWidget->show();
    });
    ctLayout->addWidget(ctWindowButton);

    layout->addWidget(m_oldDeviceInfoWidget);
    m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(ctComboBox->currentText()));

    auto* bottomCtLine = new QFrame(this);
    bottomCtLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomCtLine);

    auto* nvapiCheckBox = new QCheckBox("NVAPI"_L1, this);
    nvapiCheckBox->setToolTip(tr("Enable NVIDIA's NVAPI GPU support library"));
    nvapiCheckBox->setChecked(m_settings->nvapiEnabled());
    connect(nvapiCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setNvapiEnabled(checked);
    });
    layout->addWidget(nvapiCheckBox);

    auto* waylandCheckBox = new QCheckBox(tr("Enable Wayland driver"), this);
    waylandCheckBox->setChecked(m_settings->waylandEnabled());
    connect(waylandCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setWaylandEnabled(checked);
    });
    layout->addWidget(waylandCheckBox);

    auto* hdrCheckBox = new QCheckBox("HDR"_L1, this);
    hdrCheckBox->setToolTip(tr("Enabling HDR auto-enables the wine-wayland driver as it is a requirement"));
    hdrCheckBox->setChecked(m_settings->hdrEnabled());
    connect(hdrCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setHdrEnabled(checked);
    });
    layout->addWidget(hdrCheckBox);

    auto* wow64CheckBox = new QCheckBox(tr("Enable WOW64"));
    wow64CheckBox->setToolTip(tr("Compatibility with 32-bit applications"));
    wow64CheckBox->setChecked(m_settings->wow64Enabled());
    connect(wow64CheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setWow64Enabled(checked);
    });
    layout->addWidget(wow64CheckBox);

    auto* sdlInputCheckBox = new QCheckBox(tr("SDL input instead of HIDRAW/Steam Input"));
    sdlInputCheckBox->setChecked(m_settings->sdlInputEnabled());
    connect(sdlInputCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setSdlInputEnabled(checked);
    });
    layout->addWidget(sdlInputCheckBox);

    auto* openglCheckBox = new QCheckBox(tr("OpenGL instead of Vulkan"));
    openglCheckBox->setChecked(m_settings->openglEnabled());
    connect(openglCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setOpenglEnabled(checked);
    });
    layout->addWidget(openglCheckBox);
}
