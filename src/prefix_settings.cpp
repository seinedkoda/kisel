#include "prefix_settings.hpp"

using namespace Qt::StringLiterals;
using namespace kisel;

PrefixSettings::PrefixSettings(const QString& path, QObject* parent)
    : QSettings(path, QSettings::IniFormat, parent)
{
}

void PrefixSettings::setCtPath(const QString& path)
{
    setValue("ct"_L1, path);
}

QString PrefixSettings::ctPath() const
{
    return value("ct"_L1).toString();
}

void PrefixSettings::setMangoHudEnabled(bool enabled)
{
    setValue("mangohud"_L1, enabled);
}

bool PrefixSettings::mangoHudEnabled() const
{
    return value("mangohud"_L1, false).toBool();
}

void PrefixSettings::setObsVkCaptureEnabled(bool enabled)
{
    setValue("obsvkcapture"_L1, enabled);
}

bool PrefixSettings::obsVkCaptureEnabled() const
{
    return value("obsvkcapture"_L1, false).toBool();
}

void PrefixSettings::setXaliaEnabled(bool enabled)
{
    setValue("xalia"_L1, enabled);
}

bool PrefixSettings::xaliaEnabled() const
{
    return value("xalia"_L1, true).toBool();
}

void PrefixSettings::setNvapiEnabled(bool enabled)
{
    setValue("nvapi"_L1, enabled);
}

bool PrefixSettings::nvapiEnabled() const
{
    return value("nvapi"_L1, false).toBool();
}

void PrefixSettings::setWaylandEnabled(bool enabled)
{
    setValue("wayland"_L1, enabled);
}

bool PrefixSettings::waylandEnabled() const
{
    return value("wayland"_L1, false).toBool();
}

void PrefixSettings::setHdrEnabled(bool enabled)
{
    setValue("hdr"_L1, enabled);
}

bool PrefixSettings::hdrEnabled() const
{
    return value("hdr"_L1, false).toBool();
}

void PrefixSettings::setWow64Enabled(bool enabled)
{
    setValue("wow64"_L1, enabled);
}

bool PrefixSettings::wow64Enabled() const
{
    return value("wow64"_L1, false).toBool();
}

void PrefixSettings::setSdlInputEnabled(bool enabled)
{
    setValue("sdlInput"_L1, enabled);
}

bool PrefixSettings::sdlInputEnabled() const
{
    return value("sdlInput"_L1, false).toBool();
}

void PrefixSettings::setOpenglEnabled(bool enabled)
{
    setValue("opengl"_L1, enabled);
}

bool PrefixSettings::openglEnabled() const
{
    return value("opengl"_L1, false).toBool();
}

void PrefixSettings::setSteamEnabled(bool enabled)
{
    setValue("steam"_L1, enabled);
}

bool PrefixSettings::steamEnabled() const
{
    return value("steam"_L1, false).toBool();
}

void PrefixSettings::setSteamEnvEnabled(bool enabled)
{
    setValue("steamEnv"_L1, enabled);
}

bool PrefixSettings::steamEnvEnabled() const
{
    return value("steamEnv"_L1, true).toBool();
}

void PrefixSettings::setSteamOverlayEnabled(bool enabled)
{
    setValue("steamOverlay"_L1, enabled);
}

bool PrefixSettings::steamOverlayEnabled() const
{
    return value("steamOverlay"_L1, true).toBool();
}

void PrefixSettings::setOnlineFixEnabled(bool enabled)
{
    setValue("onlineFix"_L1, enabled);
}

bool PrefixSettings::onlineFixEnabled() const
{
    return value("onlineFix"_L1, false).toBool();
}