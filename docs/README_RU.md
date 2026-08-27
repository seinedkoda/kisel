# Кисель

Эффективный запуск Windows программ

![screenshot](../resources/banner-ru.png)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Flatpak](https://img.shields.io/badge/flatpak-%234A90D9.svg?style=for-the-badge&logo=flatpak&logoColor=white)
![Arch](https://img.shields.io/badge/Arch%20Linux-1793D1?logo=arch-linux&logoColor=fff&style=for-the-badge)
![Debian](https://img.shields.io/badge/Debian-D70A53?style=for-the-badge&logo=debian&logoColor=white)
![Fedora](https://img.shields.io/badge/Fedora-294172?style=for-the-badge&logo=fedora&logoColor=white)

## Цель проекта 🚩
Предоставить программу моментального запуска exe на компилируемом языке с минимальными зависимостями, не превращая её в ещё один лаунчер с добавлением exe через множество меню со своей специфичной игровой библиотекой. Просто выбрать exe и запуск. А для сохранения запуска exe файла используется встроенная функция создания ярлыков на рабочем столе

## Основное🍒
- Код на C++ и минимальные действия для запуска exe-файлов
- Запуск игр без Steam через [umu-launcher](https://github.com/Open-Wine-Components/umu-launcher)
- Совместимость со средой Steam
- Настройка и управление префиксами
- Меню загрузки средств совместимости
- Ярлыки для рабочего стола
- Установка дополнительных dll для префикса
- WOW64 по умолчанию и отсутствие 32-битных зависимостей
- Поддержка OnlineFix (Отключено по умолчанию)
- Доступные для загрузки средства совместимости:
    - [Proton-GE]([Proton-GE](https://github.com/GloriousEggroll/proton-ge-custom)) (по умолчанию)
    - [Proton-CachyOS]([Proton-CachyOS](https://github.com/CachyOS/proton-cachyos))
- Настройка следующих параметров:
    - Автоматическое обновление среды выполнения
    - Использование Steam или umu-launcher
    - MangoHud
    - OBS Vulkan Game Capture
    - Xalia
    - NVAPI
    - Драйвер Wayland
    - HDR
    - SDL Input
    - Использование среды Steam
    - WOW64
    - OnlineFix
    - И ещё некоторые...

## Установка 📦

### Flatpak (Универсальный)
<a href='https://flathub.org/apps/io.github.seinedkoda.kisel'>
    <img width='240' alt='Get it on Flathub' src='https://flathub.org/api/badge?locale=ru'/>
</a>

### Arch Linux
**[Загрузите последнюю версию .pacman](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo pacman -U kisel-*.pacman
```

### Debian
**[Загрузите последнюю версию .deb](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo apt install ./kisel-*.deb
```

### Fedora
**[Загрузите последнюю версию .rpm](https://github.com/seinedkoda/kisel/releases/latest)**
```bash
sudo dnf install ./kisel-*.rpm
```

### Сборка из исходного кода
- **Зависимости:**
  - qt6-base
  - icoutils
- **Зависимости для сборки:**
  - cmake
  - qt6-tools
- **Опциональные зависимости:**
  - winetricks
  - mangohud
  - obs-vkcapture

```bash
git clone https://github.com/seinedkoda/kisel.git
cd kisel
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

## Поддержать 🥴

**[Boosty](https://boosty.to/seinedk/donate)**

## Лицензия 📖
Данный проект распространяется под лицензией **GNU General Public License v3.0** (GPLv3).
Для получения дополнительной информации см. файл [LICENSE](LICENSE).

### Сторонние ресурсы

- **[Papirus Icon Theme](https://github.com/PapirusDevelopmentTeam/papirus-icon-theme)** - запасной набор иконок
  - **Автор:** Papirus Development Team
  - **Лицензия:** [GNU General Public License v3.0 (GPLv3)](resources/icons/thirdparty/Papirus/LICENSE)
  - **Изменения:** Изменён index.theme, убраны неиспользуемые иконки