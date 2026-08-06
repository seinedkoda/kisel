# Кисель

Эффективный запуск Windows программ

![screenshot](../resources/screenshot_ru.png)

## Основные возможности
- Возможность запуска игр без Steam с помощью [umu-launcher](https://github.com/Open-Wine-Components/umu-launcher), а также с ним
- Совместимость со средой Steam
- Код на C++ и минимальные действия для запуска exe-файлов
- Возможность создания ярлыков на рабочем столе вместо отображения списка добавленных приложений
- Управление префиксами и инструментами совместимости (Proton)
- Установка дополнительных DLL-файлов в префиксы
- Поддержка OnlineFix (по умолчанию отключено)
- Поддержка загрузки инструментов совместимости:
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

## Зависимости
- **qt6-base**: Графический интерфейс
- **[umu-launcher](https://github.com/Open-Wine-Components/umu-launcher)**: Способ запуска приложений Windows без Steam
- **icoutils**: Извлечение значков
- ### По желанию:
    - **winetricks**: Дополнительные скрипты для работы с префиксом Wine
    - **mangohud**: Мониторинга FPS, температуры, загрузки ЦП/ГП поверх игры
    - **obs-vkcapture**: Плагин для захвата игр на Vulkan для OBS

## Установка

### Arch Linux
[Скачать последнюю версию pkg.tar.zst](https://github.com/seinedkoda/kisel/releases/latest)

```bash
sudo pacman -U kisel-*.pkg.tar.zst
```

### Сборка из исходного кода с использованием CMake

*Требуется Qt6 LinguistTools (qt6-tools)*

```bash
git clone https://github.com/seinedkoda/kisel.git
cd kisel
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

## Лицензия
Данный проект распространяется под лицензией **GNU General Public License v3.0** (GPLv3).
Для получения дополнительной информации см. файл [LICENSE](LICENSE).

### Сторонние ресурсы

- **[Papirus Icon Theme](https://github.com/PapirusDevelopmentTeam/papirus-icon-theme)** - запасной набор иконок
  - **Автор:** Papirus Development Team
  - **Лицензия:** [GNU General Public License v3.0 (GPLv3)](resources/icons/thirdparty/Papirus/LICENSE)
  - **Изменения:** Изменён index.theme, убраны неиспользуемые иконки