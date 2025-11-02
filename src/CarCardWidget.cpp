#include "CarCardWidget.h"
#include "db/Database.h"
#include "utils/CurrencyConverter.h"
#include <QCoreApplication>
#include <QCursor>
#include <QDebug> // Added for qDebug
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

// Делает скруглённые углы у переданного изображения
static QPixmap makeRounded(const QPixmap &src, int radius) {
  if (src.isNull())
    return src;
  QPixmap dst(src.size());
  dst.fill(Qt::transparent);
  QPainter p(&dst);
  p.setRenderHint(QPainter::Antialiasing, true);
  QPainterPath path;
  path.addRoundedRect(src.rect(), radius, radius);
  p.setClipPath(path);
  p.drawPixmap(0, 0, src);
  p.end();
  return dst;
}

CarCardWidget::CarCardWidget(const CarInfo &car, const QString &currency,
                             QWidget *parent)
    : QWidget(parent), currentCurrency_(currency) {
  carData_ = new CarInfo(car);
  setMinimumSize(320, 360);
  setMaximumSize(360, 420);
  // Устанавливаем размерную политику, чтобы карточка не растягивалась по
  // горизонтали Maximum означает, что виджет может быть от минимального до
  // максимального размера, но не больше
  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
  setStyleSheet("CarCardWidget { background-color: white; border: none; "
                "border-radius: 10px; } "
                "CarCardWidget > QLabel { background-color: transparent; "
                "border: none; }");

  // Устанавливаем стандартный курсор для карточки
  setCursor(QCursor(Qt::ArrowCursor));

  auto *layout = new QVBoxLayout(this);
  layout->setSpacing(6);
  layout->setContentsMargins(12, 12, 12, 12);

  // Сначала изображение, затем название (фото выше названия)

  // Фотография (реальная)
  imageLabel_ = new QLabel(this);
  imageLabel_->setFixedHeight(180);
  imageLabel_->setAlignment(Qt::AlignCenter);
  imageLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; }");
  imageLabel_->setScaledContents(false);
  // тень для придания объема
  auto *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(18);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 80));
  imageLabel_->setGraphicsEffect(shadow);

  QPixmap pix;
  bool imageLoaded = false;

  // --- REWORK: строго ищем фото только по ключу, SVG и ресурсы не ищем ---
  // Получаем ключ: если imagePath типа :/images/cars/bmw_x5.svg, то берем
  // только bmw_x5
  QString key;
  if (!carData_->imagePath.isEmpty()) {
    key = carData_->imagePath;
    // Найти последний / и .
    int start = key.lastIndexOf('/') + 1;
    int end = key.lastIndexOf('.');
    if (start >= 0 && end > start)
      key = key.mid(start, end - start);
    else
      key = key.section('/', -1).section('.', 0, 0);
  }
  // Путь к папке с фотками
  QString binDir = QCoreApplication::applicationDirPath();
  // бинарник находится в build/bin; поднимаемся на уровень проекта
  QString basePhotos =
      QDir(binDir + "/../../resources/photos/cars/").absolutePath();
  QString absJpg = basePhotos + "/" + key + ".jpg";
  QString absJpeg = basePhotos + "/" + key + ".jpeg";
  QString absPng = basePhotos + "/" + key + ".png";

  qDebug() << "image key:" << key;
  qDebug() << "check paths:" << absJpg << QFile::exists(absJpg) << absJpeg
           << QFile::exists(absJpeg) << absPng << QFile::exists(absPng);

  if (QFile::exists(absJpg)) {
    imageLoaded = pix.load(absJpg);
  } else if (QFile::exists(absJpeg)) {
    imageLoaded = pix.load(absJpeg);
  } else if (QFile::exists(absPng)) {
    imageLoaded = pix.load(absPng);
  }

  // Если не найдено — placeholder
  if (!imageLoaded) {
    QString absPlaceholder =
        QDir(binDir + "/../../resources/images/placeholder.svg").absolutePath();
    pix.load(absPlaceholder);
  }

  if (!pix.isNull()) {
    // Масштаб под ширину карточки, чтобы фото не наезжало на текст
    const int targetW = 320;
    const int targetH = 180;
    QPixmap scaledFill =
        pix.scaled(targetW, targetH, Qt::KeepAspectRatioByExpanding,
                   Qt::SmoothTransformation);
    // обрезаем по центру
    int x = (scaledFill.width() - targetW) / 2;
    int y = (scaledFill.height() - targetH) / 2;
    QPixmap cropped = scaledFill.copy(x, y, targetW, targetH);
    // скругляем углы у самого изображения
    QPixmap rounded = makeRounded(cropped, 12);
    imageLabel_->setPixmap(rounded);
    imageLabel_->setText("");
  } else {
    imageLabel_->setText("🚗");
    imageLabel_->setStyleSheet(
        "QLabel { background-color: #e0e0e0; border: none; color: #999; "
        "font-size: 40px; }");
  }
  layout->addWidget(imageLabel_, 0, Qt::AlignHCenter);
  // Включаем отслеживание мыши для показа tooltip при наведении
  imageLabel_->setMouseTracking(true);
  imageLabel_->installEventFilter(this);
  setMouseTracking(true);

  // Название под фото
  QString carName = QString("%1 %2 (%3)")
                        .arg(carData_->brand, carData_->model,
                             QString::number(carData_->year));
  nameLabel_ = new QLabel(carName, this);
  nameLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding: 0px; "
      "font-size: 16px; font-weight: bold; color: #333; }");
  nameLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  nameLabel_->setWordWrap(false);
  nameLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(nameLabel_);

  priceLabel_ = new QLabel(this);
  updatePriceDisplay();
  priceLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding: 0px; "
      "font-size: 14px; color: #4CAF50; font-weight: bold; }");
  priceLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  priceLabel_->setWordWrap(false);
  priceLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(priceLabel_);

  // Характеристики убраны с карточки — оставляем только краткое описание ниже

  // Краткое описание под фото
  shortDescription_ = carData_->description;
  descLabel_ = new QLabel(shortDescription_, this);
  descLabel_->setWordWrap(true);
  descLabel_->setMaximumHeight(40); // Ограничиваем высоту для единообразия
  descLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  descLabel_->setStyleSheet(
      "QLabel { background-color: transparent; border: none; padding-top: 2px; "
      "font-size: 12px; color: #455A64; font-weight: 500; }");
  descLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  descLabel_->setTextFormat(Qt::PlainText);
  layout->addWidget(descLabel_);

  // Всплывающее окно с характеристиками - непрозрачное черное окно
  detailsPopup_ = new QFrame(this);
  detailsPopup_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
  detailsPopup_->setStyleSheet("QFrame { background-color: black; "
                               "border: none; border-radius: 8px; }");
  detailsPopup_->hide();
  detailsPopup_->setAttribute(Qt::WA_TransparentForMouseEvents);

  auto *popupLayout = new QVBoxLayout(detailsPopup_);
  popupLayout->setContentsMargins(12, 12, 12, 12);
  popupLayout->setSpacing(4);

  detailsLabel_ = new QLabel(detailsPopup_);
  detailsLabel_->setWordWrap(true);
  detailsLabel_->setStyleSheet(
      "QLabel { background: transparent; border: none; "
      "font-size: 12px; color: #E0E0E0; font-weight: 600; "
      "padding: 0px; margin: 0px; line-height: 1.4; }");
  detailsLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  popupLayout->addWidget(detailsLabel_);

  // Тень для объема
  auto *popupShadow = new QGraphicsDropShadowEffect(detailsPopup_);
  popupShadow->setBlurRadius(12);
  popupShadow->setOffset(0, 2);
  popupShadow->setColor(QColor(0, 0, 0, 100));
  detailsPopup_->setGraphicsEffect(popupShadow);

  // Фиксированный отступ вместо addStretch() для одинакового расстояния между
  // описанием и кнопками
  layout->addSpacing(10);

  // Горизонтальный layout для кнопок
  auto *btnLayout = new QHBoxLayout();
  btnLayout->setContentsMargins(
      0, -20, 0, 0); // Отрицательный верхний margin, чтобы поднять кнопки выше

  rentBtn_ = new QPushButton("Арендовать", this);
  rentBtn_->setStyleSheet(
      "QPushButton { background-color: #2196F3; color: white; "
      "padding: 10px 14px; border-radius: 6px; font-size: 14px; }"
      "QPushButton:hover { background-color: #1976D2; }"
      "QPushButton:pressed { transform: translateY(1px); }");
  rentBtn_->setMinimumHeight(38);
  btnLayout->addWidget(rentBtn_);

  // Кнопка закладок - заменена на иконку
  bookmarkBtn_ = new QToolButton(this);
  bookmarkBtn_->setText(carData_->bookmarked ? "★" : "☆");
  bookmarkBtn_->setToolTip(carData_->bookmarked ? "Убрать из закладок"
                                                : "В закладки");
  bookmarkBtn_->setStyleSheet(
      QString("QToolButton { background-color: %1; border: none; "
              "border-radius: 6px; padding: 6px; font-size: 20px; color: %2; }"
              "QToolButton:hover { filter: brightness(0.97); }"
              "QToolButton:pressed { transform: translateY(1px); }")
          .arg(carData_->bookmarked ? "#FFC107" : "#E0E0E0")
          .arg(carData_->bookmarked ? "#FF6F00" : "#757575"));
  bookmarkBtn_->setMinimumSize(38, 38);
  bookmarkBtn_->setMaximumSize(38, 38);
  btnLayout->addWidget(bookmarkBtn_);

  layout->addLayout(btnLayout);

  connect(rentBtn_, &QPushButton::clicked,
          [this]() { emit rentClicked(carData_->id); });

  connect(bookmarkBtn_, &QToolButton::clicked, [this]() {
    carData_->bookmarked = !carData_->bookmarked;
    bookmarkBtn_->setText(carData_->bookmarked ? "★" : "☆");
    bookmarkBtn_->setToolTip(carData_->bookmarked ? "Убрать из закладок"
                                                  : "В закладки");
    bookmarkBtn_->setStyleSheet(
        QString(
            "QToolButton { background-color: %1; border: none; "
            "border-radius: 6px; padding: 6px; font-size: 20px; color: %2; }"
            "QToolButton:hover { filter: brightness(0.97); }"
            "QToolButton:pressed { transform: translateY(1px); }")
            .arg(carData_->bookmarked ? "#FFC107" : "#E0E0E0")
            .arg(carData_->bookmarked ? "#FF6F00" : "#757575"));
    emit bookmarkToggled(carData_->id, carData_->bookmarked);
  });
}

