#pragma once

#include <QSettings>

namespace kisel {
class PrefixSettings : public QSettings {
public:
    explicit PrefixSettings(const QString& path, QObject* parent = nullptr);

    void setCtPath(const QString& path);
    [[nodiscard]] QString ctPath() const;
    void setMangoHudEnabled(bool enabled);
    [[nodiscard]] bool mangoHudEnabled() const;
    void setObsVkCaptureEnabled(bool enabled);
    [[nodiscard]] bool obsVkCaptureEnabled() const;
    void setXaliaEnabled(bool enabled);
    [[nodiscard]] bool xaliaEnabled() const;
    void setWaylandEnabled(bool enabled);
    [[nodiscard]] bool waylandEnabled() const;
    void setSteamEnabled(bool enabled);
    [[nodiscard]] bool steamEnabled() const;
    void setWow64Enabled(bool enabled);
    [[nodiscard]] bool wow64Enabled() const;
};
}