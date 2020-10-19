#ifndef DOCKWIDGETLIST_H
#define DOCKWIDGETLIST_H

#include <QMainWindow>
#include <QList>

class QDockWidget;
class QAction;
class QMenu;

class DockWidgetList : public QObject
{
    Q_OBJECT
public:
    explicit DockWidgetList(QMainWindow *parent);

    void registerMenu(QMenu *menu, QAction *before);

private slots:
    void onViewActionToggled(bool visible);
    void onWidgetAdded(const QString &id);
    void onWidgetRemoved(const QString &id);
    void onWidgetUpdated(const QString &id);

private:
    QMainWindow *m_mw;
    QList<QDockWidget *> m_dockWidgetList;
    QAction *m_beforeAction = nullptr;
    QMenu *m_menu;
};

#endif // DOCKWIDGETLIST_H
