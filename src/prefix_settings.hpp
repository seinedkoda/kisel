#pragma once

#include <QSettings>

namespace kisel {
class PrefixSettings : public QSettings {
public:
    explicit PrefixSettings(const QString& path, QObject* parent = nullptr);

    void setCtPath(const QString& path);
    [[nodiscard]] QString ctPath() const;

    // Services
    void setMangoHudEnabled(bool enabled);
    [[nodiscard]] bool mangoHudEnabled() const;
    void setObsVkCaptureEnabled(bool enabled);
    [[nodiscard]] bool obsVkCaptureEnabled() const;
    void setXaliaEnabled(bool enabled);
    [[nodiscard]] bool xaliaEnabled() const;

    // Compatibility
    void setNvapiEnabled(bool enabled);
    [[nodiscard]] bool nvapiEnabled() const;
    void setWaylandEnabled(bool enabled);
    [[nodiscard]] bool waylandEnabled() const;
    void setHdrEnabled(bool enabled);
    [[nodiscard]] bool hdrEnabled() const;
    void setWow64Enabled(bool enabled);
    [[nodiscard]] bool wow64Enabled() const;
    void setSdlInputEnabled(bool enabled);
    [[nodiscard]] bool sdlInputEnabled() const;
    void setOpenglEnabled(bool enabled);
    [[nodiscard]] bool openglEnabled() const;

    // Using Steam
    void setSteamEnabled(bool enabled);
    [[nodiscard]] bool steamEnabled() const;
    void setSteamEnvEnabled(bool enabled);
    [[nodiscard]] bool steamEnvEnabled() const;
    void setOnlineFixEnabled(bool enabled);
    [[nodiscard]] bool onlineFixEnabled() const;
};
}