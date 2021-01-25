/***************************************************************************
 *   Copyright (C) 2015-2021 by Ilya Kotov                                 *
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


/*
Syntax:
%p - artist,
%a - album,
%aa - album artist,
%t - title,
%n - track number,
%NN - 2-digit track number,
%I - track index,
%g - genre,
%c - comment,
%C - composer,
%D - disc number,
%f - file name,
%F - full path,
%y - year,
%l - duration,
%I - track index,
%{bitrate} - bitrate,
%{samplerate} - sample rate,
%{channels} - number of channels,
%{samplesize} - bits per sample,
%{format} - format name,
%{decoder} - decoder name,
%{filesize} - file size,
%if(A,B,C) or %if(A&B&C,D,E) - condition,
%dir(n) - name of the directory located on n levels above,
%dir - full path of the parent directory.
*/

#include <QStringList>
#include <QUrl>
#include "metadataformatter.h"

#define DUMP_NODES

MetaDataFormatter::MetaDataFormatter(const QString &pattern)
{
    m_fieldNames.insert("t", Qmmp::TITLE);
    m_fieldNames.insert("p", Qmmp::ARTIST);
    m_fieldNames.insert("aa", Qmmp::ALBUMARTIST);
    m_fieldNames.insert("a", Qmmp::ALBUM);
    m_fieldNames.insert("c", Qmmp::COMMENT);
    m_fieldNames.insert("g", Qmmp::GENRE);
    m_fieldNames.insert("C", Qmmp::COMPOSER);
    m_fieldNames.insert("y", Qmmp::YEAR);
    m_fieldNames.insert("n", Qmmp::TRACK);
    m_fieldNames.insert("D", Qmmp::DISCNUMBER);
    m_fieldNames.insert("F", Param::PATH);
    m_fieldNames.insert("NN", Param::TWO_DIGIT_TRACK);
    m_fieldNames.insert("l", Param::DURATION);
    m_fieldNames.insert("f", Param::FILE_NAME);
    m_fieldNames.insert("I", Param::TRACK_INDEX);
    m_propertyNames.insert("bitrate", Qmmp::BITRATE);
    m_propertyNames.insert("samplerate", Qmmp::SAMPLERATE);
    m_propertyNames.insert("channels", Qmmp::CHANNELS);
    m_propertyNames.insert("samplesize", Qmmp::BITS_PER_SAMPLE);
    m_propertyNames.insert("format", Qmmp::FORMAT_NAME);
    m_propertyNames.insert("decoder", Qmmp::DECODER);
    m_propertyNames.insert("filesize", Qmmp::FILE_SIZE);

    if(!pattern.isEmpty())
        setPattern(pattern);
}

void MetaDataFormatter::setPattern(const QString &pattern)
{
    if(pattern == m_pattern)
        return;

    m_pattern = pattern;
    m_nodes.clear();

#ifdef DUMP_NODES
    qDebug("MetaDataFormatter: pattern: %s", qPrintable(pattern));
#endif
    m_nodes = compile(pattern);
#ifdef DUMP_NODES
    qDebug("MetaDataFormatter: dump of nodes");
    for(const Node &n : qAsConst(m_nodes))
    {
        qDebug("=>%s", qPrintable(dumpNode(n)));
    }
    qDebug("MetaDataFormatter: end of dump");
#endif
}

const QString MetaDataFormatter::pattern() const
{
    return m_pattern;
}

QString MetaDataFormatter::format(const PlayListTrack *item) const
{
    return evalute(&m_nodes, item, item->trackIndex());
}

QString MetaDataFormatter::format(const TrackInfo &info, int trackIndex) const
{
    return evalute(&m_nodes, &info, trackIndex);
}

QString MetaDataFormatter::format(const TrackInfo *info, int trackIndex) const
{
    return format(*info, trackIndex);
}

QString MetaDataFormatter::formatDuration(qint64 duration, bool hideZero, bool showMs)
{
    if(duration <= 0)
    {
        if(hideZero)
            return QString();
        else
            return showMs ? QLatin1String("0:00.000") : QLatin1String("0:00");
    }

    QString out;
    qint64 durationInSeconds = duration / 1000;
    if(durationInSeconds >= 3600)
        out = QString("%1:%2").arg(durationInSeconds / 3600).arg(durationInSeconds % 3600 / 60, 2, 10, QChar('0'));
    else
        out = QString("%1").arg(durationInSeconds % 3600 / 60);
    out += QString(":%1").arg(durationInSeconds % 60, 2, 10, QChar('0'));
    if(showMs)
        out += QString(".%1").arg(duration % 1000, 3, 10, QChar('0'));
    return out;
}

