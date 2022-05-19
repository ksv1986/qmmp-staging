/**************************************************************************
*   Copyright (C) 2016 by Ilya Kotov                                      *
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


#ifndef TWOPANELFILEDIALOG_H
#define TWOPANELFILEDIALOG_H

#include <qmmpui/filedialog.h>

class TwoPanelFileDialogImpl;

/*!
 *  @author Ilya Kotov <forkotov02@ya.ru>
 */
class TwoPanelFileDialog : public FileDialog
{
    Q_OBJECT
public:
    TwoPanelFileDialog();
    virtual ~TwoPanelFileDialog();

protected:
    void raise(const QString &dir, Mode mode, const QString &caption,
               const QStringList &mask) override;

    QStringList exec(QWidget *parent, const QString &dir, Mode mode,
                     const QString &caption, const QString &filter, QString *) override;


private:
    TwoPanelFileDialogImpl *m_dialog;
};




class TwoPanelFileDialogFactory : public QObject, public FileDialogFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qmmp.qmmpui.FileDialogFactoryInterface.1.0")
    Q_INTERFACES(FileDialogFactory)
public:
    FileDialog* create() override;
    FileDialogProperties properties() const override;
    void showAbout(QWidget*) override;
    QString translation() const override;
};


#endif
