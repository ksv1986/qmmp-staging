#include <QString>
#include <QList>
#include <QtGlobal>
#include <QWidget>
#include "generalfactory.h"

QWidget *GeneralFactory::createWidget(int, QWidget *)
{
    return nullptr;
}
