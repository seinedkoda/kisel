# Kisel

Efficient launch of Windows programs

*Translations*: **[Русский](docs/README_RU.md)**

![screenshot](resources/screenshot.png)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Arch](https://img.shields.io/badge/Arch%20Linux-1793D1?logo=arch-linux&logoColor=fff&style=for-the-badge)
![Debian](https://img.shields.io/badge/Debian-D70A53?style=for-the-badge&logo=debian&logoColor=white)
![Fedora](https://img.shields.io/badge/Fedora-294172?style=for-the-badge&logo=fedora&logoColor=white)

## Highlights 🍒
- It can run games without Steam using [umu-launcher](https://github.com/Open-Wine-Components/umu-launcher), and with it
- Compatibility with the Steam environment
- C++ code and minimal actions to run exe
- Ability to create desktop shortcuts instead of displaying the list of added applications
- Managing Prefixes and Compatibility Tools (Proton)
- Installing additional prefix dlls
- OnlineFix Support (Disabled by default)
- Support for downloading compatibility tools:
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

### Arch Linux
**[Download the latest version of pkg.tar.zst](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo pacman -U kisel-*.pkg.tar.zst
```

### Debian
**[Download the latest version of deb](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo apt install ./kisel-*.deb
```

### Fedora
**[Download the latest version of rpm](https://github.com/seinedkoda/kisel/releases/latest)**
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