bool MetaDataFormatter::parseField(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    QString fieldName;
    int field = Qmmp::UNKNOWN;

    //try to find field with 2 symbols
    if((*i) + 1 != end)
    {
        fieldName.append((**i));
        fieldName.append(*((*i)+1));
        field = m_fieldNames.value(fieldName, Qmmp::UNKNOWN);
    }
    //try to find field with 1 symbol
    if(field == Qmmp::UNKNOWN)
    {
        fieldName.clear();
        fieldName.append((**i));
        field = m_fieldNames.value(fieldName, Qmmp::UNKNOWN);
    }

    if(field != Qmmp::UNKNOWN)
    {
        Node node;
        node.command = Node::PRINT_TEXT;
        Param param;
        param.type = Param::FIELD;
        param.field = field;
        node.params.append(param);
        nodes->append(node);
        (*i) += fieldName.size() - 1;
        return true;
    }
    return false;
}

bool MetaDataFormatter::parseProperty(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    if((*i) + 1 == end || (*i) + 2 == end)
        return false;

    if((**i) != QChar('{'))
        return false;

    (*i)++; //skip '{'

    QString propertyName;

    while((*i) != end && (**i) != QChar('}'))
    {
        propertyName.append((**i));
        (*i)++;
    }

    int field = m_propertyNames.value(propertyName, Qmmp::UNKNOWN);
    if(field != Qmmp::UNKNOWN)
    {
        Node node;
        node.command = Node::PRINT_TEXT;
        Param param;
        param.type = Param::PROPERTY;
        param.field = field;
        node.params.append(param);
        nodes->append(node);
        return true;
    }
    return false;
}

bool MetaDataFormatter::parseIf(QList<MetaDataFormatter::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    if((*i) + 1 == end || (*i) + 2 == end)
        return false;

    if((**i) != QChar('i') || *((*i)+1) != QChar('f'))
        return false;

    (*i)+=2;

    Node node;
    node.command = Node::IF_KEYWORD;

    int brackets_tracker = 0;
    QString var1, var2, var3;

    enum {
        STARTING = 0,
        READING_VAR1,
        READING_VAR2,
        READING_VAR3,
        FINISHED,

    } state = STARTING;

    bool escaped = false;

    while((*i) != end)
    {
        if((**i) == QChar('\\'))
        {
            (*i)++;
            escaped = true;
            continue;
        }

        if(escaped) //ignore escaped brackets
        {
            escaped = false;
        }
        else
        {
            if((**i) == QChar('('))
            {
                brackets_tracker++;
                if(state == STARTING)
                {
                    state = READING_VAR1;
                    (*i)++;
                    continue;
                }
            }
            else if((**i) == QChar(')'))
                brackets_tracker--;
        }

        switch (state)
        {
        case STARTING:
        {
            break;
        }
        case READING_VAR1:
        {
            if((**i) == QChar(',') && brackets_tracker == 1)
            {
                state = READING_VAR2;
                break;
            }
            var1.append((**i));
            break;
        }
        case READING_VAR2:
        {
            if((**i) == QChar(',') && brackets_tracker == 1)
            {
                state = READING_VAR3;
                break;
            }
            var2.append((**i));
            break;
        }
        case READING_VAR3:
        {
            if((**i) == QChar(')') && brackets_tracker == 0)
            {
                state = FINISHED;
                break;
            }
            var3.append((**i));
            break;
        }
        default:
            break;
        }

        if(state == FINISHED)
            break;

        (*i)++;
    }

    if(state != FINISHED)
    {
        qWarning("MetaDataFormatter: syntax error");
        return false;
    }

    Param param1, param2, param3;
    param1.type = Param::NODES, param2.type = Param::NODES, param3.type = Param::NODES;
    param1.children = compile(var1);
    param2.children = compile(var2);
    param3.children = compile(var3);
    node.params << param1 << param2 << param3;
    nodes->append(node);
    return true;
}

