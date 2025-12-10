# Послойная структура MainWindow

## 📐 Общая иерархия виджетов

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           QMainWindow                                    │
│                         (MainWindow)                                     │
│  ┌───────────────────────────────────────────────────────────────────┐  │
│  │                    QWidget (centralWidget_)                        │  │
│  │                    QVBoxLayout (mainLayout)                        │  │
│  │  ┌─────────────────────────────────────────────────────────────┐  │  │
│  │  │  СЛОЙ 1: HEADER (QWidget)                                    │  │  │
│  │  │  ┌───────────────────────────────────────────────────────┐  │  │  │
│  │  │  │  QHBoxLayout (headerLayout)                           │  │  │  │
│  │  │  │  ┌──────────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌────┐ │  │  │  │
│  │  │  │  │QLabel    │  │Stretch│  │QPush │  │QCombo│  │QPush│ │  │  │  │
│  │  │  │  │welcome   │  │       │  │Button│  │Box   │  │Button│ │  │  │  │
│  │  │  │  │Label_    │  │       │  │myRent│  │curren│  │logout│ │  │  │  │
│  │  │  │  │          │  │       │  │alsBtn│  │cyBox_│  │Btn_  │ │  │  │  │
│  │  │  │  └──────────┘  └──────┘  └──────┘  └──────┘  └────┘ │  │  │  │
│  │  │  └───────────────────────────────────────────────────────┘  │  │  │
│  │  └─────────────────────────────────────────────────────────────┘  │  │
│  │  ┌─────────────────────────────────────────────────────────────┐  │  │
│  │  │  СЛОЙ 2: TABS (QTabWidget)                                   │  │  │
│  │  │  ┌───────────────────────────────────────────────────────┐  │  │  │
│  │  │  │  ВКЛАДКА 1: "Автомобили" (QWidget - carsTab)          │  │  │  │
│  │  │  │  ┌─────────────────────────────────────────────────┐  │  │  │  │
│  │  │  │  │  QVBoxLayout (carsLayout)                       │  │  │  │  │
│  │  │  │  │  ┌───────────────────────────────────────────┐  │  │  │  │  │
│  │  │  │  │  │  СЛОЙ 2.1: ФИЛЬТРЫ (QHBoxLayout)          │  │  │  │  │  │
│  │  │  │  │  │  ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐     │  │  │  │  │  │
│  │  │  │  │  │  │QLine│ │QCom│ │QSpin│ │QSpin│ │QDoub│     │  │  │  │  │  │
│  │  │  │  │  │  │Edit │ │boBo│ │Box  │ │Box  │ │leSpin│     │  │  │  │  │  │
│  │  │  │  │  │  │searc│ │x   │ │seats│ │power│ │Box   │     │  │  │  │  │  │
│  │  │  │  │  │  │hEdit│ │engin│ │Min  │ │Min  │ │capaci│     │  │  │  │  │  │
│  │  │  │  │  │  │_    │ │eType│ │Filter│ │Filter│ │tyMin │     │  │  │  │  │  │
│  │  │  │  │  │  └────┘ └────┘ └────┘ └────┘ └────┘     │  │  │  │  │  │
│  │  │  │  │  └───────────────────────────────────────────┘  │  │  │  │  │
│  │  │  │  │  ┌───────────────────────────────────────────┐  │  │  │  │  │
│  │  │  │  │  │  СЛОЙ 2.2: SCROLL AREA (QScrollArea)      │  │  │  │  │  │
│  │  │  │  │  │  ┌─────────────────────────────────────┐  │  │  │  │  │  │
│  │  │  │  │  │  │  QWidget (carsContainer_)           │  │  │  │  │  │  │
│  │  │  │  │  │  │  ┌───────────────────────────────┐  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  QGridLayout (gridLayout)     │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  ┌────┐ ┌────┐ ┌────┐        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  │Card│ │Card│ │Card│  ...    │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  │1   │ │2   │ │3   │        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  └────┘ └────┘ └────┘        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  ┌────┐ ┌────┐ ┌────┐        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  │Card│ │Card│ │Card│  ...    │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  │4   │ │5   │ │6   │        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  └────┘ └────┘ └────┘        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  └───────────────────────────────┘  │  │  │  │  │  │  │
│  │  │  │  │  │  └─────────────────────────────────────┘  │  │  │  │  │  │
│  │  │  │  │  └───────────────────────────────────────────┘  │  │  │  │  │
│  │  │  │  └─────────────────────────────────────────────────┘  │  │  │  │
│  │  │  └───────────────────────────────────────────────────────┘  │  │  │
│  │  │  ┌───────────────────────────────────────────────────────┐  │  │  │
│  │  │  │  ВКЛАДКА 2: "Закладки" (QWidget - bookmarksContainer_)│  │  │  │
│  │  │  │  ┌─────────────────────────────────────────────────┐  │  │  │  │
│  │  │  │  │  QVBoxLayout (bmLayoutOuter)                    │  │  │  │  │
│  │  │  │  │  ┌───────────────────────────────────────────┐  │  │  │  │  │
│  │  │  │  │  │  QScrollArea (bmScroll)                   │  │  │  │  │  │
│  │  │  │  │  │  ┌─────────────────────────────────────┐  │  │  │  │  │  │
│  │  │  │  │  │  │  QWidget (bmInner_)                 │  │  │  │  │  │  │
│  │  │  │  │  │  │  ┌───────────────────────────────┐  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  QGridLayout (bmGrid)         │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  ┌────┐ ┌────┐ ┌────┐        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  │Card│ │Card│ │Card│  ...    │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  │  └────┘ └────┘ └────┘        │  │  │  │  │  │  │  │
│  │  │  │  │  │  │  └───────────────────────────────┘  │  │  │  │  │  │  │
│  │  │  │  │  │  └─────────────────────────────────────┘  │  │  │  │  │  │
│  │  │  │  │  └───────────────────────────────────────────┘  │  │  │  │  │
│  │  │  │  └─────────────────────────────────────────────────┘  │  │  │  │
│  │  │  └───────────────────────────────────────────────────────┘  │  │  │
│  │  │  ┌───────────────────────────────────────────────────────┐  │  │  │
│  │  │  │  ВКЛАДКА 3: "Мои аренды" (QWidget - rentalsWidget_)   │  │  │  │
│  │  │  │  ┌─────────────────────────────────────────────────┐  │  │  │  │
│  │  │  │  │  QVBoxLayout (rentalsLayout)                    │  │  │  │  │
│  │  │  │  │  ┌───────────────────────────────────────────┐  │  │  │  │  │
│  │  │  │  │  │  QTableView (rentalsTable_)               │  │  │  │  │  │
│  │  │  │  │  │  (использует RentalsModel)                │  │  │  │  │  │
│  │  │  │  │  └───────────────────────────────────────────┘  │  │  │  │  │
│  │  │  │  └─────────────────────────────────────────────────┘  │  │  │  │
│  │  │  └───────────────────────────────────────────────────────┘  │  │  │
│  │  └───────────────────────────────────────────────────────┘  │  │  │
│  └─────────────────────────────────────────────────────────────┘  │  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 🔍 Детальное описание каждого слоя

