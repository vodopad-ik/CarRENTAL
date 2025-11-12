

#include <memory>
#include "../../../src/CustomCalendarWidget.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CustomCalendarWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_CustomCalendarWidget_t {
    uint offsetsAndSizes[2];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CustomCalendarWidget_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_CustomCalendarWidget_t qt_meta_stringdata_CustomCalendarWidget = {
    {
        QT_MOC_LITERAL(0, 20)
    },
    "CustomCalendarWidget"
};
#undef QT_MOC_LITERAL
}

Q_CONSTINIT static const uint qt_meta_data_CustomCalendarWidget[] = {


      10,
       0,
       0,    0,
       0,    0,
       0,    0,
       0,    0,
       0,    0,
       0,
       0,

       0
};

Q_CONSTINIT const QMetaObject CustomCalendarWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QCalendarWidget::staticMetaObject>(),
    qt_meta_stringdata_CustomCalendarWidget.offsetsAndSizes,
    qt_meta_data_CustomCalendarWidget,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CustomCalendarWidget_t,

        QtPrivate::TypeAndForceComplete<CustomCalendarWidget, std::true_type>
    >,
    nullptr
} };

void CustomCalendarWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *CustomCalendarWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomCalendarWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomCalendarWidget.stringdata0))
        return static_cast<void*>(this);
    return QCalendarWidget::qt_metacast(_clname);
}

int CustomCalendarWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCalendarWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
