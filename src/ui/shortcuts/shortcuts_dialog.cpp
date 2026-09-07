#include "shortcuts_dialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>
#include <utility>

#include "core/prefix/prefix_model.hpp"
#include "core/shortcuts/shortcuts_utils.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

ShortcutDialog::ShortcutDialog(RunConfig* runConfig, QWidget* parent)
    : QDialog(parent)
    , m_exeFile(runConfig->exeFile())
    , m_currentPrefix(runConfig->prefix())
    , m_iconToolButton(new QToolButton(this))
    , m_prefixComboBox(new QComboBox(this))
    , m_categoryComboBox(new QComboBox(this))
{
    setWindowTitle(tr("Shortcuts"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(300);

    const QString individualPrefixName = m_exeFile->id();
    m_individualPrefix = PREFIX_MODEL->forName(individualPrefixName);
    if (m_individualPrefix == nullptr) {
        m_individualPrefix = new Prefix(individualPrefixName, this);
    }
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* titleLabel = new QLabel(tr("<h3>Shortcuts</h3>"));
    layout->addWidget(titleLabel);

    m_menuCheckBox = new QCheckBox(tr("Menu"), this);
    m_menuCheckBox->setIcon(QIcon::fromTheme("start-here-symbolic"));
    m_menuCheckBox->setChecked(true);
    layout->addWidget(m_menuCheckBox);

    m_desktopCheckbox = new QCheckBox(tr("Desktop"), this);
    m_desktopCheckbox->setIcon(QIcon::fromTheme("user-desktop-symbolic"));
    layout->addWidget(m_desktopCheckbox);

    auto* parametersGroupBox = new QGroupBox(tr("Parameters"), this);
    layout->addWidget(parametersGroupBox);

    auto* parametersLayout = new QVBoxLayout(parametersGroupBox);

    auto* appearanceWidget = new QWidget(this);
    parametersLayout->addWidget(appearanceWidget);

    auto* appearanceLayout = new QHBoxLayout(appearanceWidget);
    appearanceLayout->setContentsMargins(0, 0, 0, 0);
    appearanceLayout->setSpacing(6);

    auto* nameWidget = new QWidget(this);
    appearanceLayout->addWidget(nameWidget);

    auto* nameLayout = new QVBoxLayout(nameWidget);
    nameLayout->setContentsMargins(0, 0, 0, 0);
    nameLayout->setAlignment(Qt::AlignVCenter);

    auto* nameLabel = new QLabel(tr("Shortcut name"), this);
    nameLayout->addWidget(nameLabel);

    m_nameEdit = new QLineEdit(m_exeFile->baseName(), this);
    m_nameEdit->setPlaceholderText(m_exeFile->baseName());
    m_nameEdit->setCursorPosition(0);
    nameLayout->addWidget(m_nameEdit);

    const QIcon& icon = m_exeFile->icon();
    setIconSizes(icon);

    auto* iconMenu = new QMenu(this);
    for (const QSize& size : std::as_const(m_iconSizes)) {
        auto* action = iconMenu->addAction(icon.pixmap(size), QStringLiteral("%1x%2").arg(size.width()).arg(size.height()));
        action->setData(size);
    }

    if (!icon.isNull()) {
        m_iconToolButton->setIcon(m_exeFile->icon().pixmap(m_currentSize).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    m_iconToolButton->setIconSize(QSize(64, 64));
    m_iconToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_iconToolButton->setMenu(iconMenu);
    appearanceLayout->addWidget(m_iconToolButton);

    connect(m_iconToolButton, &QToolButton::triggered, this, [this](QAction* action) {
        m_currentSize = action->data().toSize();
        m_iconToolButton->setIcon(action->icon().pixmap(m_currentSize).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    auto* bottomAppearanceLine = new QFrame(this);
    bottomAppearanceLine->setFrameShape(QFrame::HLine);
    parametersLayout->addWidget(bottomAppearanceLine);

    auto* categoryLabel = new QLabel(tr("Category"), this);
    parametersLayout->addWidget(categoryLabel);

    for (auto i = categoryMap().cbegin(), end = categoryMap().cend(); i != end; ++i) {
        m_categoryComboBox->addItem(i.key(), i.value());
    }
    m_categoryComboBox->setCurrentText(tr("Game"));
    parametersLayout->addWidget(m_categoryComboBox);

    auto* bottomCategoryLine = new QFrame(this);
    bottomCategoryLine->setFrameShape(QFrame::HLine);
    parametersLayout->addWidget(bottomCategoryLine);

    auto* prefixLabel = new QLabel(tr("Prefix"), this);
    parametersLayout->addWidget(prefixLabel);

    bool prefixIsIndividual = m_currentPrefix->name() == individualPrefixName;

    auto* individualPrefixCheckBox = new QCheckBox(tr("Individual"), this);
    individualPrefixCheckBox->setChecked(prefixIsIndividual);
    parametersLayout->addWidget(individualPrefixCheckBox);

    m_prefixComboBox->setPlaceholderText(individualPrefixName);
    m_prefixComboBox->setModel(PREFIX_MODEL);
    m_prefixComboBox->setDisabled(prefixIsIndividual);
    if (prefixIsIndividual) {
        m_prefixComboBox->setCurrentIndex(-1);
    } else {
        m_prefixComboBox->setCurrentText(m_currentPrefix->name());
    }
    parametersLayout->addWidget(m_prefixComboBox);

    connect(individualPrefixCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefixComboBox->setDisabled(checked);
        if (checked) {
            m_prefixComboBox->setCurrentIndex(-1);
        } else {
            m_prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
        }
    });

    connect(m_prefixComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index == -1) {
            m_currentPrefix = m_individualPrefix;
        } else {
            m_currentPrefix = PREFIX_MODEL->forIndex(index);
        }
    });

    auto* buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ShortcutDialog::onAccepted);

    adjustSize();
    setFixedSize(size());
}

void ShortcutDialog::onAccepted()
{
    QString name = m_nameEdit->text();
    if (name.isEmpty()) {
        name = m_exeFile->baseName();
    }

    QString category = m_categoryComboBox->currentData().toString();

    ShortcutLocations locations;
    if (m_menuCheckBox->isChecked()) {
        locations |= ShortcutLocation::Menu;
    }
    if (m_desktopCheckbox->isChecked()) {
        locations |= ShortcutLocation::Desktop;
    }

    createShortcut(m_exeFile, m_currentPrefix, locations, m_nameEdit->text(), m_currentSize, category);

    close();
}

void ShortcutDialog::setIconSizes(const QIcon& icon)
{
    m_iconSizes = icon.availableSizes();
    std::ranges::sort(m_iconSizes,
        [](const QSize& a, const QSize& b) {
            return (a.width() * a.height()) > (b.width() * b.height());
        });

    if (!m_iconSizes.isEmpty()) {
        m_currentSize = m_iconSizes.first();
    }
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
