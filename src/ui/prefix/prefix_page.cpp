#include "prefix_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "ui/prefix/prefix_list_widget.hpp"

using namespace kisel;

PrefixPage::PrefixPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* defaultPrefixTypeLabel = new QLabel(tr("Default prefix type"), this);
    layout->addWidget(defaultPrefixTypeLabel);

    auto* prefixTypeComboBox = new QComboBox(this);
    prefixTypeComboBox->addItems({ tr("Shared"), tr("Individual"), tr("Portable") });
    prefixTypeComboBox->setCurrentIndex(APP_SETTINGS->prefixType());
    layout->addWidget(prefixTypeComboBox);

    connect(prefixTypeComboBox, &QComboBox::currentIndexChanged, this, [](int type) {
        APP_SETTINGS->setPrefixType(static_cast<AppSettings::PrefixType>(type));
    });

    auto* defaultPrefixLabel = new QLabel(tr("Default shared prefix"), this);
    layout->addWidget(defaultPrefixLabel);

    auto* prefixComboBox = new QComboBox(this);
    prefixComboBox->setModel(PREFIX_MODEL);
    prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    connect(prefixComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultPrefixPath(PREFIX_MODEL->getByIndex(index)->path());
    });
    layout->addWidget(prefixComboBox);

    auto* bottomDefaultPrefixLine = new QFrame(this);
    bottomDefaultPrefixLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomDefaultPrefixLine);

    auto* prefixListWidget = new PrefixListWidget(this);
    prefixListWidget->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(prefixListWidget);
}
