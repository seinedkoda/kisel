#include "shortcuts_dialog.hpp"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>
#include <utility>

#include "core/app/app.hpp"
#include "core/appsettings/app_settings.hpp"
#include "core/prefix/prefix_model.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

ShortcutDialog::ShortcutDialog(RunConfig* runConfig, QWidget* parent)
    : QDialog(parent)
    , m_exeFile(runConfig->exeFile())
    , m_currentPrefix(runConfig->prefix())
    , m_menuCheckBox(new QCheckBox(tr("Menu"), this))
    , m_desktopCheckbox(new QCheckBox(tr("Desktop"), this))
    , m_iconToolButton(new QToolButton(this))
    , m_individualPrefixCheckBox(new QCheckBox(tr("Individual"), this))
    , m_prefixComboBox(new QComboBox(this))
    , m_categoryComboBox(new QComboBox(this))
    , m_iconMenu(new QMenu(this))
{
    setWindowTitle(tr("Shortcuts"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(300);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    auto* titleLabel = new QLabel(tr("<h3>Shortcuts</h3>"));
    layout->addWidget(titleLabel);

    m_menuShortcut = SHORTCUT_MODEL->shortcut(m_exeFile->id(), ShortcutLocation::Menu);

    m_menuCheckBox->setIcon(QIcon::fromTheme("start-here-symbolic"));
    m_menuCheckBox->setChecked(m_menuShortcut != nullptr);
    layout->addWidget(m_menuCheckBox);

    m_desktopShortcut = SHORTCUT_MODEL->shortcut(m_exeFile->id(), ShortcutLocation::Desktop);

    m_desktopCheckbox->setIcon(QIcon::fromTheme("user-desktop-symbolic"));
    m_desktopCheckbox->setChecked(m_desktopShortcut != nullptr);
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

    m_iconToolButton->setIconSize(QSize(64, 64));
    m_iconToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_iconToolButton->setMenu(m_iconMenu);
    appearanceLayout->addWidget(m_iconToolButton);

    // Add icon list
    const QIcon& icon = m_exeFile->icon();
    setIconSizes(icon);
    for (const QSize& size : std::as_const(m_iconSizes)) {
        auto* action = m_iconMenu->addAction(icon.pixmap(size), QStringLiteral("%1x%2").arg(size.width()).arg(size.height()));
        action->setData(size);
    }

    connect(m_iconToolButton, &QToolButton::triggered, this, [this](QAction* action) {
        m_currentIconSize = action->data().toSize();
        m_iconToolButton->setIcon(action->icon().pixmap(m_currentIconSize).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    auto* bottomAppearanceLine = new QFrame(this);
    bottomAppearanceLine->setFrameShape(QFrame::HLine);
    parametersLayout->addWidget(bottomAppearanceLine);

    auto* categoryLabel = new QLabel(tr("Category"), this);
    parametersLayout->addWidget(categoryLabel);

    // Add categories
    for (auto i = categoryMap().cbegin(), end = categoryMap().cend(); i != end; ++i) {
        m_categoryComboBox->addItem(i.value(), i.key());
    }

    parametersLayout->addWidget(m_categoryComboBox);

    auto* bottomCategoryLine = new QFrame(this);
    bottomCategoryLine->setFrameShape(QFrame::HLine);
    parametersLayout->addWidget(bottomCategoryLine);

    auto* prefixLabel = new QLabel(tr("Prefix"), this);
    parametersLayout->addWidget(prefixLabel);

    parametersLayout->addWidget(m_individualPrefixCheckBox);

    m_prefixComboBox->setPlaceholderText(m_exeFile->id());
    m_prefixComboBox->setModel(PREFIX_MODEL);
    parametersLayout->addWidget(m_prefixComboBox);

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

    setDefaultParameters();

    connect(m_individualPrefixCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_prefixComboBox->setDisabled(checked);
        if (checked) {
            m_prefixComboBox->setCurrentIndex(-1);
        } else {
            m_prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
        }
    });

    adjustSize();
    setFixedSize(size());
}

void ShortcutDialog::setDefaultParameters()
{
    const QString individualPrefixName = m_exeFile->id();
    m_individualPrefix = PREFIX_MODEL->forName(individualPrefixName);
    if (m_individualPrefix == nullptr) {
        m_individualPrefix = new Prefix(individualPrefixName, this);
    }

    Shortcut* shortcut = nullptr;
    if (m_menuShortcut != nullptr) {
        shortcut = m_menuShortcut;
    } else if (m_desktopShortcut != nullptr) {
        shortcut = m_desktopShortcut;
    }

    if (shortcut == nullptr) {
        m_nameEdit->setText(m_exeFile->baseName());
        bool isIndividualPrefix = m_currentPrefix->name() == individualPrefixName;
        if (isIndividualPrefix) {
            m_prefixComboBox->setCurrentIndex(-1);
        } else {
            m_prefixComboBox->setCurrentText(m_currentPrefix->name());
        }
        m_prefixComboBox->setDisabled(isIndividualPrefix);
        m_individualPrefixCheckBox->setChecked(isIndividualPrefix);
        m_categoryComboBox->setCurrentText(tr("Game"));
    } else {
        m_nameEdit->setText(shortcut->name());
        m_currentPrefix = PREFIX_MODEL->forName(shortcut->prefixName());
        if (m_currentPrefix == nullptr) {
            if (APP_SETTINGS->useIndividualPrefix()) {
                m_prefixComboBox->setCurrentIndex(-1);
            } else {
                m_prefixComboBox->setCurrentText(PREFIX_MODEL->defaultPrefix()->name());
            }
        }
        bool isIndividualPrefix = m_currentPrefix->name() == individualPrefixName;
        m_prefixComboBox->setCurrentText(m_currentPrefix->name());
        m_prefixComboBox->setDisabled(isIndividualPrefix);
        m_individualPrefixCheckBox->setChecked(isIndividualPrefix);
        m_categoryComboBox->setCurrentText(categoryMap().value(shortcut->category()));
        QList<QSize> shortcutIconSizes = shortcut->icon().availableSizes();
        if (!shortcutIconSizes.isEmpty()) {
            m_currentIconSize = shortcut->icon().availableSizes().constFirst();
        } else if (!m_iconSizes.isEmpty()) {
            m_currentIconSize = m_iconSizes.first();
        }
    }

    if (!m_exeFile->icon().isNull()) {
        m_iconToolButton->setIcon(m_exeFile->icon().pixmap(m_currentIconSize).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ShortcutDialog::setIconSizes(const QIcon& icon)
{
    m_iconSizes = icon.availableSizes();
    std::ranges::sort(m_iconSizes,
        [](const QSize& a, const QSize& b) {
            return (a.width() * a.height()) > (b.width() * b.height());
        });

    if (!m_iconSizes.isEmpty()) {
        m_currentIconSize = m_iconSizes.first();
    }
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
    } else {
        SHORTCUT_MODEL->removeShortcut(m_menuShortcut);
    }

    if (m_desktopCheckbox->isChecked()) {
        locations |= ShortcutLocation::Desktop;
    } else {
        SHORTCUT_MODEL->removeShortcut(m_desktopShortcut);
    }

    if (locations != 0U) {
        m_currentPrefix->makePath();
        SHORTCUT_MODEL->createShortcut(m_exeFile, m_currentPrefix, locations, m_nameEdit->text(), m_currentIconSize, category);
        PREFIX_MODEL->refreshList();
    }

    close();
}

const QMap<QString, QString>& ShortcutDialog::categoryMap()
{
    static const QMap<QString, QString> categoryMap {
        { "Other"_L1, tr("Other") },
        { "AudioVideo"_L1, tr("AudioVideo") },
        { "Audio"_L1, tr("Audio") },
        { "Video"_L1, tr("Video") },
        { "Development"_L1, tr("Development") },
        { "Education"_L1, tr("Education") },
        { "HealthFitness"_L1, tr("HealthFitness") },
        { "Game"_L1, tr("Game") },
        { "Graphics"_L1, tr("Graphics") },
        { "Network"_L1, tr("Network") },
        { "Office"_L1, tr("Office") },
        { "Science"_L1, tr("Science") },
        { "Settings"_L1, tr("Settings") },
        { "System"_L1, tr("System") },
        { "Utility"_L1, tr("Utility") }
    };

    return categoryMap;
}
