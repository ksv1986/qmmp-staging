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
        connect(dockWidget->toggleViewAction(), SIGNAL(triggered(bool)), SLOT(onViewActionTriggered(bool)));
        connect(dockWidget, SIGNAL(visibilityChanged(bool)), SLOT(onVisibilityChanged(bool)));
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

void DockWidgetList::setTitleBarsVisible(bool visible)
{
    m_titleBarsVisible = visible;

    if(visible)
    {
        for(QDockWidget *w : qAsConst(m_dockWidgetList))
        {
            QWidget *widget = w->titleBarWidget();
            if(widget)
            {
                w->setTitleBarWidget(nullptr);
                delete widget;
            }
        }
    }
    else
    {
        for(QDockWidget *w : qAsConst(m_dockWidgetList))
        {
            if(!w->titleBarWidget())
                w->setTitleBarWidget(new QWidget());
        }
    }
}

void DockWidgetList::onViewActionTriggered(bool checked)
{
    if(!sender() || !sender()->parent())
        return;

    QDockWidget *dockWidget = qobject_cast<QDockWidget *>(sender()->parent());

    if(!dockWidget)
        return;

    QString id = dockWidget->objectName();

    if(checked && !dockWidget->widget())
    {
        QWidget *w = General::createWidget(id, m_mw);
        if(w)
        {
            dockWidget->setWidget(w);
            w->show();
        }
    }
    else if(!checked && dockWidget->widget())
    {
        dockWidget->widget()->deleteLater();
    }
}

void DockWidgetList::onVisibilityChanged(bool visible)
{
    QDockWidget *dockWidget = qobject_cast<QDockWidget *>(sender());

    if(!dockWidget)
        return;

    QString id = dockWidget->objectName();

    if(visible && !dockWidget->widget())
    {
        QWidget *w = General::createWidget(id, m_mw);
        if(w)
        {
            dockWidget->setWidget(w);
            w->show();
        }
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
    connect(dockWidget->toggleViewAction(), SIGNAL(toggled(bool)), SLOT(onViewActionTriggered(bool)));
    m_dockWidgetList << dockWidget;
    setTitleBarsVisible(m_titleBarsVisible);

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
