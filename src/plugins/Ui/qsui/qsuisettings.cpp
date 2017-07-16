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

#include <QSettings>
#include <QDir>
#include <QFontDialog>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/metadataformattermenu.h>
#include "actionmanager.h"
#include "shortcutitem.h"
#include "shortcutdialog.h"
#include "popupsettings.h"
#include "toolbareditor.h"
#include "qsuisettings.h"

QSUISettings::QSUISettings(QWidget *parent) : QWidget(parent)
{
    m_ui.setupUi(this);
    //setup icons
    m_ui.popupTemplateButton->setIcon(QIcon::fromTheme("configure"));
    m_ui.customizeToolBarButton->setIcon(QIcon::fromTheme("configure"));
    //icon sizes
    m_ui.toolBarIconSizeComboBox->addItem(tr("Default"), -1);
    m_ui.toolBarIconSizeComboBox->addItem(tr("16x16"), 16);
    m_ui.toolBarIconSizeComboBox->addItem(tr("22x22"), 22);
    m_ui.toolBarIconSizeComboBox->addItem(tr("32x32"), 32);
    m_ui.toolBarIconSizeComboBox->addItem(tr("48x48"), 48);
    m_ui.toolBarIconSizeComboBox->addItem(tr("64x64"), 64);
    //load settings
    readSettings();
    loadFonts();
    createActions();
}

QSUISettings::~QSUISettings()
{}

void QSUISettings::on_plFontButton_clicked()
{
    bool ok = false;
    QFont font = m_ui.plFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        m_ui.plFontLabel->setText(font.family () + " " + QString::number(font.pointSize ()));
        m_ui.plFontLabel->setFont(font);
    }
}

void QSUISettings::on_columnFontButton_clicked()
{
    bool ok = false;
    QFont font = m_ui.columnFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        m_ui.columnFontLabel->setText(font.family () + " " + QString::number(font.pointSize ()));
        m_ui.columnFontLabel->setFont(font);
    }
}

void QSUISettings::on_tabsFontButton_clicked()
{
    bool ok = false;
    QFont font = m_ui.tabsFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        m_ui.tabsFontLabel->setText(font.family () + " " + QString::number(font.pointSize ()));
        m_ui.tabsFontLabel->setFont(font);
    }
}

void QSUISettings::showEvent(QShowEvent *)
{
    m_ui.hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    m_ui.hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void QSUISettings::loadFonts()
{
    QString fontName;
    QFont font;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");

    m_ui.systemFontsCheckBox->setChecked(settings.value("use_system_fonts", true).toBool());

    fontName = settings.value ("pl_font", qApp->font("QAbstractItemView").toString()).toString();
    font.fromString(fontName);
    m_ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.plFontLabel->setFont(font);

    fontName = settings.value ("pl_tabs_font", qApp->font("QTabWidget").toString()).toString();
    font.fromString(fontName);
    m_ui.tabsFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.tabsFontLabel->setFont(font);

    fontName = settings.value ("pl_header_font", qApp->font("QAbstractItemView").toString()).toString();
    font.fromString(fontName);
    m_ui.columnFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    m_ui.columnFontLabel->setFont(font);
}

void QSUISettings::createActions()
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui.windowTitleButton->setMenu(menu);
    m_ui.windowTitleButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(patternSelected(QString)), SLOT(addWindowTitleString(QString)));
}

void QSUISettings::on_popupTemplateButton_clicked()
{
    PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();
}

void QSUISettings::on_customizeToolBarButton_clicked()
{
    ToolBarEditor editor(this);
    editor.exec();
}

void QSUISettings::on_resetFontsButton_clicked()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.remove("Simple/pl_font");
    settings.remove("Simple/pl_tabs_font");
    settings.remove("Simple/pl_header_font");
    loadFonts();
}

