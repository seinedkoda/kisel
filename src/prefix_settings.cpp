#include "prefix_settings.hpp"

using namespace kisel;

PrefixSettings::PrefixSettings(const QString& path, QObject* parent)
    : QSettings(path, QSettings::IniFormat, parent)
{
}

void PrefixSettings::setCtPath(const QString& path)
{
    setValue("ct", path);
}

QString PrefixSettings::ctPath() const
{
    return value("ct").toString();
}

void PrefixSettings::setMangoHudEnabled(bool enabled)
{
    setValue("mangohud", enabled);
}

bool PrefixSettings::mangoHudEnabled() const
{
    return value("mangohud", false).toBool();
}

void PrefixSettings::setObsVkCaptureEnabled(bool enabled)
{
    setValue("obsvkcapture", enabled);
}

bool PrefixSettings::obsVkCaptureEnabled() const
{
    return value("obsvkcapture", false).toBool();
}

void PrefixSettings::setXaliaEnabled(bool enabled)
{
    setValue("xalia", enabled);
}

bool PrefixSettings::xaliaEnabled() const
{
    return value("xalia", true).toBool();
}

void PrefixSettings::setWaylandEnabled(bool enabled)
{
    setValue("wayland", enabled);
}

bool PrefixSettings::waylandEnabled() const
{
    return value("wayland", false).toBool();
}

void PrefixSettings::setSteamEnabled(bool enabled)
{
    setValue("steam", enabled);
}

bool PrefixSettings::steamEnabled() const
{
    return value("steam", true).toBool();
}

void PrefixSettings::setWow64Enabled(bool enabled)
{
    setValue("wow64", enabled);
}

bool PrefixSettings::wow64Enabled() const
{
    return value("wow64", false).toBool();
}