### **СЛОЙ 0: QMainWindow (корневой контейнер)**

**Файл:** `MainWindow.cpp:31-33`
```cpp
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Аренда автомобилей");
  resize(1200, 800);
```

**Роль:**
- Главное окно приложения
- Размер: 1200x800 пикселей
- Содержит центральный виджет (`centralWidget_`)

**Связи:**
- `setCentralWidget(ui_.centralWidget_)` - устанавливает центральный виджет

---

### **СЛОЙ 1: HEADER (Верхняя панель)**

**Файл:** `MainWindow.cpp:72-114`

**Структура:**
```
QWidget (header)
  └─ QHBoxLayout (headerLayout)
      ├─ QLabel (welcomeLabel_)          // "Добро пожаловать, Имя!"
      ├─ addStretch()                    // Растягивающийся пробел
      ├─ QPushButton (myRentalsBtn_)     // "Мои аренды"
      ├─ QComboBox (currencyBox_)        // Выбор валюты (USD/EUR/BYN)
      └─ QPushButton (logoutBtn_)        // "Выйти"
```

**Стилизация:**
- Фон: `#2196F3` (синий)
- Padding: `15px`
- Кнопки: белый фон, синий текст, скруглённые углы

**Связи:**
- `headerLayout->addWidget(header)` → добавляется в `mainLayout`
- Сигналы:
  - `currencyBox_->currentTextChanged` → `onCurrencyChanged()`
  - `myRentalsBtn_->clicked` → `onShowMyRentals()`
  - `logoutBtn_->clicked` → `onLogout()`

