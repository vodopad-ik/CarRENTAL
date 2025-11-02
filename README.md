# CarRental (C++/Qt Desktop App)

Приложение для аренды автомобилей на C++ и Qt (Widgets + SQLite).

## Возможности

- Система регистрации и входа пользователей
- Просмотр доступных автомобилей с фильтрацией
- Добавление автомобилей в избранное
- Бронирование автомобилей на выбранные даты
- Просмотр истории аренд
- Конвертация валют (USD, EUR, BYN)
- Календарь с визуализацией забронированных дат

## Требования (Linux)

- CMake >= 3.21
- Компилятор C++17 (gcc/clang)
- Qt6 dev packages (Widgets, Sql)

Установка на Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools
```

Если Qt6 недоступен, можно использовать Qt5 (CMake автоматически переключится):

```bash
sudo apt install -y qtbase5-dev
```

## Сборка

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

Исполняемый файл будет находиться в `build/bin/CarRental`.

## Запуск

```bash
cd build
./bin/CarRental
```

База данных SQLite создаётся автоматически в `~/.carrental/app.db` при первом запуске.

## Структура проекта

- `src/` — исходный код C++
  - `MainWindow.*` — главное окно приложения
  - `LoginDialog.*` — диалог входа/регистрации
  - `RentalDialog.*` — диалог бронирования
  - `CarCardWidget.*` — виджет карточки автомобиля
  - `CustomCalendarWidget.*` — кастомный календарь
  - `db/Database.*` — работа с базой данных
  - `utils/CurrencyConverter.*` — конвертер валют
- `resources/` — ресурсы (изображения автомобилей)
- `CMakeLists.txt` — конфигурация сборки
