#include "ct_page.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "ui/compatibilitytools/ct_list_widget.hpp"

using namespace kisel;

CtPage::CtPage(QWidget* parent)
    : QWidget(parent)
    , m_oldDeviceInfoWidget(new OldDeviceInfoWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* umuLabel = new QLabel("UMU", this);
    layout->addWidget(umuLabel);

    auto* umuPathComboBox = new QComboBox(this);
    if (APP_SETTINGS->isFlatpak()) {
        umuPathComboBox->addItem(tr("Built-in (Flatpak)"), false);
        umuPathComboBox->setDisabled(true);
    } else {
        umuPathComboBox->addItem(tr("Built-in"), false);
        umuPathComboBox->addItem(tr("System"), true);
        umuPathComboBox->setCurrentIndex(APP_SETTINGS->useSystemUMU() ? 1 : 0);
        connect(umuPathComboBox, &QComboBox::activated, this, [umuPathComboBox]() {
            APP_SETTINGS->setUseSystemUMU(umuPathComboBox->currentData().toBool());
        });
    }
    layout->addWidget(umuPathComboBox);

    auto* runtimeAutoUpdateCheckBox = new QCheckBox(tr("Runtime auto-update"), this);
    runtimeAutoUpdateCheckBox->setChecked(APP_SETTINGS->runtimeAutoUpdate());
    connect(runtimeAutoUpdateCheckBox, &QCheckBox::clicked, this, [](bool checked) {
        APP_SETTINGS->setRuntimeAutoUpdate(checked);
    });
    layout->addWidget(runtimeAutoUpdateCheckBox);

    auto* bottomUmuLine = new QFrame(this);
    bottomUmuLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomUmuLine);

    auto* defaultCtLabel = new QLabel(tr("Default compatibility tool"), this);
    layout->addWidget(defaultCtLabel);

    auto* defaultCtComboBox = new QComboBox(this);
    defaultCtComboBox->setPlaceholderText(tr("<No installed>"));
    auto* ctInstalledProxyModel = new CtInstalledProxyModel(this);
    ctInstalledProxyModel->setSourceModel(CT_MODEL);
    defaultCtComboBox->setModel(ctInstalledProxyModel);
    defaultCtComboBox->setCurrentIndex(CT_MODEL->ctIndex(CT_MODEL->defaultCt()));
    connect(defaultCtComboBox, &QComboBox::currentIndexChanged, this, [this, defaultCtComboBox](int index) {
        APP_SETTINGS->setDefaultCtPath(CT_MODEL->forIndex(index)->path());
        m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(defaultCtComboBox->currentText()));
    });
    layout->addWidget(defaultCtComboBox);

    layout->addWidget(m_oldDeviceInfoWidget);
    m_oldDeviceInfoWidget->setHidden(OldDeviceInfoWidget::isCompatibleCt(defaultCtComboBox->currentText()));

    auto* bottomDefaultCtLine = new QFrame(this);
    bottomDefaultCtLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomDefaultCtLine);

    auto* ctListWidget = new CtListWidget(this);
    ctListWidget->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(ctListWidget);
}