**Код создания:**
```cpp
auto *header = new QWidget(this);
header->setStyleSheet("background-color: #2196F3; padding: 15px;");
auto *headerLayout = new QHBoxLayout(header);
// ... добавление элементов ...
mainLayout->addWidget(header);  // Добавляется в главный layout
```

---

### **СЛОЙ 2: TABS (Вкладки)**

**Файл:** `MainWindow.cpp:116-209`

**Структура:**
```
QTabWidget (tabs_)
  ├─ Вкладка 1: "Автомобили" (carsTab)
  ├─ Вкладка 2: "Закладки" (bookmarksContainer_)
  └─ Вкладка 3: "Мои аренды" (rentalsWidget_)
```

**Связи:**
- `mainLayout->addWidget(ui_.tabs_)` → добавляется в главный layout
- Сигнал: `tabs_->currentChanged` → загрузка данных при переключении

---

### **СЛОЙ 2.1: ФИЛЬТРЫ (Вкладка "Автомобили")**

**Файл:** `MainWindow.cpp:122-159`

**Структура:**
```
QHBoxLayout (searchLayout)
  ├─ QLineEdit (searchEdit_)              // Поиск по марке/модели
  ├─ QComboBox (engineTypeFilter_)        // Тип двигателя (Любой/EV/Hybrid/ICE)
  ├─ QSpinBox (seatsMinFilter_)           // Минимум мест (≥)
  ├─ QSpinBox (powerMinFilter_)           // Минимум лошадиных сил (≥)
  └─ QDoubleSpinBox (capacityMinFilter_)  // Минимум объёма двигателя (≥)
```

**Роль:**
- Фильтрация списка автомобилей
- Все фильтры подключены к `connectFilters()` → `loadCars()`

**Связи:**
- `carsLayout->addLayout(searchLayout)` → добавляется в layout вкладки
- Сигналы (все вызывают `applyFilters()`):
  - `searchEdit_->textChanged`
  - `engineTypeFilter_->currentTextChanged`
  - `seatsMinFilter_->valueChanged`
  - `powerMinFilter_->valueChanged`
  - `capacityMinFilter_->valueChanged`

**Код:**
```cpp
auto *searchLayout = new QHBoxLayout();
filters_.searchEdit_ = new QLineEdit();
filters_.searchEdit_->setPlaceholderText("Поиск по марке, модели...");
// ... остальные фильтры ...
searchLayout->addWidget(filters_.searchEdit_);
// ...
carsLayout->addLayout(searchLayout);
```

---

### **СЛОЙ 2.2: SCROLL AREA + GRID (Вкладка "Автомобили")**

**Файл:** `MainWindow.cpp:161-178`

**Структура:**
```
QScrollArea (scrollArea_)
  └─ QWidget (carsContainer_)
      └─ QGridLayout (gridLayout)
          └─ CarCardWidget (создаются динамически через carsView_)
```

**Роль:**
- Прокручиваемая область для карточек автомобилей
- Сетка 3 колонки (320px каждая)
- Отступы: 20px

**Связи:**
- `carsLayout->addWidget(ui_.scrollArea_)` → добавляется в layout вкладки
- `scrollArea_->setWidget(carsContainer_)` → устанавливает контейнер
- `carsView_` (CarCardsView) управляет карточками в `gridLayout`

**Код:**
```cpp
ui_.scrollArea_ = new QScrollArea();
ui_.scrollArea_->setWidgetResizable(true);
ui_.carsContainer_ = new QWidget();
auto *gridLayout = new QGridLayout(ui_.carsContainer_);
gridLayout->setSpacing(20);
gridLayout->setContentsMargins(20, 20, 20, 20);
ui_.scrollArea_->setWidget(ui_.carsContainer_);
carsLayout->addWidget(ui_.scrollArea_);
```

**CarCardsView:**
```cpp
carsView_ = std::make_unique<CarCardsView>(ui_.carsContainer_, 3);
// carsView_ управляет добавлением CarCardWidget в gridLayout
```

---

### **СЛОЙ 2.3: ЗАКЛАДКИ (Вкладка "Закладки")**

**Файл:** `MainWindow.cpp:182-200`

**Структура:**
```
QWidget (bookmarksContainer_)
  └─ QVBoxLayout (bmLayoutOuter)
      └─ QScrollArea (bmScroll)
          └─ QWidget (bmInner_)
              └─ QGridLayout (bmGrid)
                  └─ CarCardWidget (создаются через bookmarksView_)
```

**Роль:**
- Отображение закладок пользователя
- Аналогична структуре вкладки "Автомобили"

