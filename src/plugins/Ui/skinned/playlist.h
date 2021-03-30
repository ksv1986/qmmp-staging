/***************************************************************************
 *   Copyright (C) 2006-2021 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QPointer>

class QMenu;
class Skin;
class ListWidget;
class PlayListItem;
class Button;
class PlayListModel;
class PlayListTitleBar;
class PlayListSlider;
class MainWindow;
class SymbolDisplay;
class OutputState;
class PixmapWidget;
class PlaylistControl;
class KeyboardManager;
class PlayListManager;
class PlayListBrowser;
class PlayListSelector;
class QmmpUiSettings;

/**
   @author Ilya Kotov <forkotov02@ya.ru>
*/
class PlayList : public QWidget
{
    Q_OBJECT
public:
    explicit PlayList(PlayListManager *manager, QWidget *parent = nullptr);
    virtual ~PlayList();

    void setMinimalMode(bool b = true);
    void writeSettings();

#ifdef QMMP_WS_X11
    bool useCompiz() const;
#endif

signals:
    void play();
    void next();
    void prev();
    void pause();
    void stop();
    void eject();
    void loadPlaylist();
    void savePlaylist();
    void closed();

public slots:
    void setTime(qint64 time);
    void readSettings();

private slots:
    void showAddMenu();
    void showSubMenu();
    void showSelectMenu();
    void showSortMenu();
    void showPlaylistMenu();
    void updateSkin();
    void deletePlaylist();
    void renamePlaylist();
    void showPlayLists();
    void generateCopySelectedMenu();
    void copySelectedMenuActionTriggered(QAction *action);
    void onCurrentPlayListChanged(PlayListModel *current, PlayListModel *previous);
    void onListChanged(int flags);

private:
    void updatePositions();
    QString formatTime (int sec);
    void drawPixmap (QPainter *painter, int x, int y, const QPixmap &pix);
    void createMenus();
    void createActions();
    //events
    void paintEvent (QPaintEvent *) override;
    void resizeEvent (QResizeEvent *) override;
    void mouseMoveEvent (QMouseEvent *) override;
    void mousePressEvent (QMouseEvent *) override;
    void mouseReleaseEvent (QMouseEvent *) override;
    void changeEvent (QEvent*) override;
    void closeEvent (QCloseEvent*) override;
    void keyPressEvent (QKeyEvent*) override;
#ifdef QMMP_WS_X11
    bool event (QEvent *event) override;
#endif
    QMenu *m_addMenu;
    QMenu *m_subMenu;
    QMenu *m_selectMenu;
    QMenu *m_sortMenu;
    QMenu *m_playlistMenu;
    QMenu *m_copySelectedMenu;
    QWidget *m_resizeWidget;
    Button *m_buttonAdd;
    Button *m_buttonSub;
    Button *m_selectButton;
    Button *m_sortButton;
    Button* m_playlistButton;
    PlaylistControl* m_pl_control;
    SymbolDisplay* m_length_totalLength;
    SymbolDisplay* m_current_time;
    Skin *m_skin;
    ListWidget *m_listWidget;
    PlayListTitleBar *m_titleBar;
    PlayListSlider *m_plslider;
    bool m_resize = false;
    bool m_update = false;
    int m_ratio;
    int m_height;
    bool m_shaded = false;
    PlayListManager *m_pl_manager;
    QmmpUiSettings *m_ui_settings;
    KeyboardManager* m_keyboardManager;
    QPointer <PlayListBrowser> m_pl_browser;
    PlayListSelector *m_pl_selector = nullptr;
#ifdef QMMP_WS_X11
    bool m_compiz;
#endif
};

#endif
