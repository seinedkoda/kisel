#include "prefix.hpp"

#include "ct_model.hpp"
#include "prefix_settings.hpp"

using namespace kisel;

Prefix::Prefix(const QString& path, QObject* parent)
    : QObject(parent)
    , m_dir(path)
{
    m_settings = new PrefixSettings(m_dir.filePath(".kisel/prefix.conf"), this);

    Ct* ct = CT_MODEL->forPath(m_settings->ctPath());
    m_ct = (ct != nullptr) ? ct : CT_MODEL->defaultCt();
}

QString Prefix::path() const
{
    return m_dir.absolutePath();
}

QString Prefix::name() const
{
    return m_dir.dirName();
}

Ct* Prefix::ct() const
{
    return m_ct;
}

PrefixSettings* Prefix::settings() const
{
    return m_settings;
}

bool Prefix::makePath()
{
    return m_dir.mkpath(".");
}

void Prefix::setCt(Ct* ct)
{
    m_ct = ct;
    m_settings->setCtPath(m_ct ? m_ct->path() : QString());
}
