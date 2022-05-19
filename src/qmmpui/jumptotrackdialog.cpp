/***************************************************************************
 *   Copyright (C) 2007-2022 by Ilya Kotov                                 *
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

#include <QAction>
#include <QEvent>
#include <QKeyEvent>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QShortcut>
#include <QKeySequence>
#include <QStyledItemDelegate>
#include <QtDebug>
#include <QPainter>
#include <qmmp/soundcore.h>
#include "playlistmanager.h"
#include "mediaplayer.h"
#include "jumptotrackdialog_p.h"

class TrackItemDelegate : public QStyledItemDelegate
{
public:
    explicit TrackItemDelegate(QObject *parent): QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QString extraText = index.data(JumpToTrackDialog::QueueRole).toString();

        if(!extraText.isEmpty())
        {
            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);
            QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
            int spacing = opt.fontMetrics.maxWidth();
            int extraTextWidth = opt.fontMetrics.horizontalAdvance(extraText);

            opt.rect.setWidth(opt.rect.width() - extraTextWidth - spacing);
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

            opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
            opt.text = extraText;
            opt.rect.setWidth(opt.rect.width() + extraTextWidth + spacing);
            opt.rect.setX(opt.rect.x() + (opt.rect.width() - extraTextWidth) - spacing);
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        }
        else
        {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }
};

JumpToTrackDialog::JumpToTrackDialog(PlayListModel *model, QWidget* parent)
        : QDialog (parent)
{
    setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    m_model = model;
    m_pl_manager = PlayListManager::instance();
    m_listModel = new TrackListModel(m_model, this);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSourceModel(m_listModel);
    m_proxyModel->setSortLocaleAware(true);
    songsListView->setItemDelegate(new TrackItemDelegate(this));
    songsListView->setModel(m_proxyModel);

    connect(songsListView,SIGNAL(doubleClicked(QModelIndex)),SLOT(jumpTo(QModelIndex)));
    connect(songsListView,SIGNAL(doubleClicked(QModelIndex)),SLOT(accept()));
    connect(songsListView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(queueUnqueue(QModelIndex, QModelIndex)));

    connect(m_model, SIGNAL(destroyed()), SLOT(close()));

    new QShortcut(tr("Q"), this, SLOT(on_queuePushButton_clicked()));
    new QShortcut(tr("J"), this, SLOT(on_jumpToPushButton_clicked()));

    filterLineEdit->installEventFilter(this);
    songsListView->installEventFilter(this);
    connect(filterLineEdit, SIGNAL(textChanged(QString)), m_proxyModel, SLOT(setFilterFixedString(QString)));
}

JumpToTrackDialog::~JumpToTrackDialog()
{
}

void JumpToTrackDialog::on_queuePushButton_clicked()
{
    QModelIndexList selectedRows = songsListView->selectionModel()->selectedRows();
    if (!selectedRows.isEmpty())
    {
        int selected = (m_proxyModel->mapToSource(selectedRows.first())).row();
        PlayListTrack *track = m_model->findTrack(selected);
        m_model->setQueued(track);

        if(track->isQueued())
            queuePushButton->setText(tr("Unqueue"));
        else
            queuePushButton->setText(tr("Queue"));
    }
}

void JumpToTrackDialog::on_jumpToPushButton_clicked()
{
    QModelIndexList mi_list = songsListView->selectionModel()->selectedRows();
    if (!mi_list.isEmpty())
    {
        jumpTo(mi_list.at(0));
    }
}

void JumpToTrackDialog::jumpTo(const QModelIndex & index)
{
    int selected = (m_proxyModel->mapToSource(index)).row();
    PlayListTrack *track = m_model->findTrack(selected);
    m_model->setCurrent(track);
    SoundCore::instance()->stop();
    m_pl_manager->activatePlayList(m_model);
    MediaPlayer::instance()->play();
}

void JumpToTrackDialog::queueUnqueue(const QModelIndex& curr,const QModelIndex&)
{
    if(!curr.isValid())
        return;
    int row = m_proxyModel->mapToSource(curr).row();
    if (m_model->findTrack(row)->isQueued())
        queuePushButton->setText(tr("Unqueue"));
    else
        queuePushButton->setText(tr("Queue"));
}

bool JumpToTrackDialog::eventFilter(QObject *o, QEvent *e)
{
    if(o == filterLineEdit && e->type() == QEvent::KeyPress)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(e);
        QModelIndex index = songsListView->currentIndex();
        bool select_first = false;
        if(!index.isValid() && m_proxyModel->rowCount())
        {
            select_first = true;
            index = m_proxyModel->index(0,0);
        }

        if(key_event->key() == Qt::Key_Up)
        {
            if(!select_first)
                index = m_proxyModel->index(index.row() - 1, index.column());
            if(index.isValid())
                songsListView->setCurrentIndex(index);
            return true;
        }
        else if(key_event->key() == Qt::Key_Down)
        {
            if(!select_first)
                index = m_proxyModel->index(index.row() + 1, index.column());
            if(index.isValid())
                songsListView->setCurrentIndex(index);
            return true;
        }
        else if(key_event->key() == Qt::Key_Return)
        {
            if(index.isValid())
            {
                jumpTo(index);
                accept();
            }
            return true;
        }
    }
    else if(o == songsListView && e->type() == QEvent::KeyPress)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(e);
        QModelIndex index = songsListView->currentIndex();

        if(key_event->key() == Qt::Key_Return)
        {
            if(index.isValid())
            {
                jumpTo(index);
                accept();
            }
            return true;
        }
    }
    return QDialog::eventFilter(o, e);
}

///TrackListModel
TrackListModel::TrackListModel(PlayListModel *model, QObject *parent) : QAbstractListModel(parent), m_model(model)
{
    m_queue = QSet<PlayListTrack *>(m_model->queuedTracks().cbegin(), m_model->queuedTracks().cend());
    connect(m_model, SIGNAL(listChanged(int)), SLOT(onListChanged(int)));
}

QVariant TrackListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        PlayListTrack *track = m_model->findTrack(index.row());
        QString title = track->value(Qmmp::TITLE);
        QString artist = track->value(Qmmp::ARTIST);

        if(title.isEmpty()) //using file name if title is empty
        {
            title = track->path().section('/',-1);
            title = title.left(title.lastIndexOf('.'));
        }
        if(!artist.isEmpty())
            title.prepend(artist + QStringLiteral(" - "));

        return title;
    }
    else if(role == JumpToTrackDialog::QueueRole)
    {
        PlayListTrack *track = m_model->findTrack(index.row());
        if(track->isQueued())
            return QStringLiteral("[%1]").arg(track->queuedIndex() + 1);
        else
            return QVariant();
    }

    return QVariant();
}

int TrackListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_model->trackCount();
}

void TrackListModel::onListChanged(int flags)
{
    if(flags & PlayListModel::STRUCTURE)
    {
        beginResetModel();
        m_queue = QSet<PlayListTrack *>(m_model->queuedTracks().cbegin(), m_model->queuedTracks().cend());
        endResetModel();
    }
    else if(flags & PlayListModel::QUEUE)
    {
        QSet<PlayListTrack *> changed = m_queue;
        m_queue = QSet<PlayListTrack *>(m_model->queuedTracks().cbegin(), m_model->queuedTracks().cend());
        changed.unite(m_queue);

        for(PlayListTrack *t : qAsConst(changed))
            emit dataChanged(index(t->trackIndex() ,0), index(t->trackIndex(), 0), { JumpToTrackDialog::QueueRole } );
    }
}
