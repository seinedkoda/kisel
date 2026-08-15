#include "shortcut_dialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "prefix_model.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

ShortcutDialog::ShortcutDialog(RunConfig* runConfig, QWidget* parent)
    : QDialog(parent)
    , m_currentPrefix(runConfig->prefix())
    , m_prefixComboBox(new QComboBox(this))
    , m_categoryComboBox(new QComboBox(this))
{
    setWindowTitle(tr("Create shortcut"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(300);

    auto* exeFile = runConfig->exeFile();
    const QString individualPrefixName = Prefix::generatePrefixNameFromFile(exeFile->path());
    bool isIndividualPrefix = m_currentPrefix->name() == individualPrefixName;
    if (isIndividualPrefix) {
        m_individualPrefix = m_currentPrefix;
    } else {
        m_individualPrefix = new Prefix(individualPrefixName, this);
    }

    auto* layout = new QVBoxLayout(this);

    auto* contentWidget = new QWidget(this);
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setAlignment(Qt::AlignTop);
    layout->addWidget(contentWidget);

    auto* appearanceGroupBox = new QGroupBox(tr("Appearance"), this);
    contentLayout->addWidget(appearanceGroupBox);

    auto* appearanceLayout = new QGridLayout(appearanceGroupBox);

    auto* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(64, 64);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(exeFile->icon().pixmap(64, 64));
    appearanceLayout->addWidget(iconLabel, 0, 0, 2, 1, Qt::AlignHCenter);

    auto* rightExeIconLine = new QFrame(this);
    rightExeIconLine->setFrameShape(QFrame::VLine);
    appearanceLayout->addWidget(rightExeIconLine, 0, 1, 2, 1);

    auto* nameLabel = new QLabel(tr("Shortcut name"), this);
    appearanceLayout->addWidget(nameLabel, 0, 2);

    m_nameEdit = new QLineEdit(exeFile->baseName(), this);
    m_nameEdit->setPlaceholderText(tr("Shortcut name"));
    m_nameEdit->setCursorPosition(0);
    appearanceLayout->addWidget(m_nameEdit, 1, 2);

    auto* prefixGroupBox = new QGroupBox(tr("Prefix"), this);
    contentLayout->addWidget(prefixGroupBox);

    auto* prefixLayout = new QVBoxLayout(prefixGroupBox);

    auto* individualPrefixCheckBox = new QCheckBox(tr("Individual"), this);
    individualPrefixCheckBox->setChecked(isIndividualPrefix);
    prefixLayout->addWidget(individualPrefixCheckBox);

    m_prefixComboBox->setPlaceholderText(individualPrefixName);
    m_prefixComboBox->setModel(PREFIX_MODEL);
    m_prefixComboBox->setDisabled(isIndividualPrefix);
    if (PREFIX_MODEL->containsName(m_currentPrefix->name())) {
        m_prefixComboBox->setCurrentText(m_currentPrefix->name());
    } else {
        m_prefixComboBox->setCurrentIndex(-1);
    }
    prefixLayout->addWidget(m_prefixComboBox);

    connect(individualPrefixCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefixComboBox->setDisabled(checked);
        if (checked) {
            m_currentPrefix = m_individualPrefix;
            m_prefixComboBox->setCurrentIndex(-1);
        } else {
            m_currentPrefix = PREFIX_MODEL->defaultPrefix();
            m_prefixComboBox->setCurrentText(m_currentPrefix->name());
        }
    });

    auto* locationGroupBox = new QGroupBox(tr("Location"), this);
    contentLayout->addWidget(locationGroupBox);

    auto* locationLayout = new QGridLayout(locationGroupBox);

    m_menuCheckBox = new QCheckBox(tr("Menu"), this);
    m_menuCheckBox->setChecked(true);
    locationLayout->addWidget(m_menuCheckBox, 0, 0);

    for (auto i = categoryMap().cbegin(), end = categoryMap().cend(); i != end; ++i) {
        m_categoryComboBox->addItem(i.key(), i.value());
    }
    m_categoryComboBox->setCurrentText(tr("Other"));
    locationLayout->addWidget(m_categoryComboBox, 0, 1);

    m_desktopCheckbox = new QCheckBox(tr("Desktop"), this);
    locationLayout->addWidget(m_desktopCheckbox, 1, 0);

    connect(m_menuCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_categoryComboBox->setEnabled(checked);
    });

    auto* addButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Add"), this);
    auto* cancelButton = new QPushButton(QIcon::fromTheme("window-close"), tr("Cancel"), this);

    auto* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(addButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    layout->addWidget(buttonBox);

    connect(m_nameEdit, &QLineEdit::textChanged, this, [addButton](QStringView text) {
        addButton->setDisabled(text.isEmpty());
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this, exeFile]() {
        QString category = m_categoryComboBox->currentData().toString();
        if (m_menuCheckBox->isChecked()) {
            exeFile->createShortcut(*m_currentPrefix, m_nameEdit->text(), ExecutableFile::ShortcutDestination::Menu, category);
        }
        if (m_desktopCheckbox->isChecked()) {
            exeFile->createShortcut(*m_currentPrefix, m_nameEdit->text(), ExecutableFile::ShortcutDestination::Desktop, category);
        }
        close();
    });
}

const QMap<QString, QString>& ShortcutDialog::categoryMap()
{
    static const QMap<QString, QString> categoryMap {
        { tr("Other"), "Other"_L1 },
        { tr("AudioVideo"), "AudioVideo"_L1 },
        { tr("Audio"), "Audio"_L1 },
        { tr("Video"), "Video"_L1 },
        { tr("Development"), "Development"_L1 },
        { tr("Education"), "Education"_L1 },
        { tr("HealthFitness"), "HealthFitness"_L1 },
        { tr("Game"), "Game"_L1 },
        { tr("Graphics"), "Graphics"_L1 },
        { tr("Network"), "Network"_L1 },
        { tr("Office"), "Office"_L1 },
        { tr("Science"), "Science"_L1 },
        { tr("Settings"), "Settings"_L1 },
        { tr("System"), "System"_L1 },
        { tr("Utility"), "Utility"_L1 }
    };

    return categoryMap;
}
