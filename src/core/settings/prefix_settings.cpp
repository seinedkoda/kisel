#include "prefix_settings.hpp"

#include "core/settings/app_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixSettings::PrefixSettings(const QString& path, QObject* parent)
    : BaseSettings(path, parent), m_appSettings(APP_SETTINGS)
{
}

bool PrefixSettings::mangoHudEnabled() const
{
    return value("mangohud"_L1, m_appSettings->mangoHudEnabled()).toBool();
}

bool PrefixSettings::gamescopeEnabled() const
{
    return value("gamescope"_L1, m_appSettings->gamescopeEnabled()).toBool();
}

QString PrefixSettings::gamescopeArgs() const
{
    return value("gamescopeArgs"_L1, m_appSettings->gamescopeArgs()).toString();
}

bool PrefixSettings::obsVkCaptureEnabled() const
{
    return value("obsvkcapture"_L1, m_appSettings->obsVkCaptureEnabled()).toBool();
}

bool PrefixSettings::xaliaEnabled() const
{
    return value("xalia"_L1, m_appSettings->xaliaEnabled()).toBool();
}

QString PrefixSettings::ctPath() const
{
    return value("ct"_L1, m_appSettings->ctPath()).toString();
}

bool PrefixSettings::nvapiEnabled() const
{
    return value("nvapi"_L1, m_appSettings->nvapiEnabled()).toBool();
}

bool PrefixSettings::waylandEnabled() const
{
    return value("wayland"_L1, m_appSettings->waylandEnabled()).toBool();
}

bool PrefixSettings::hdrEnabled() const
{
    return value("hdr"_L1, m_appSettings->hdrEnabled()).toBool();
}

bool PrefixSettings::wow64Enabled() const
{
    return value("wow64"_L1, m_appSettings->wow64Enabled()).toBool();
}

bool PrefixSettings::sdlInputEnabled() const
{
    return value("sdlInput"_L1, m_appSettings->sdlInputEnabled()).toBool();
}

bool PrefixSettings::openglEnabled() const
{
    return value("opengl"_L1, !deviceSupportsVulkan()).toBool();
}

void PrefixSettings::setGameId(const QString& gameId)
{
    setValue("gameId"_L1, gameId);
}

QString PrefixSettings::gameId() const
{
    return value("gameId"_L1).toString();
}

void PrefixSettings::setStore(const QString& store)
{
    setValue("store"_L1, store);
}

QString PrefixSettings::store() const
{
    return value("store"_L1).toString();
}

bool PrefixSettings::steamEnabled() const
{
    return value("steam"_L1, m_appSettings->steamEnabled()).toBool();
}

bool PrefixSettings::steamEnvEnabled() const
{
    return value("steamEnv"_L1, m_appSettings->steamEnvEnabled()).toBool();
}

bool PrefixSettings::steamOverlayEnabled() const
{
    return value("steamOverlay"_L1, m_appSettings->steamOverlayEnabled()).toBool();
}

bool PrefixSettings::onlineFixEnabled() const
{
    return value("onlineFix"_L1, m_appSettings->onlineFixEnabled()).toBool();
}