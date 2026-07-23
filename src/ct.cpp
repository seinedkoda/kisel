#include "ct.hpp"

using namespace kisel;

Ct::Ct(const QString& path, QObject* parent)
    : QObject(parent)
    , m_dir(path)
{
    if (m_dir.absolutePath().contains("steam")) {
        m_icon = QIcon::fromTheme("steam");
    } else {
        m_icon = QIcon(":/icons/kisel.svg");
    }
}

QString Ct::path() const
{
    return m_dir.absolutePath();
};

QString Ct::name() const
{
    return m_dir.dirName();
};

QIcon Ct::icon() const
{
    return m_icon;
};

bool Ct::exists() const
{
    return m_dir.exists();
}