# Kisel

Efficient launch of Windows programs

*Translations*: **[Русский](docs/README_RU.md)**

![screenshot](resources/banner.png)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Flatpak](https://img.shields.io/badge/flatpak-%234A90D9.svg?style=for-the-badge&logo=flatpak&logoColor=white)
![Arch](https://img.shields.io/badge/Arch%20Linux-1793D1?logo=arch-linux&logoColor=fff&style=for-the-badge)
![Debian](https://img.shields.io/badge/Debian-D70A53?style=for-the-badge&logo=debian&logoColor=white)
![Fedora](https://img.shields.io/badge/Fedora-294172?style=for-the-badge&logo=fedora&logoColor=white)

## Project goal 🚩
Provide an instant-run exe file program in a compiled language with minimal dependencies, without turning it into yet another launcher with multiple menus for adding a exe with its own specific game library. Just select the exe and run. And to save the exe file launcher, use the built-in desktop shortcut feature

## Highlights 🍒
- C++ code and minimal actions to run exe
- Launching games without Steam using [umu-launcher](https://github.com/Open-Wine-Components/umu-launcher)
- Compatibility with the Steam environment
- Configuring and managing prefixes
- Compatibility Tools Download Menu
- Desktop shortcuts
- Installing additional prefix dlls
- WOW64 by default and no 32-bit dependencies
- OnlineFix Support (Disabled by default)
- Supported compatibility tools for download:
    - [Proton-GE](https://github.com/GloriousEggroll/proton-ge-custom) (Default)
    - [Proton-CachyOS](https://github.com/CachyOS/proton-cachyos)
- Configure the following parameters:
    - Runtime auto-update
    - Using Steam or umu-launcher
    - MangoHud
    - OBS Vulkan Game Capture
    - Xalia
    - NVAPI
    - Wayland driver
    - HDR
    - SDL Input
    - Steam Environment
    - WOW64
    - OnlineFix
    - And some others...

## Installation 📦

### Flatpak (Universal)
<a href='https://flathub.org/apps/io.github.seinedkoda.kisel'>
    <img width='240' alt='Get it on Flathub' src='https://flathub.org/api/badge?locale=en'/>
</a>

### Arch Linux
**[Download the latest version of .pacman](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo pacman -U kisel-*.pacman
```

### Debian
**[Download the latest version of .deb](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo apt install ./kisel-*.deb
```

### Fedora
**[Download the latest version of .rpm](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo dnf install ./kisel-*.rpm
```

### Build from source
- **Dependencies:**
  - qt6-base
  - icoutils
- **Make dependencies:**
  - cmake
  - qt6-tools
- **Optional dependencies:**
  - winetricks
  - mangohud
  - obs-vkcapture

```bash
git clone https://github.com/seinedkoda/kisel.git
cd kisel
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

## Support 🥴

**[Boosty](https://boosty.to/seinedk/donate)**

## License 📖
This project is licensed under the **GNU General Public License v3.0** (GPLv3).
For more information, see the file [LICENSE](LICENSE).

### Third-Party Resources

- **[Papirus Icon Theme](https://github.com/PapirusDevelopmentTeam/papirus-icon-theme)** - fallback icons theme
  - **Author:** Papirus Development Team
  - **License:** [GNU General Public License v3.0 (GPLv3)](resources/icons/thirdparty/Papirus/LICENSE)
  - **Changes:** Changed index.theme, removed unused icons