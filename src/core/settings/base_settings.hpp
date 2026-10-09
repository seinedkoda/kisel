#pragma once

#include <QSettings>
#include <QVersionNumber>

namespace kisel {
class BaseSettings : public QSettings {
    Q_OBJECT

public:
    explicit BaseSettings(const QString& path, QObject* parent = nullptr);

    static bool deviceSupportsVulkan();
    static QVersionNumber vulkanApiVersion();
    static bool deviceSupportsModernVulkan();

    // Services
    void setMangoHudEnabled(bool enabled);
    [[nodiscard]] virtual bool mangoHudEnabled() const;
    void setGamescopeEnabled(bool enabled);
    [[nodiscard]] virtual bool gamescopeEnabled() const;
    void setGamescopeArgs(const QString& args);
    [[nodiscard]] virtual QString gamescopeArgs() const;
    void setObsVkCaptureEnabled(bool enabled);
    [[nodiscard]] virtual bool obsVkCaptureEnabled() const;
    void setXaliaEnabled(bool enabled);
    [[nodiscard]] virtual bool xaliaEnabled() const;

    // Compatibility
    void setCtPath(const QString& path);
    [[nodiscard]] virtual QString ctPath() const;
    void setNvapiEnabled(bool enabled);
    [[nodiscard]] virtual bool nvapiEnabled() const;
    void setWaylandEnabled(bool enabled);
    [[nodiscard]] virtual bool waylandEnabled() const;
    void setHdrEnabled(bool enabled);
    [[nodiscard]] virtual bool hdrEnabled() const;
    void setWow64Enabled(bool enabled);
    [[nodiscard]] virtual bool wow64Enabled() const;
    void setSdlInputEnabled(bool enabled);
    [[nodiscard]] virtual bool sdlInputEnabled() const;
    void setOpenglEnabled(bool enabled);
    [[nodiscard]] virtual bool openglEnabled() const;

    // Plaform
    void setSteamEnabled(bool enabled);
    [[nodiscard]] virtual bool steamEnabled() const;
    void setSteamEnvEnabled(bool enabled);
    [[nodiscard]] virtual bool steamEnvEnabled() const;
    void setSteamOverlayEnabled(bool enabled);
    [[nodiscard]] virtual bool steamOverlayEnabled() const;
    void setOnlineFixEnabled(bool enabled);
    [[nodiscard]] virtual bool onlineFixEnabled() const;
};
}