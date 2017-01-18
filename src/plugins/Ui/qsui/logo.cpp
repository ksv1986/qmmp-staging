/***************************************************************************
 *   Copyright (C) 2011-2017 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
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

#include <QPainter>
#include <QFile>
#include <QTimer>
#include <cmath>
#include <qmmp/qmmp.h>
#include "inlines.h"
#include "logo.h"

Logo::Logo(QWidget *parent) : Visual(parent)
{
    QPixmap pixmap(":/qsui/terminus.png");
    m_letters.insert('0', pixmap.copy(0, 0, 8, 14));
    m_letters.insert('1', pixmap.copy(8, 0, 8, 14));
    m_letters.insert('2', pixmap.copy(16, 0, 8, 14));
    m_letters.insert('3', pixmap.copy(24, 0, 8, 14));
    m_letters.insert('4', pixmap.copy(32, 0, 8, 14));
    m_letters.insert('5', pixmap.copy(40, 0, 8, 14));
    m_letters.insert('6', pixmap.copy(48, 0, 8, 14));
    m_letters.insert('7', pixmap.copy(56, 0, 8, 14));
    m_letters.insert('8', pixmap.copy(64, 0, 8, 14));
    m_letters.insert('9', pixmap.copy(72, 0, 8, 14));
    m_letters.insert('A', pixmap.copy(80, 0, 8, 14));
    m_letters.insert('B', pixmap.copy(88, 0, 8, 14));
    m_letters.insert('C', pixmap.copy(96, 0, 8, 14));
    m_letters.insert('D', pixmap.copy(104, 0, 8, 14));
    m_letters.insert('E', pixmap.copy(112, 0, 8, 14));
    m_letters.insert('F', pixmap.copy(120, 0, 8, 14));
    m_letters.insert('/', pixmap.copy(128, 0, 8, 14));
    m_letters.insert('|', pixmap.copy(136, 0, 8, 14));
    m_letters.insert('\\', pixmap.copy(144, 0, 8, 14));
    m_letters.insert('_', pixmap.copy(152, 0, 8, 14));
    m_letters.insert('-', pixmap.copy(160, 0, 8, 14));
    m_letters.insert('X', pixmap.copy(168, 0, 8, 14));
    m_letters.insert('.', pixmap.copy(176, 0, 8, 14));
    m_letters.insert(' ', pixmap.copy(184, 0, 8, 14));

    QFile file(":/ascii_logo.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while(!file.atEnd())
    {
        QString line = file.readLine();
        m_source_lines.append(line);
    }

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(updateLetters()));
    m_timer->setInterval(50);

    m_value = 0;
    m_elapsed = 0;
    updateLetters();
    Visual::add(this);
}

Logo::~Logo()
{
    Visual::remove(this);
}

void Logo::start()
{
    m_timer->start();
}

void Logo::stop()
{
    m_timer->stop();
}

void Logo::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), "black");

    for(int row = 0; row < m_lines.count(); ++row)
    {
        QString text = m_lines.at(row);
        for(int i = 0; i < text.size(); ++i)
        {
            painter.drawPixmap(width() / 2 - 155 + i*8,row*14, m_letters.value(text[i]));
        }
    }
}

void Logo::mousePressEvent(QMouseEvent *)
{
    m_elapsed = 2000;
    m_value = 0;
}

void Logo::updateLetters()
{
    if(m_elapsed < 2000)
    {
        m_value = m_elapsed / 100;
        processPreset1();
    }
    else if (m_elapsed >= 2000 && m_elapsed < 6000)
    {
        m_value++;
        processPreset2();
    }
    else if(m_elapsed >= 6000 && m_elapsed < 9000)
    {
        m_value++;
        processPreset3();
    }
    else if(m_elapsed >= 9000 && m_elapsed < 12000)
    {
        processPreset4();
    }
    else if(m_elapsed >= 12000)
    {
       m_value = 0;
       m_elapsed = 0;
    }
    m_elapsed += 50;
}

void Logo::processPreset1()
{
    m_lines.clear();
    QString line;
    for(int i = 0; i < m_source_lines.count(); ++i)
    {
        line = m_source_lines[i];
        line = line.replace("X", ".");
        if(m_value == i)
        {
            line.remove(0,2);
            line.append("  ");
        }
        else if(m_value == i - 1 || m_value == i + 1)
        {
            line.remove(0,1);
            line.append(" ");
        }
        m_lines.append(line);
    }
    update();
}

void Logo::processPreset2()
{
    m_lines.clear();
    QString str = QString("..0000..");//.arg(Qmmp::strVersion().left(5));
    int at = m_value % str.size();

    foreach(QString line, m_source_lines)
    {
        while(line.contains("X"))
        {
            at++;
            line.replace(line.indexOf("X"), 1, QString("%1").arg(str.at(at % str.size()),
                                                                 0, 16).toUpper());
        }

        m_lines.append(line);
    }
    update();
}

void Logo::processPreset3()
{
    m_lines.clear();
    QString str = QString("...%1...").arg(Qmmp::strVersion().left(6));
    int at = m_value % str.size();

    foreach(QString line, m_source_lines)
    {
        while(line.contains("X"))
        {
            at++;
            line.replace(line.indexOf("X"), 1, QString("%1").arg(str.at(at % str.size()),
                                                                 0, 16).toUpper());
        }

        m_lines.append(line);
    }
    update();
}

void Logo::processPreset4()
{
    m_lines.clear();

    int max = 0;

    if(takeData(m_buffer))
    {
        for(int j = 0; j < QMMP_VISUAL_NODE_SIZE; j+=8)
        {
            max = qMax(max, int(std::abs(m_buffer[j] * 65536.0)));
        }
    }

    m_value -= 512;
    m_value = qMax(m_value, max);

    int at = 0;

    foreach(QString line, m_source_lines)
    {
        int count = line.count("X");
        int k = 0;

        while(k < m_value * count / 65536 / 2)
        {
            int value = std::abs(m_buffer[qMin(at++, QMMP_VISUAL_NODE_SIZE)] * 16);
            line.replace(line.indexOf("X"), 1, QString("%1").arg(value, 0, 16).toUpper());
            k++;
        }

        k = 0;

        while(k < m_value * count / 65536 / 2)
        {
            int value = std::abs(m_buffer[qMin(at++, QMMP_VISUAL_NODE_SIZE)] * 16);
            line.replace(line.lastIndexOf("X"), 1, QString("%1").arg(value, 0, 16).toUpper());
            k++;
        }

        while(line.contains("X"))
        {
            line.replace(line.indexOf("X"), 1, ".");
        }

        m_lines.append(line);
    }
    update();
}
