[Read in English](README-en.md)

# TicketCounter

Кроссплатформенное приложение для учёта выполненных тикетов (задач) с графическим интерфейсом на GTK4. Позволяет отслеживать количество тикетов за день, ставить ежедневные цели, просматривать статистику и графики выполненных задач. Поддерживает настройку времени сброса счётчика, часовых поясов, экспорт/импорт данных в JSON. Приложение автоматически сохраняет данные и восстанавливает их при запуске.

## Требования

- C++17+
- GTK4 (gtkmm4)
- CMake 3.16+
- nlohmann::json

## Сборка на Linux

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
./TicketCounter
```

## Сборка на Windows (MSVC)

1. Установите Visual Studio 2019+ с компонентами "Desktop development with C++".
2. Скачайте и установите MSYS2: https://www.msys2.org/
3. В MSYS2: `pacman -S mingw-w64-x86_64-gtk4 mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake`
4. Установите nlohmann/json через vcpkg или вручную.
5. `mkdir build`
6. `cd build`
7. `cmake -G "MinGW Makefiles" ..`
8. `mingw32-make`
9. Запустите TicketCounter.exe

Альтернативно с vcpkg:
- `vcpkg install gtkmm nlohmann-json`
- `cmake -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..`
- `cmake --build . --config Release`

## Структура проекта

- src/: Исходный код
- include/: Заголовочные файлы
- tests/: Тесты
- data/: Данные JSON