**Связи:**
- `tabs_->addTab(bookmarksContainer_, "Закладки")`
- `bookmarksView_` управляет карточками в `bmGrid`

**Код:**
```cpp
ui_.bookmarksContainer_ = new QWidget();
auto *bmLayoutOuter = new QVBoxLayout(ui_.bookmarksContainer_);
auto *bmScroll = new QScrollArea();
ui_.bmInner_ = new QWidget();
auto *bmGrid = new QGridLayout(ui_.bmInner_);
bmScroll->setWidget(ui_.bmInner_);
bmLayoutOuter->addWidget(bmScroll);
ui_.tabs_->addTab(ui_.bookmarksContainer_, "Закладки");
```

---

### **СЛОЙ 2.4: АРЕНДЫ (Вкладка "Мои аренды")**

**Файл:** `MainWindow.cpp:202-207`

**Структура:**
```
QWidget (rentalsWidget_)
  └─ QVBoxLayout (rentalsLayout)
      └─ QTableView (rentalsTable_)
          └─ RentalsModel (rentalsModel_)  // Модель данных
```

**Роль:**
- Таблица с историей аренд
- Использует Model/View архитектуру Qt

**Связи:**
- `tabs_->addTab(rentalsWidget_, "Мои аренды")`
- `rentalsTable_->setModel(rentalsModel_)` → связь с моделью
- Модель обновляется через `rentalsModel_->refresh(customerId_)`

**Код:**
```cpp
ui_.rentalsWidget_ = new QWidget();
auto *rentalsLayout = new QVBoxLayout(ui_.rentalsWidget_);
rentalsTable_ = new QTableView();
rentalsLayout->addWidget(rentalsTable_);
ui_.tabs_->addTab(ui_.rentalsWidget_, "Мои аренды");
```

---

## 🔗 Связи между компонентами

### **1. Управление данными:**

```
Database::instance()
    ↓
CarsCatalogController (catalogController_)
    ↓
CarCardsView (carsView_ / bookmarksView_)
    ↓
CarCardWidget (создаются динамически)
```

### **2. Управление валютой:**

```
QComboBox (currencyBox_)
    ↓ (сигнал: currentTextChanged)
MainWindow::onCurrencyChanged()
    ↓
updateCurrencyForAllCards()
    ├─ carsView_->setCurrency()
    ├─ bookmarksView_->setCurrency()
    └─ rentalsModel_->setCurrency()
```

### **3. Фильтрация:**

```
Фильтры (searchEdit_, engineTypeFilter_, ...)
    ↓ (сигналы: textChanged, valueChanged)
connectFilters() → applyFilters()
    ↓
currentFilters() → CatalogFilters
    ↓
catalogController_->setFilters()
    ↓
loadCars() / loadBookmarks()
```

### **4. События карточек:**

```
CarCardWidget
    ├─ rentClicked → MainWindow::onCarRentClicked()
    │                   ↓
    │                   RentalDialog
    │                   ↓
    │                   loadCars() + onShowMyRentals()
    │
    └─ bookmarkToggled → MainWindow::onBookmarkToggled()
                            ↓
                            Database::setBookmarked()
                            ↓
                            loadBookmarks()
```

---

## 📊 Последовательность создания (setupUI)

```
1. Создание centralWidget_ и mainLayout
   └─ setCentralWidget(ui_.centralWidget_)

2. Создание HEADER
   ├─ QWidget (header)
   ├─ QHBoxLayout (headerLayout)
   ├─ QLabel (welcomeLabel_)
   ├─ addStretch()
   ├─ QPushButton (myRentalsBtn_)
   ├─ QComboBox (currencyBox_)
   └─ QPushButton (logoutBtn_)
   └─ mainLayout->addWidget(header)

3. Создание TABS
   └─ QTabWidget (tabs_)

4. Создание ВКЛАДКИ 1: "Автомобили"
   ├─ QWidget (carsTab)
   ├─ QVBoxLayout (carsLayout)
   ├─ QHBoxLayout (searchLayout) + фильтры
   ├─ QScrollArea (scrollArea_)
   │   └─ QWidget (carsContainer_)
   │       └─ QGridLayout (gridLayout)
   └─ tabs_->addTab(carsTab, "Автомобили")

5. Создание ВКЛАДКИ 2: "Закладки"
   ├─ QWidget (bookmarksContainer_)
   ├─ QVBoxLayout (bmLayoutOuter)
   ├─ QScrollArea (bmScroll)
   │   └─ QWidget (bmInner_)
   │       └─ QGridLayout (bmGrid)
   └─ tabs_->addTab(bookmarksContainer_, "Закладки")

6. Создание ВКЛАДКИ 3: "Мои аренды"
   ├─ QWidget (rentalsWidget_)
   ├─ QVBoxLayout (rentalsLayout)
   ├─ QTableView (rentalsTable_)
   └─ tabs_->addTab(rentalsWidget_, "Мои аренды")

7. Добавление TABS в mainLayout
   └─ mainLayout->addWidget(ui_.tabs_)

8. Создание View-компонентов
   ├─ carsView_ = CarCardsView(ui_.carsContainer_, 3)
   └─ bookmarksView_ = CarCardsView(ui_.bmInner_, 3)

9. Подключение сигналов
   ├─ currencyBox_ → onCurrencyChanged()
   ├─ myRentalsBtn_ → onShowMyRentals()
   ├─ tabs_ → loadBookmarks() / onShowMyRentals()
   ├─ logoutBtn_ → onLogout()
   └─ connectFilters() → loadCars()
```

