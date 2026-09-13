#include "prefix_platfrom_page.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "core/appsettings/app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixPlatformPage::PrefixPlatformPage(PrefixSettings* settings, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* umuBox = new QGroupBox(tr("Don't use Steam"), this);
    umuBox->setToolTip(tr("Use umu-launcher to launch"));
    umuBox->setCheckable(true);
    umuBox->setChecked(!settings->steamEnabled());
    layout->addWidget(umuBox);

    auto* umuBoxLayout = new QVBoxLayout(umuBox);
    umuBoxLayout->setAlignment(Qt::AlignTop);

    auto* steamEnvCheckBox = new QCheckBox(tr("Steam Environment"), this);
    steamEnvCheckBox->setToolTip(tr("Using the Steam environment for better compatibility with some games"));
    steamEnvCheckBox->setChecked(settings->steamEnvEnabled());
    connect(steamEnvCheckBox, &QCheckBox::clicked, this, [settings](bool checked) {
        settings->setSteamEnvEnabled(checked);
    });
    umuBoxLayout->addWidget(steamEnvCheckBox);

    auto* bottomSteamEnvLine = new QFrame(this);
    bottomSteamEnvLine->setFrameShape(QFrame::HLine);
    umuBoxLayout->addWidget(bottomSteamEnvLine);

    auto* openOnlineDB = new QLabel(tr("<a href=https://umu.openwinecomponents.org>Online database</a>"), this);
    openOnlineDB->setToolTip(tr("Database for finding game fixes\n(https://umu.openwinecomponents.org)"));
    openOnlineDB->setOpenExternalLinks(true);
    umuBoxLayout->addWidget(openOnlineDB);

    auto* gameIdLabel = new QLabel(tr("Game ID"), this);
    umuBoxLayout->addWidget(gameIdLabel);

    auto* gameIdEdit = new QLineEdit(this);
    gameIdEdit->setPlaceholderText("umu-default");
    gameIdEdit->setToolTip(tr("Determines the umu-id from the umu databases for games requiring fixes"));
    gameIdEdit->setText(settings->gameId());
    umuBoxLayout->addWidget(gameIdEdit);

    connect(gameIdEdit, &QLineEdit::textEdited, this, [settings](const QString& text) {
        settings->setGameId(text);
    });

    auto* storeLabel = new QLabel(tr("Store"), this);
    umuBoxLayout->addWidget(storeLabel);

    auto* storeWidget = new QWidget(this);
    umuBoxLayout->addWidget(storeWidget);

    auto* storeLayout = new QHBoxLayout(storeWidget);
    storeLayout->setContentsMargins(0, 0, 0, 0);

    auto* storeComboBox = new QComboBox(this);
    storeComboBox->setPlaceholderText(tr("None (Steam)"));
    storeComboBox->setToolTip(tr("Determines which store to use to search for UMU fixes"));
    storeComboBox->addItems(storeList());
    storeComboBox->setCurrentText(settings->store());
    storeLayout->addWidget(storeComboBox);

    connect(storeComboBox, &QComboBox::currentIndexChanged, this, [settings, storeComboBox]() {
        settings->setStore(storeComboBox->currentText());
    });

    auto* clearStoreButton = new QToolButton(this);
    clearStoreButton->setIcon(QIcon::fromTheme("edit-clear"));
    connect(clearStoreButton, &QToolButton::clicked, this, [storeComboBox]() {
        storeComboBox->setCurrentIndex(-1);
    });
    storeLayout->addWidget(clearStoreButton);

    auto* useSteamBox = new QGroupBox(tr("Use Steam"), this);
    useSteamBox->setEnabled(APP_SETTINGS->steamExists());
    useSteamBox->setCheckable(true);
    useSteamBox->setChecked(settings->steamEnabled());
    auto* steamLayout = new QVBoxLayout(useSteamBox);
    steamLayout->setAlignment(Qt::AlignTop);
    layout->addWidget(useSteamBox);

    auto* steamOverlayCheckBox = new QCheckBox(tr("Steam Overlay"));
    steamOverlayCheckBox->setChecked(settings->steamOverlayEnabled());
    connect(steamOverlayCheckBox, &QCheckBox::clicked, this, [settings](bool checked) {
        settings->setSteamOverlayEnabled(checked);
    });
    steamLayout->addWidget(steamOverlayCheckBox);

    auto* onlineFixCheckBox = new QCheckBox(tr("Enable OnlineFix"));
    onlineFixCheckBox->setChecked(settings->onlineFixEnabled());
    connect(onlineFixCheckBox, &QCheckBox::clicked, this, [settings](bool checked) {
        settings->setOnlineFixEnabled(checked);
    });
    steamLayout->addWidget(onlineFixCheckBox);

    connect(umuBox, &QGroupBox::toggled, this, [settings, useSteamBox](bool checked) {
        settings->setSteamEnabled(!checked);
        useSteamBox->setChecked(!checked);
    });

    connect(useSteamBox, &QGroupBox::toggled, this, [settings, umuBox](bool checked) {
        settings->setSteamEnabled(checked);
        umuBox->setChecked(!checked);
    });
}

const QStringList& PrefixPlatformPage::storeList()
{
    static const QStringList list {
        "Amazon"_L1,
        "BattleNET"_L1,
        "EA"_L1,
        "EGS"_L1,
        "GOG"_L1,
        "Humble"_L1,
        "ItchIO"_L1,
        "Ubisoft"_L1,
        "ZoomPlatform"_L1,
    };

    return list;
}