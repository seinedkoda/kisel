#include "about_dialog.hpp"
#include "app_settings.hpp"

#include <QButtonGroup>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Qt::StringLiterals;
using namespace kisel;

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Kisel — About the program"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    auto* layout = new QVBoxLayout(this);

    auto* aboutAppWidget = new QWidget(this);
    layout->addWidget(aboutAppWidget);

    auto* aboutAppLayout = new QHBoxLayout(aboutAppWidget);

    auto* logoLabel = new QLabel(this);
    logoLabel->setFixedSize(64, 64);
    QPixmap logoPixmap(":/icons/kisel.svg"_L1);
    logoLabel->setPixmap(logoPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    aboutAppLayout->addWidget(logoLabel);

    auto* aboutAppInfoWidget = new QWidget(this);
    auto* aboutAppInfoLayout = new QVBoxLayout(aboutAppInfoWidget);
    aboutAppLayout->addWidget(aboutAppInfoWidget, Qt::AlignLeft);

    const QString flatpakString = APP_SETTINGS->isFlatpak() ? "(Flatpak)"_L1 : ""_L1;
    auto* nameLabel = new QLabel(tr("<b>Kisel %1 %2 ©%3</b>").arg(APP_VERSION, flatpakString, APP_AUTHOR), this);
    aboutAppInfoLayout->addWidget(nameLabel);

    auto* descriptionLabel = new QLabel(tr("<i>Efficient launch of Windows programs</i>"), this);
    aboutAppInfoLayout->addWidget(descriptionLabel);

    auto* sourceCodeLabel = new QLabel(tr("<a href='%1'>Source code (%2)</a>").arg("https://github.com/seinedkoda/kisel", "GNU GPLv3"), this);
    sourceCodeLabel->setOpenExternalLinks(true);
    aboutAppInfoLayout->addWidget(sourceCodeLabel);

    auto* bottomAppInfoLine = new QFrame(this);
    bottomAppInfoLine->setFrameShape(QFrame::HLine);
    layout->addWidget(bottomAppInfoLine);

    auto* sytemInfoWidget = new QWidget(this);
    layout->addWidget(sytemInfoWidget);

    auto* systemInfoLayout = new QVBoxLayout(sytemInfoWidget);
    systemInfoLayout->setAlignment(Qt::AlignHCenter);

    const QString supportedVulkanVersion = APP_SETTINGS->deviceSupportsVulkan() ? APP_SETTINGS->vulkanApiVersion().toString() : tr("No");

    const QString systemInfoText = QString(tr("<dl>"
                                              "<dt><b>System Information:</b></dt>"
                                              "<dd>OS: <i>%1</i></dd>"
                                              "<dd>Supported Vulkan API: <i>%2</i></dd>"
                                              "<dt><b>Components:</b></dt>"
                                              "<dd>umu-run (<i>%3</i>)</dd>"
                                              "<dd>winetricks (<i>%4</i>)</dd>"
                                              "<dd>mangohud (<i>%5</i>)</dd>"
                                              "<dd>obs-vkcapture (<i>%6</i>)</dd>"
                                              "</dl>"))
                                       .arg(QSysInfo::prettyProductName(),
                                           supportedVulkanVersion,
                                           APP_SETTINGS->umuPath(),
                                           APP_SETTINGS->winetricksPath(),
                                           APP_SETTINGS->mangoHudPath(),
                                           APP_SETTINGS->obsVkCapturePath());

    auto* systemInfoLabel = new QLabel(systemInfoText, this);
    systemInfoLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    systemInfoLayout->addWidget(systemInfoLabel);

    auto* aboutQtButton = new QPushButton(tr("About Qt"), this);
    connect(aboutQtButton, &QPushButton::clicked, this, [this]() { QMessageBox::aboutQt(this, tr("About Qt")); });

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::close);
    buttonBox->addButton(aboutQtButton, QDialogButtonBox::HelpRole);
    layout->addWidget(buttonBox);

    adjustSize();
    setFixedSize(size());
}