void CarCardWidget::updateCurrency(const QString &currency) {
  currentCurrency_ = currency;
  updatePriceDisplay();
}

int CarCardWidget::carId() const { return carData_ ? carData_->id : -1; }

void CarCardWidget::updateBookmarkStatus(bool bookmarked) {
  if (!carData_)
    return;
  carData_->bookmarked = bookmarked;
  bookmarkBtn_->setText(bookmarked ? "★" : "☆");
  bookmarkBtn_->setToolTip(bookmarked ? "Убрать из закладок" : "В закладки");
  bookmarkBtn_->setStyleSheet(
      QString("QToolButton { background-color: %1; border: none; "
              "border-radius: 6px; padding: 6px; font-size: 20px; color: %2; }"
              "QToolButton:hover { filter: brightness(0.97); }"
              "QToolButton:pressed { transform: translateY(1px); }")
          .arg(bookmarked ? "#FFC107" : "#E0E0E0")
          .arg(bookmarked ? "#FF6F00" : "#757575"));
}

void CarCardWidget::updatePriceDisplay() {
  auto &converter = CurrencyConverter::instance();
  auto currency = CurrencyConverter::fromString(currentCurrency_);
  double price = converter.fromBase(carData_->pricePerDay, currency);
  QString symbol = converter.symbol(currency);

  // Форматируем цену правильно с разделителем тысяч и символом валюты
  QString priceText = QString::number(price, 'f', 2) + " " + symbol + "/день";
  priceLabel_->setText(priceText);
}

