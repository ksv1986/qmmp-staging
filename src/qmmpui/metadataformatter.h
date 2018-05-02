/***************************************************************************
 *   Copyright (C) 2015-2018 by Ilya Kotov                                 *
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

#ifndef METADATAFORMATTER_H
#define METADATAFORMATTER_H

#include <QString>
#include <QMap>
#include <QList>
#include <qmmpui/playlisttrack.h>
#include <qmmp/qmmp.h>
#include "qmmpui_export.h"

/*! @brief The MetaDataFormatter formats metadata using templates.
 * @author Ilya Kotov <forkotov02@ya.ru>
 */
class QMMPUI_EXPORT MetaDataFormatter
{
public:
    /*!
     * Constructor.
     * @param pattern Metadata template.
     * Syntax:
     * %p - artist,
     * %a - album,
     * %aa - album artist,
     * %t - title,
     * %n - track number,
     * %NN - 2-digit track number,
     * %g - genre,
     * %c - comment,
     * %C - composer,
     * %D - disc number,
     * %f - file name,
     * %F - full path,
     * %y - year,
     * %l - duration,
     * %I - track index,
     * %if(A,B,C) or %if(A&B&C,D,E) - condition,
     * %dir(n) - Name of the directory located on \b n levels above.
     */
    MetaDataFormatter(const QString &pattern = QString());
    /*!
     * Setups metadata template.
     * @param pattern Metadata template string.
     */
    void setPattern(const QString &pattern);
    /*!
     * Returns metadata template.
     */
    const QString pattern() const;
    /*!
     * Converts metadata of item \b item to one string using template.
     */
    QString format(const PlayListTrack *item) const;
    /*!
     * Converts metadata of \b TrackInfo pointer \b info to one string using template.
     * \param info pointer to \b TrackInfo object.
     * \param track Index of track.
     */
    QString format(const TrackInfo &info, int trackIndex = 0) const;
    /*!
     * Returns formatted duration (example: 05:02:03).
     * \param duration Duration in milliseconds.
     * \param hideZero Setting for zero values output.
     * If \b hideZero is \b true, then the function outputs empty string for zero length,
     * otherwise outputs "0:00".
     */
    static QString formatDuration(qint64 duration, bool hideZero = true, bool showMs = false);

private:
    struct Node;
    struct Param;

    struct Node
    {
        enum {
            PRINT_TEXT = 0,
            IF_KEYWORD,
            OR_OPERATOR,
            AND_OPERATOR,
            DIR_FUNCTION
        } command;

        QList<Param> params;
    };

    struct Param
    {
        enum {
            FIELD = 0,
            TEXT,
            NUMERIC,
            NODES
        } type;

        //extra fields
        enum
        {
            PATH = Qmmp::DISCNUMBER + 1,
            TWO_DIGIT_TRACK,
            DURATION,
            FILE_NAME,
            TRACK_INDEX
        };

        int field;
        QString text;
        int number;
        QList<Node> children;
    };

    bool parseField(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end);
    bool parseIf(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end);
    bool parseDir(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end);
    void parseText(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end);
    void parseEscape(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end);

    QString evalute(const QList<Node> *nodes, const TrackInfo *info, int trackIndex) const;
    QString printParam(Param *p, const TrackInfo *info, int trackIndex) const;
    QString printField(int field, const TrackInfo *info, int trackIndex) const;

    QString dumpNode(Node node) const;

    QList<MetaDataFormatter::Node> compile(const QString &expr);
    QString m_pattern;
    QList<MetaDataFormatter::Node> m_nodes;
    QMap<QString, int> m_fieldNames;
};

#endif // METADATAFORMATTER2_H