---

## 🎨 Стилизация слоёв

### **HEADER:**
- Фон: `#2196F3` (синий)
- Padding: `15px`
- Кнопки: белый фон, синий текст

### **ФИЛЬТРЫ:**
- `QLineEdit`: рамка `#e0e0e0`, при фокусе `#2196F3`
- `QComboBox` / `QSpinBox`: высота `38px`

### **SCROLL AREA:**
- Фон: `#f5f5f5` (светло-серый)
- Без рамки

### **GRID LAYOUT:**
- Отступы: `20px` со всех сторон
- Расстояние между элементами: `20px`
- 3 колонки по `320px` каждая

---

## 🔄 Потоки данных

### **Загрузка автомобилей:**
```
loadCars()
    ↓
catalogController_->setFilters(currentFilters())
    ↓
catalogController_->loadAvailable(customerId_)
    ↓
QList<CarInfo>
    ↓
carsView_->showCars(cars, binder)
    ↓
Создание CarCardWidget в gridLayout
    ↓
Подключение сигналов (rentClicked, bookmarkToggled)
```

### **Обновление валюты:**
```
Пользователь выбирает валюту в currencyBox_
    ↓
currentTextChanged("USD"/"EUR"/"BYN")
    ↓
onCurrencyChanged()
    ↓
currentCurrency_ = currencyBox_->currentText()
    ↓
updateCurrencyForAllCards()
    ├─ carsView_->setCurrency()
    │   └─ Для каждой карточки: card->updateCurrency()
    ├─ bookmarksView_->setCurrency()
    │   └─ Для каждой карточки: card->updateCurrency()
    └─ rentalsModel_->setCurrency()
        └─ emit dataChanged() → обновление таблицы
```

---

## 📝 Ключевые моменты

1. **Иерархия Layout:**
   - `QVBoxLayout` (mainLayout) - вертикальный главный layout
   - `QHBoxLayout` (headerLayout) - горизонтальный layout для header
   - `QGridLayout` (gridLayout, bmGrid) - сетка для карточек

2. **ScrollArea:**
   - `setWidgetResizable(true)` - автоматическое изменение размера содержимого
   - Содержит `QWidget` с `QGridLayout` внутри

3. **Динамическое создание:**
   - `CarCardWidget` создаются через `CarCardsView::showCars()`
   - Не создаются в `setupUI()`, а добавляются при загрузке данных

4. **Model/View:**
   - `RentalsModel` наследуется от `QSqlQueryModel`
   - `QTableView` отображает данные через модель

5. **Управление памятью:**
   - `carsView_` и `bookmarksView_` - `std::unique_ptr`
   - `rentalsModel_` - обычный указатель (удаляется в деструкторе)

---

## 🎯 Резюме структуры

```
MainWindow
├─ Header (фиксированная панель сверху)
│   ├─ Приветствие
│   ├─ Кнопки управления
│   └─ Выбор валюты
│
└─ Tabs (переключаемые вкладки)
    ├─ Автомобили
    │   ├─ Фильтры (поиск, тип двигателя, параметры)
    │   └─ Сетка карточек (прокручиваемая)
    │
    ├─ Закладки
    │   └─ Сетка карточек (прокручиваемая)
    │
    └─ Мои аренды
        └─ Таблица (Model/View)
```





