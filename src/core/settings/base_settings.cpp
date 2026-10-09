#include "base_settings.hpp"

#include <QVulkanInstance>

using namespace Qt::StringLiterals;
using namespace kisel;

BaseSettings::BaseSettings(const QString& path, QObject* parent)
    : QSettings(path, QSettings::IniFormat, parent)
{
}

bool BaseSettings::deviceSupportsVulkan()
{
    QVulkanInstance vulkanInstance;
    static bool supportsVulkan = vulkanInstance.create();
    return supportsVulkan;
}

QVersionNumber BaseSettings::vulkanApiVersion()
{
    QVulkanInstance vulkanInstance;
    static QVersionNumber vulkanApiVersion = vulkanInstance.supportedApiVersion();
    return vulkanApiVersion;
}

bool BaseSettings::deviceSupportsModernVulkan()
{
    static QVersionNumber modernApiVersion(1, 4);
    return vulkanApiVersion() >= modernApiVersion;
}

void BaseSettings::setMangoHudEnabled(bool enabled)
{
    setValue("mangohud"_L1, enabled);
}

bool BaseSettings::mangoHudEnabled() const
{
    return value("mangohud"_L1, false).toBool();
}

void BaseSettings::setGamescopeEnabled(bool enabled)
{
    setValue("gamescope"_L1, enabled);
}

bool BaseSettings::gamescopeEnabled() const
{
    return value("gamescope"_L1, false).toBool();
}

void BaseSettings::setGamescopeArgs(const QString& args)
{
    setValue("gamescopeArgs"_L1, args);
}

QString BaseSettings::gamescopeArgs() const
{
    return value("gamescopeArgs"_L1).toString();
}

void BaseSettings::setObsVkCaptureEnabled(bool enabled)
{
    setValue("obsvkcapture"_L1, enabled);
}

bool BaseSettings::obsVkCaptureEnabled() const
{
    return value("obsvkcapture"_L1, false).toBool();
}

void BaseSettings::setXaliaEnabled(bool enabled)
{
    setValue("xalia"_L1, enabled);
}

bool BaseSettings::xaliaEnabled() const
{
    return value("xalia"_L1, true).toBool();
}

void BaseSettings::setCtPath(const QString& path)
{
    setValue("ct"_L1, path);
}

QString BaseSettings::ctPath() const
{
    return value("ct"_L1).toString();
}

void BaseSettings::setNvapiEnabled(bool enabled)
{
    setValue("nvapi"_L1, enabled);
}

bool BaseSettings::nvapiEnabled() const
{
    return value("nvapi"_L1, false).toBool();
}

void BaseSettings::setWaylandEnabled(bool enabled)
{
    setValue("wayland"_L1, enabled);
}

bool BaseSettings::waylandEnabled() const
{
    return value("wayland"_L1, false).toBool();
}

void BaseSettings::setHdrEnabled(bool enabled)
{
    setValue("hdr"_L1, enabled);
}

bool BaseSettings::hdrEnabled() const
{
    return value("hdr"_L1, false).toBool();
}

void BaseSettings::setWow64Enabled(bool enabled)
{
    setValue("wow64"_L1, enabled);
}

bool BaseSettings::wow64Enabled() const
{
    return value("wow64"_L1, true).toBool();
}

void BaseSettings::setSdlInputEnabled(bool enabled)
{
    setValue("sdlInput"_L1, enabled);
}

bool BaseSettings::sdlInputEnabled() const
{
    return value("sdlInput"_L1, false).toBool();
}

void BaseSettings::setOpenglEnabled(bool enabled)
{
    setValue("opengl"_L1, enabled);
}

bool BaseSettings::openglEnabled() const
{
    return value("opengl"_L1, !deviceSupportsVulkan()).toBool();
}

void BaseSettings::setSteamEnabled(bool enabled)
{
    setValue("steam"_L1, enabled);
}

bool BaseSettings::steamEnabled() const
{
    return value("steam"_L1, false).toBool();
}

void BaseSettings::setSteamEnvEnabled(bool enabled)
{
    setValue("steamEnv"_L1, enabled);
}

bool BaseSettings::steamEnvEnabled() const
{
    return value("steamEnv"_L1, true).toBool();
}

void BaseSettings::setSteamOverlayEnabled(bool enabled)
{
    setValue("steamOverlay"_L1, enabled);
}

bool BaseSettings::steamOverlayEnabled() const
{
    return value("steamOverlay"_L1, true).toBool();
}

void BaseSettings::setOnlineFixEnabled(bool enabled)
{
    setValue("onlineFix"_L1, enabled);
}

bool BaseSettings::onlineFixEnabled() const
{
    return value("onlineFix"_L1, false).toBool();
}