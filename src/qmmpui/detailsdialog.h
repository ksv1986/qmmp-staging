/***************************************************************************
 *   Copyright (C) 2009-2021 by Ilya Kotov                                 *
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
#ifndef DETAILSDIALOG_H
#define DETAILSDIALOG_H

#include <QDialog>
#include <QList>
#include <QSet>
#include <qmmp/qmmp.h>
#include <qmmp/trackinfo.h>
#include "qmmpui_export.h"

class QTextCodec;
class QAbstractButton;
class PlayListTrack;
class MetaDataModel;
class MetaDataItem;

namespace Ui {
    class DetailsDialog;
}

/** @brief The DetailsDialog class provides dialog to show/edit metadata.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT DetailsDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     * Constructor.
     * @param tracks a list of tracks which should be used.
     * @param parent Parent widget.
     */
    explicit DetailsDialog(const QList<PlayListTrack *> &tracks, QWidget *parent = nullptr);
    /*!
     * Destructor.
     */
    ~DetailsDialog();

    QStringList modifiedPaths() const;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_tabWidget_currentChanged(int index);
    void on_directoryButton_clicked();
    void on_prevButton_clicked();
    void on_nextButton_clicked();

private:
    Ui::DetailsDialog *m_ui;
    void closeEvent(QCloseEvent *) override;
    void updatePage();
    void printInfo();
    QString formatRow(const QString &key, const QString &value) const;
    QString formatRow(const MetaDataItem &item) const;
    MetaDataModel *m_metaDataModel = nullptr;
    QList<PlayListTrack *> m_tracks;
    TrackInfo m_info;
    int m_page = 0;
    QSet<QString> m_modifiedPaths;
};

#endif