bool MetaDataFormatter::parseDir(QList<MetaDataFormatter::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    if((*i) + 1 == end || (*i) + 2 == end)
        return false;

    if((**i) != QChar('d') || *((*i)+1) != QChar('i') || *((*i)+2) != QChar('r'))
        return false;

    (*i)+=3;

    Node node;
    node.command = Node::DIR_FUNCTION;

    if((**i) == end || (**i) != QChar('(')) // %dir without params
    {
        (*i)--;
        nodes->append(node);
        return true;
    }

    QString var;

    enum {
        STARTING = 0,
        READING_VAR,
        FINISHED,

    } state = STARTING;

    while((*i) != end)
    {
        if((**i) == QChar('(') && state == STARTING)
        {
            state = READING_VAR;
            (*i)++;
            continue;
        }

        switch (state)
        {
        case STARTING:
        {
            break;
        }
        case READING_VAR:
        {
            if((**i) == QChar(')'))
            {
                state = FINISHED;
                break;
            }
            var.append((**i));
            break;
        }
        default:
            break;
        }

        if(state == FINISHED)
            break;

        (*i)++;
    }

    if(state != FINISHED)
    {
        qWarning("MetaDataFormatter: syntax error");
        return false;
    }

    Param param;
    param.type = Param::NUMERIC;
    bool ok = false;
    param.number = var.toInt(&ok);
    if(!ok)
        param.number = 0;
    node.params << param;
    nodes->append(node);
    return true;
}

void MetaDataFormatter::parseText(QList<MetaDataFormatter::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    Node node;
    node.command = Node::PRINT_TEXT;
    Param param;
    param.type = Param::TEXT;
    node.params.append(param);

    forever
    {
        if((*i) == end || (**i) == QChar('%'))
        {
            (*i)--;
            break;
        }
        node.params[0].text.append(**i);
        (*i)++;
    }
    if(!node.params[0].text.isEmpty())
        nodes->append(node);
}

void MetaDataFormatter::parseEscape(QList<MetaDataFormatter::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    if((*i) == end)
        return;

    Node node;
    node.command = Node::PRINT_TEXT;
    Param param;
    param.type = Param::TEXT;
    node.params.append(param);
    node.params[0].text.append(**i);
    nodes->append(node);
}

QString MetaDataFormatter::evalute(const QList<Node> *nodes, const TrackInfo *info, int trackIndex) const
{
    QString out;
    for(int i = 0; i < nodes->count(); ++i)
    {
        Node node = nodes->at(i);
        if(node.command == Node::PRINT_TEXT)
        {
            Param p = node.params.first();
            out.append(printParam(&p, info, trackIndex));

        }
        else if(node.command == Node::IF_KEYWORD)
        {
            QString var1 = printParam(&node.params[0], info, trackIndex);
            if(var1.isEmpty() || var1 == "0")
                out.append(printParam(&node.params[2], info, trackIndex));
            else
                out.append(printParam(&node.params[1], info, trackIndex));
        }
        else if(node.command == Node::AND_OPERATOR)
        {
            QString var1 = printParam(&node.params[0], info, trackIndex);
            QString var2 = printParam(&node.params[1], info, trackIndex);
            if(!var1.isEmpty() && !var2.isEmpty())
                out.append("1");
        }
        else if(node.command == Node::OR_OPERATOR)
        {
            QString var1 = printParam(&node.params[0], info, trackIndex);
            if(!var1.isEmpty())
                out.append("1");
            else
            {
                QString var2 = printParam(&node.params[1], info, trackIndex);
                if(!var2.isEmpty())
                    out.append("1");
            }
        }
        else if(node.command == Node::DIR_FUNCTION)
        {
            if(node.params.isEmpty())
                out.append(info->path().mid(0, info->path().lastIndexOf('/')));
            else
                out.append(info->path().section('/', -node.params[0].number - 2, -node.params[0].number - 2));
        }
    }
    return out;
}

QString MetaDataFormatter::printParam(MetaDataFormatter::Param *p, const TrackInfo *info, int trackIndex) const
{
    switch (p->type)
    {
    case Param::FIELD:
        return printField(p->field, info, trackIndex);
        break;
    case Param::PROPERTY:
        return printProperty(p->field, info);
        break;
    case Param::TEXT:
        return p->text;
        break;
    case Param::NODES:
        return evalute(&p->children, info, trackIndex);
        break;
    default:
        break;
    }
    return QString();
}