void QSUISettings::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    //playlist
    m_ui.protocolCheckBox->setChecked(settings.value("pl_show_protocol", false).toBool());
    m_ui.numbersCheckBox->setChecked(settings.value("pl_show_numbers", true).toBool());
    m_ui.lengthsCheckBox->setChecked(settings.value("pl_show_lengths", true).toBool());
    m_ui.alignCheckBox->setChecked(settings.value("pl_align_numbers", false).toBool());
    m_ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    m_ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    //tabs
    m_ui.tabsClosableCheckBox->setChecked(settings.value("pl_tabs_closable", false).toBool());
    m_ui.showNewPLCheckBox->setChecked(settings.value("pl_show_new_pl_button", false).toBool());
    m_ui.showTabListMenuCheckBox->setChecked(settings.value("pl_show_tab_list_menu", false).toBool());
    //view
    m_ui.hiddenCheckBox->setChecked(settings.value("start_hidden", false).toBool());
    m_ui.hideOnCloseCheckBox->setChecked(settings.value("hide_on_close", false).toBool());
    m_ui.windowTitleLineEdit->setText(settings.value("window_title_format","%if(%p,%p - %t,%t)").toString());
    //analyzer colors
    m_ui.aColor1->setColor(settings.value("vis_color1", "#BECBFF").toString());
    m_ui.aColor2->setColor(settings.value("vis_color2", "#BECBFF").toString());
    m_ui.aColor3->setColor(settings.value("vis_color3", "#BECBFF").toString());
    m_ui.peaksColor->setColor(settings.value("vis_peak_color", "#DDDDDD").toString());
    m_ui.bgColor->setColor(settings.value("vis_bg_color", "Black").toString());
    //playlist colors
    QString normal_bg = palette().color(QPalette::Base).name();
    QString alternate = palette().color(QPalette::AlternateBase).name();
    QString selected_bg = palette().color(QPalette::Highlight).name();
    QString normal = palette().color(QPalette::Text).name();
    QString current = palette().color(QPalette::Text).name();
    QString highlighted = palette().color(QPalette::HighlightedText).name();
    QString group_bg = palette().color(QPalette::Base).name();
    QString group_text = palette().color(QPalette::Text).name();
    m_ui.plSystemColorsCheckBox->setChecked(settings.value("pl_system_colors", true).toBool());
    m_ui.plBg1Color->setColor(settings.value("pl_bg1_color", normal_bg).toString());
    m_ui.plBg2Color->setColor(settings.value("pl_bg2_color", alternate).toString());
    m_ui.plHlColor->setColor(settings.value("pl_highlight_color", selected_bg).toString());
    m_ui.plTextNormalColor->setColor(settings.value("pl_normal_text_color", normal).toString());
    m_ui.plTextCurrentColor->setColor(settings.value("pl_current_text_color", current).toString());
    m_ui.plTextHlCurrentColor->setColor(settings.value("pl_hl_text_color", highlighted).toString());
    m_ui.plGrBgColor->setColor(settings.value("pl_group_bg", group_bg).toString());
    m_ui.plGrTextColor->setColor(settings.value("pl_group_text", group_text).toString());
    //toolbar
    int index = m_ui.toolBarIconSizeComboBox->findData(settings.value("toolbar_icon_size", -1).toInt());
    m_ui.toolBarIconSizeComboBox->setCurrentIndex(index > 0 ? index : 0);
    settings.endGroup();
}

void QSUISettings::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    settings.setValue("pl_show_protocol", m_ui.protocolCheckBox->isChecked());
    settings.setValue("pl_show_numbers", m_ui.numbersCheckBox->isChecked());
    settings.setValue("pl_show_lengths", m_ui.lengthsCheckBox->isChecked());
    settings.setValue("pl_align_numbers", m_ui.alignCheckBox->isChecked());
    settings.setValue("pl_show_anchor", m_ui.anchorCheckBox->isChecked());
    settings.setValue("pl_show_popup", m_ui.popupCheckBox->isChecked());
    settings.setValue("pl_tabs_closable", m_ui.tabsClosableCheckBox->isChecked());
    settings.setValue("pl_show_new_pl_button", m_ui.showNewPLCheckBox->isChecked());
    settings.setValue("pl_show_tab_list_menu", m_ui.showTabListMenuCheckBox->isChecked());
    settings.setValue("start_hidden", m_ui.hiddenCheckBox->isChecked());
    settings.setValue("hide_on_close", m_ui.hideOnCloseCheckBox->isChecked());
    settings.setValue("window_title_format", m_ui.windowTitleLineEdit->text());
    settings.setValue("vis_color1", m_ui.aColor1->colorName());
    settings.setValue("vis_color2", m_ui.aColor2->colorName());
    settings.setValue("vis_color3", m_ui.aColor3->colorName());
    settings.setValue("vis_peak_color", m_ui.peaksColor->colorName());
    settings.setValue("vis_bg_color", m_ui.bgColor->colorName());
    settings.setValue("pl_system_colors", m_ui.plSystemColorsCheckBox->isChecked());
    settings.setValue("pl_bg1_color", m_ui.plBg1Color->colorName());
    settings.setValue("pl_bg2_color", m_ui.plBg2Color->colorName());
    settings.setValue("pl_highlight_color", m_ui.plHlColor->colorName());
    settings.setValue("pl_normal_text_color", m_ui.plTextNormalColor->colorName());
    settings.setValue("pl_current_text_color", m_ui.plTextCurrentColor->colorName());
    settings.setValue("pl_hl_text_color", m_ui.plTextHlCurrentColor->colorName());
    settings.setValue("pl_group_bg", m_ui.plGrBgColor->colorName());
    settings.setValue("pl_group_text", m_ui.plGrTextColor->colorName());
    settings.setValue("pl_font", m_ui.plFontLabel->font().toString());
    settings.setValue("pl_tabs_font", m_ui.tabsFontLabel->font().toString());
    settings.setValue("pl_header_font", m_ui.columnFontLabel->font().toString());
    settings.setValue("use_system_fonts", m_ui.systemFontsCheckBox->isChecked());
    int index = m_ui.toolBarIconSizeComboBox->currentIndex();
    settings.setValue("toolbar_icon_size", m_ui.toolBarIconSizeComboBox->itemData(index));
    settings.endGroup();
}

void QSUISettings::addWindowTitleString(const QString &str)
{
    if (m_ui.windowTitleLineEdit->cursorPosition () < 1)
        m_ui.windowTitleLineEdit->insert(str);
    else
        m_ui.windowTitleLineEdit->insert(" - "+str);
}
