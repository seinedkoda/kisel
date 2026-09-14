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

    auto* defaultPrefixLabel = new QLabel(tr("Default prefix"), this);
    layout->addWidget(defaultPrefixLabel);

    auto* individualPrefixCheckBox = new QCheckBox(tr("Individual"), this);
    individualPrefixCheckBox->setChecked(APP_SETTINGS->useIndividualPrefix());
    layout->addWidget(individualPrefixCheckBox);

    auto* prefixComboBox = new QComboBox(this);
    prefixComboBox->setModel(PREFIX_MODEL);
    prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
    prefixComboBox->setDisabled(APP_SETTINGS->useIndividualPrefix());
    connect(prefixComboBox, &QComboBox::currentIndexChanged, this, [](int index) {
        APP_SETTINGS->setDefaultPrefixPath(PREFIX_MODEL->forIndex(index)->path());
    });
    layout->addWidget(prefixComboBox);

    connect(individualPrefixCheckBox, &QCheckBox::clicked, this, [prefixComboBox](bool checked) {
        APP_SETTINGS->setUseIndividualPrefix(checked);
        prefixComboBox->setDisabled(checked);
    });

    auto* bottomDefaultPrefixLine = new QFrame(this);
    bottomDefaultPrefixLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomDefaultPrefixLine);

    auto* prefixListWidget = new PrefixListWidget(this);
    prefixListWidget->layout()->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(prefixListWidget);
}