bool CarCardWidget::eventFilter(QObject *obj, QEvent *event) {
  if (obj == imageLabel_) {
    if (event->type() == QEvent::Enter) {
      showDetailsTooltip();
      return true;
    } else if (event->type() == QEvent::Leave) {
      hideDetailsTooltip();
      return true;
    }
  }
  return QWidget::eventFilter(obj, event);
}

void CarCardWidget::showDetailsTooltip() {
  // Формируем текст с подробными характеристиками
  QString detailsText;

  if (!carData_->engineType.isEmpty())
    detailsText += QString("Тип двигателя: %1\n").arg(carData_->engineType);
  if (carData_->engineCapacityL > 0.0)
    detailsText += QString("Объем: %1 л\n")
                       .arg(QString::number(carData_->engineCapacityL, 'f', 1));
  if (carData_->powerHp > 0)
    detailsText += QString("Мощность: %1 л.с.\n").arg(carData_->powerHp);
  if (carData_->seats > 0)
    detailsText += QString("Число мест: %1").arg(carData_->seats);

  // Если нет характеристик, показываем описание
  if (detailsText.isEmpty() && !carData_->description.isEmpty()) {
    detailsText = carData_->description;
  }

  // Проверяем, что есть текст для отображения
  if (detailsText.isEmpty()) {
    return;
  }

  // Устанавливаем текст
  detailsLabel_->setText(detailsText.trimmed());

  // Позиционируем окно справа от изображения
  int margin = 12;
  int cardWidth = width();

  // Получаем позицию и размеры изображения
  int imageX = imageLabel_->x();
  int imageY = imageLabel_->y();
  int imageWidth = imageLabel_->width();
  int imageHeight = imageLabel_->height();

  // Вычисляем размеры окна (увеличиваем)
  int popupWidth = 180; // Фиксированная ширина для читаемости
  detailsLabel_->setFixedWidth(popupWidth -
                               28); // padding 12px*2 + border 2px*2
  detailsLabel_->adjustSize();

  // Вычисляем высоту окна
  int labelHeight = detailsLabel_->height();
  int popupHeight = labelHeight + 32; // padding 12px*2 + spacing

  // Позиционируем справа от изображения в правом нижнем углу
  int popupX =
      imageX + imageWidth + 8; // Справа от изображения с небольшим отступом

  // Позиционируем в правом нижнем углу изображения
  int popupY = imageY + imageHeight - popupHeight -
               8; // Внизу изображения с небольшим отступом

  // Если окно не помещается в карточку, позиционируем его внутри справа
  if (popupX + popupWidth > cardWidth - margin) {
    popupX = cardWidth - popupWidth - margin; // Внутри карточки справа
  }

  // Если окно выше изображения, выравниваем по нижнему краю изображения
  if (popupY < imageY) {
    popupY = imageY + imageHeight - popupHeight - 5;
  }

  detailsPopup_->setGeometry(popupX, popupY, popupWidth, popupHeight);
  detailsPopup_->raise();
  detailsPopup_->show();
  detailsPopup_
      ->update(); // Принудительное обновление для корректного отображения
}

void CarCardWidget::hideDetailsTooltip() {
  // Скрываем всплывающее окно
  detailsPopup_->hide();
}
