#include "shortcut_dialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace kisel;

ShortcutDialog::ShortcutDialog(ExecutableFile* exeFile, QWidget* parent)
    : QDialog(parent)
    , m_categoryComboBox(new QComboBox(this))
{
    setWindowTitle(tr("Create shortcut"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(300);

    auto* layout = new QVBoxLayout(this);

    auto* topWidget = new QWidget(this);
    auto* topLayout = new QGridLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setAlignment(Qt::AlignTop);
    layout->addWidget(topWidget);

    auto* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(64, 64);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(exeFile->icon().pixmap(64, 64));
    topLayout->addWidget(iconLabel, 0, 0, 2, 1, Qt::AlignHCenter);

    auto* nameLabel = new QLabel(tr("Shortcut name"), this);
    topLayout->addWidget(nameLabel, 0, 1);

    m_nameEdit = new QLineEdit(exeFile->baseName(), this);
    m_nameEdit->setPlaceholderText(tr("Shortcut name"));
    m_nameEdit->setCursorPosition(0);
    topLayout->addWidget(m_nameEdit, 1, 1);

    m_menuCheckBox = new QCheckBox(tr("Menu"), this);
    m_menuCheckBox->setChecked(true);
    topLayout->addWidget(m_menuCheckBox, 2, 0);

    for (auto i = categoryMap().cbegin(), end = categoryMap().cend(); i != end; ++i) {
        m_categoryComboBox->addItem(i.key(), i.value());
    }
    m_categoryComboBox->setCurrentText(tr("Other"));
    topLayout->addWidget(m_categoryComboBox, 2, 1);

    m_desktopCheckbox = new QCheckBox(tr("Desktop"), this);
    topLayout->addWidget(m_desktopCheckbox, 3, 0);

    connect(m_menuCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        m_categoryComboBox->setEnabled(checked);
    });

    auto* addButton = new QPushButton(QIcon::fromTheme("list-add"), tr("Add"), this);
    auto* cancelButton = new QPushButton(QIcon::fromTheme("dialog-cancel"), tr("Cancel"), this);

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
            exeFile->createShortcut(m_nameEdit->text(), ShortcutDestination::Menu, category);
        }
        if (m_desktopCheckbox->isChecked()) {
            exeFile->createShortcut(m_nameEdit->text(), ShortcutDestination::Desktop, category);
        }
        close();
    });
}

const QMap<QString, QString>& ShortcutDialog::categoryMap()
{
    static const QMap<QString, QString> categoryMap {
        { tr("Other"), "Other" },
        { tr("AudioVideo"), "AudioVideo" },
        { tr("Audio"), "Audio" },
        { tr("Video"), "Video" },
        { tr("Development"), "Development" },
        { tr("Education"), "Education" },
        { tr("HealthFitness"), "HealthFitness" },
        { tr("Game"), "Game" },
        { tr("Graphics"), "Graphics" },
        { tr("Network"), "Network" },
        { tr("Office"), "Office" },
        { tr("Science"), "Science" },
        { tr("Settings"), "Settings" },
        { tr("System"), "System" },
        { tr("Utility"), "Utility" }
    };

    return categoryMap;
}
