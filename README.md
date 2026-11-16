# SES — Social Economic Simulator

Автоматический экономический симулятор сообщества людей на ограниченной карте.  
Automatic economic simulator of a human community on a bounded map.

**Стек / Stack:** C++17 + SDL2 (окно с сеткой). Не Godot.  
**Агенты / Agents:** пешки на карте (автономные люди), не чат-боты.

---

## Что показывает окно / What the window shows

- 2D карта-сетка (клеточный фон)
- Зелёная клетка — ресурс (еда)
- Цветные точки — пешки (агенты). Каждый тик они делают шаг: чаще к ресурсу, иногда случайно. Когда пешка достигает ресурса, ресурс перемещается.

Закрыть окно: `Esc`, `Q` или кнопка закрытия.

---

## Требования / Requirements

- CMake ≥ 3.16
- Компилятор C++17 (g++ или clang++)
- SDL2 (библиотека + заголовки)

### Linux (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libsdl2-dev
```

### Windows (кратко)

1. Установите [CMake](https://cmake.org/download/) и компилятор (MSVC через Visual Studio, или MinGW).
2. Установите SDL2 development libraries (например через [vcpkg](https://vcpkg.io/): `vcpkg install sdl2`).
3. При сборке укажите путь к SDL2, например:
   ```bat
   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[путь-к-vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build build
   ```

---

## Сборка и запуск / Build & run

Из корня репозитория:

```bash
cmake -S . -B build
cmake --build build
./build/ses
```

Если нет графического дисплея (SSH / CI), можно хотя бы проверить сборку:

```bash
cmake -S . -B build && cmake --build build
```

Для короткого smoke-теста с виртуальным дисплеем (Linux):

```bash
sudo apt install -y xvfb
xvfb-run -a ./build/ses
```
(процесс будет крутиться, пока вы его не остановите — это нормально для smoke-теста)

---

## Структура / Layout

```
SES/
  CMakeLists.txt   # сборка
  src/main.cpp     # окно SDL2, карта, пешки
  README.md
```

Один главный цикл, без тяжёлых фреймворков — удобная точка старта для экономики, потребностей и правил мира.