QString MetaDataFormatter::printField(int field, const TrackInfo *info, int trackIndex) const
{
    if(field >= Qmmp::TITLE && field <= Qmmp::DISCNUMBER)
    {
        if(field == Qmmp::TITLE)
        {
            QString title = info->value(Qmmp::TITLE);
            if(title.isEmpty()) //using file name if title is empty
            {
                title = info->path().section('/',-1);
                title = title.left(title.lastIndexOf('.'));
            }

            if(title.isEmpty()) //using full path if file name is empty
                title = info->path();

            return title;
        }
        return info->value((Qmmp::MetaData) field);
    }
    else if(field == Param::PATH)
    {
        return info->path();
    }
    else if(field == Param::TWO_DIGIT_TRACK)
    {
        return QString("%1").arg(info->value(Qmmp::TRACK),2,'0');
    }
    else if(field == Param::DURATION)
    {
        return formatDuration(info->duration());
    }
    else if(field == Param::FILE_NAME)
    {
        return info->path().section('/',-1);
    }
    else if(field == Param::TRACK_INDEX)
    {
        return QString::number(trackIndex + 1);
    }
    return QString();
}

QString MetaDataFormatter::printProperty(int field, const TrackInfo *info) const
{
    return info->value((Qmmp::TrackProperty) field);
}

QString MetaDataFormatter::dumpNode(MetaDataFormatter::Node node) const
{
    QString str;
    QStringList params;
    if(node.command == Node::PRINT_TEXT)
        str += "PRINT_TEXT";
    else if(node.command == Node::IF_KEYWORD)
        str += "IF_KEYWORD";
    else if(node.command == Node::AND_OPERATOR)
        str += "AND_OPERATOR";
    else if(node.command == Node::OR_OPERATOR)
        str += "OR_OPERATOR";
    else if(node.command == Node::DIR_FUNCTION)
        str += "DIR_FUNCTION";
    str += "(";
    for(const Param &p : qAsConst(node.params))
    {
        if(p.type == Param::FIELD)
            params.append(QString("FIELD:%1").arg(p.field));
        else if(p.type == Param::PROPERTY)
            params.append(QString("PROPERTY:%1").arg(p.field));
        else if(p.type == Param::TEXT)
            params.append(QString("TEXT:%1").arg(p.text));
        else if(p.type == Param::NUMERIC)
            params.append(QString("NUMBER:%1").arg(p.number));
        else if(p.type == Param::NODES)
        {
            QStringList nodeStrList;
            for(const Node &n : qAsConst(p.children))
            {
                nodeStrList.append(dumpNode(n));
            }
            params.append(QString("NODES:%1").arg(nodeStrList.join(",")));
        }
    }
    str.append(params.join(","));
    str.append(")");
    return str;
}

QList<MetaDataFormatter::Node> MetaDataFormatter::compile(const QString &expr)
{
    QList <Node> nodes;
    QString::const_iterator i = expr.constBegin();

    while (i != expr.constEnd())
    {
        if((*i) == QChar('%'))
        {
            ++i;
            if(i == expr.constEnd())
                continue;

            if(parseDir(&nodes, &i, expr.constEnd()))
            {
                ++i;
                continue;
            }

            if(parseField(&nodes, &i, expr.constEnd()))
            {
                ++i;
                continue;
            }

            if(parseProperty(&nodes, &i, expr.constEnd()))
            {
                ++i;
                continue;
            }

            if(parseIf(&nodes, &i, expr.constEnd()))
            {
                ++i;
                continue;
            }
            continue;
        }
        else if((*i) == QChar('&'))
        {
            ++i;
            Node node;
            node.command = Node::AND_OPERATOR;
            nodes.append(node);
        }
        else if((*i) == QChar('|'))
        {
            ++i;
            Node node;
            node.command = Node::OR_OPERATOR;
            nodes.append(node);
        }
        else if((*i) == QChar('\\'))
        {
            ++i;
            parseEscape(&nodes, &i, expr.constEnd());
            ++i;
        }
        else
        {
            parseText(&nodes, &i, expr.constEnd());
            ++i;
        }
    }

    //wrap operators
    for(int j = 0; j < nodes.count(); ++j)
    {
        if(nodes[j].command == Node::AND_OPERATOR ||
                nodes[j].command == Node::OR_OPERATOR)
        {
            if(j == 0 || j == nodes.count() - 1)
            {
                nodes.clear();
                qDebug("MetaDataFormatter: syntax error");
            }

            Param p1;
            p1.type = Param::NODES;
            p1.children << nodes.takeAt(j+1);
            Param p2;
            p2.type = Param::NODES;
            p2.children << nodes.takeAt(j-1);
            nodes[j-1].params.append(p1);
            nodes[j-1].params.append(p2);
            j--;
        }
    }
    return nodes;
}
