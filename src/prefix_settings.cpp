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

void PrefixSettings::setNvapiEnabled(bool enabled)
{
    setValue("nvapi", enabled);
}

bool PrefixSettings::nvapiEnabled() const
{
    return value("nvapi", false).toBool();
}

void PrefixSettings::setWaylandEnabled(bool enabled)
{
    setValue("wayland", enabled);
}

bool PrefixSettings::waylandEnabled() const
{
    return value("wayland", false).toBool();
}

void PrefixSettings::setHdrEnabled(bool enabled) {
    setValue("hdr", enabled);
}

bool PrefixSettings::hdrEnabled() const {
    return value("hdr", false).toBool();
}

void PrefixSettings::setWow64Enabled(bool enabled)
{
    setValue("wow64", enabled);
}

bool PrefixSettings::wow64Enabled() const
{
    return value("wow64", false).toBool();
}

void PrefixSettings::setSdlInputEnabled(bool enabled)
{
    setValue("sdlInput", enabled);
}

bool PrefixSettings::sdlInputEnabled() const
{
    return value("sdlInput", false).toBool();
}

void PrefixSettings::setOpenglEnabled(bool enabled)
{
    setValue("opengl", enabled);
}

bool PrefixSettings::openglEnabled() const
{
    return value("opengl", false).toBool();
}

void PrefixSettings::setSteamEnabled(bool enabled)
{
    setValue("steam", enabled);
}

bool PrefixSettings::steamEnabled() const
{
    return value("steam", false).toBool();
}

void PrefixSettings::setSteamEnvEnabled(bool enabled) {
    setValue("steamEnv", enabled);
}

bool PrefixSettings::steamEnvEnabled() const {
    return value("steamEnv", true).toBool();
}

void PrefixSettings::setOnlineFixEnabled(bool enabled)
{
    setValue("onlineFix", enabled);
}

bool PrefixSettings::onlineFixEnabled() const
{
    return value("onlineFix", false).toBool();
}