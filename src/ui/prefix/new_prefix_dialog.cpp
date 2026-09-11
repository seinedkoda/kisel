#include "new_prefix_dialog.hpp"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "core/app/app.hpp"

using namespace kisel;

NewPrefixDialog::NewPrefixDialog(QWidget* parent)
    : QDialog(parent)
    , m_nameInput(new QLineEdit(this))
    , m_saveButton(new QPushButton(QIcon::fromTheme("document-save"), tr("Save"), this))
{
    setWindowTitle(tr("Add new prefix"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(400);

    auto* layout = new QVBoxLayout(this);

    auto* nameLabel = new QLabel(tr("Enter the prefix name"), this);
    layout->addWidget(nameLabel);

    m_nameInput->setPlaceholderText(tr("Name"));
    layout->addWidget(m_nameInput);

    m_saveButton->setEnabled(false);

    auto* closeButton = new QPushButton(QIcon::fromTheme("window-close"), tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &NewPrefixDialog::close);

    auto* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(m_saveButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);
    layout->addWidget(buttonBox);

    connect(m_nameInput, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_saveButton->setEnabled(PREFIX_MODEL->isValidPrefixName(text));
    });

    connect(m_saveButton, &QPushButton::clicked, this, [this]() {
        Prefix* prefix = PREFIX_MODEL->add(m_nameInput->text());
        prefix->makePath();
        close();
    });

    adjustSize();
    setFixedSize(size());
}
