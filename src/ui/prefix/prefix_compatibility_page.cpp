#include "prefix_compatibility_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/compatibilitytools/ct_model.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixCompatibilityPage::PrefixCompatibilityPage(PrefixSettings* settings, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_oldDeviceInfoWidget(new OldDeviceInfoWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* ctLabel = new QLabel(tr("Compatibility tool"), this);
    layout->addWidget(ctLabel);

    auto* ctComboBox = new QComboBox(this);
    ctComboBox->setPlaceholderText(tr("<No installed>"));
    auto* ctInstalledProxyModel = new CtInstalledProxyModel(this);
    ctInstalledProxyModel->setSourceModel(CT_MODEL);
    ctComboBox->setModel(ctInstalledProxyModel);
    Ct* prefixCt = CT_MODEL->forPath(settings->ctPath());
    if (prefixCt != nullptr) {
        ctComboBox->setCurrentIndex(CT_MODEL->ctIndex(prefixCt));
    }
    connect(ctComboBox, &QComboBox::currentIndexChanged, this, [this, ctComboBox](int index) {
        m_settings->setCtPath(CT_MODEL->forIndex(index)->path());
        m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(ctComboBox->currentText()));
    });
    layout->addWidget(ctComboBox);

    layout->addWidget(m_oldDeviceInfoWidget);
    m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(ctComboBox->currentText()));

    auto* nvapiCheckBox = new QCheckBox("NVAPI"_L1, this);
    nvapiCheckBox->setToolTip(tr("Enable NVIDIA's NVAPI GPU support library"));
    nvapiCheckBox->setChecked(settings->nvapiEnabled());
    connect(nvapiCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setNvapiEnabled(checked);
    });
    layout->addWidget(nvapiCheckBox);

    auto* waylandCheckBox = new QCheckBox(tr("Enable Wayland driver"), this);
    waylandCheckBox->setChecked(settings->waylandEnabled());
    connect(waylandCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setWaylandEnabled(checked);
    });
    layout->addWidget(waylandCheckBox);

    auto* hdrCheckBox = new QCheckBox("HDR"_L1, this);
    hdrCheckBox->setToolTip(tr("Enabling HDR auto-enables the wine-wayland driver as it is a requirement"));
    hdrCheckBox->setChecked(settings->hdrEnabled());
    connect(hdrCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setHdrEnabled(checked);
    });
    layout->addWidget(hdrCheckBox);

    auto* wow64CheckBox = new QCheckBox(tr("Enable WOW64"));
    wow64CheckBox->setToolTip(tr("Compatibility with 32-bit applications"));
    wow64CheckBox->setChecked(settings->wow64Enabled());
    connect(wow64CheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setWow64Enabled(checked);
    });
    layout->addWidget(wow64CheckBox);

    auto* sdlInputCheckBox = new QCheckBox(tr("SDL input instead of HIDRAW/Steam Input"));
    sdlInputCheckBox->setChecked(settings->sdlInputEnabled());
    connect(sdlInputCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setSdlInputEnabled(checked);
    });
    layout->addWidget(sdlInputCheckBox);

    auto* openglCheckBox = new QCheckBox(tr("OpenGL instead of Vulkan"));
    openglCheckBox->setChecked(settings->openglEnabled());
    connect(openglCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_settings->setOpenglEnabled(checked);
    });
    layout->addWidget(openglCheckBox);
}
