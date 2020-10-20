#include <QDockWidget>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <qmmpui/general.h>
#include <qmmpui/uihelper.h>
#include "dockwidgetlist.h"

DockWidgetList::DockWidgetList(QMainWindow *parent) : QObject(parent), m_mw(parent)
{
    connect(UiHelper::instance(), SIGNAL(widgetAdded(QString)), SLOT(onWidgetAdded(QString)));
    connect(UiHelper::instance(), SIGNAL(widgetRemoved(QString)), SLOT(onWidgetRemoved(QString)));
    connect(UiHelper::instance(), SIGNAL(widgetUpdated(QString)), SLOT(onWidgetUpdated(QString)));

    for(const QString &id : General::enabledWidgets())
    {
        WidgetDescription desc = General::widgetDescription(id);
        QDockWidget *dockWidget = new QDockWidget(desc.name, m_mw);
        dockWidget->setObjectName(id);
        dockWidget->setAllowedAreas(desc.allowedAreas);
        m_mw->addDockWidget(desc.area, dockWidget);
        connect(dockWidget->toggleViewAction(), SIGNAL(toggled(bool)), SLOT(onViewActionToggled(bool)));
        m_dockWidgetList << dockWidget;
    }
}

void DockWidgetList::registerMenu(QMenu *menu, QAction *before)
{
    m_menu = menu;
    m_beforeAction = before;

    for(QDockWidget *dock : qAsConst(m_dockWidgetList))
        menu->insertAction(m_beforeAction, dock->toggleViewAction());
}

void DockWidgetList::onViewActionToggled(bool checked)
{
    if(!sender() || !sender()->parent())
        return;

    QString id = sender()->parent()->objectName();
    QDockWidget *dockWidget = qobject_cast<QDockWidget *>(sender()->parent());
    if(!dockWidget)
        return;

    if(checked)
    {
        QWidget *w = General::createWidget(id, m_mw);
        if(w)
        {
            dockWidget->setWidget(w);
            w->show();
        }
    }
    else if(dockWidget->widget())
    {
        dockWidget->widget()->deleteLater();
    }
}

void DockWidgetList::onWidgetAdded(const QString &id)
{
    for(QDockWidget *dockWidget : m_dockWidgetList)
    {
        if(dockWidget->objectName() == id)
            return;
    }

    WidgetDescription desc = General::widgetDescription(id);
    QDockWidget *dockWidget = new QDockWidget(desc.name, m_mw);
    dockWidget->setObjectName(id);
    dockWidget->setAllowedAreas(desc.allowedAreas);
    if(m_menu && m_beforeAction)
        m_menu->insertAction(m_beforeAction, dockWidget->toggleViewAction());
    m_mw->addDockWidget(desc.area, dockWidget);
    connect(dockWidget->toggleViewAction(), SIGNAL(toggled(bool)), SLOT(onViewActionToggled(bool)));
    m_dockWidgetList << dockWidget;

    QWidget *w = General::createWidget(id, m_mw);
    dockWidget->setWidget(w);
    w->show();
}

void DockWidgetList::onWidgetRemoved(const QString &id)
{
    for(QDockWidget *dockWidget : m_dockWidgetList)
    {
        if(dockWidget->objectName() == id)
        {
            m_dockWidgetList.removeAll(dockWidget);
            if(dockWidget->widget())
                dockWidget->widget()->deleteLater();
            dockWidget->deleteLater();
        }
    }
}

void DockWidgetList::onWidgetUpdated(const QString &id)
{
    for(QDockWidget *dockWidget : m_dockWidgetList)
    {
        if(dockWidget->objectName() == id && dockWidget->widget())
        {
            dockWidget->widget()->deleteLater();
            QWidget *w = General::createWidget(id, m_mw);
            if(w)
            {
                dockWidget->setWidget(w);
                w->show();
            }
            break;
        }
    }
}
