/***************************************************************************
 *   Copyright (C) 2007-2021 by Ilya Kotov                                 *
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
#include <QDir>
#include <QSettings>
#include <QFontDialog>
#include <QTreeWidgetItem>
#include <qmmp/decoder.h>
#include <qmmp/output.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effectfactory.h>
#include <qmmp/effect.h>
#include <qmmp/soundcore.h>
#include <qmmp/enginefactory.h>
#include <qmmp/abstractengine.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/inputsource.h>
#include <qmmp/inputsourcefactory.h>
#include "ui_configdialog.h"
#include "pluginitem_p.h"
#include "radioitemdelegate_p.h"
#include "generalfactory.h"
#include "general.h"
#include "uihelper.h"
#include "uiloader.h"
#include "filedialog.h"
#include "mediaplayer.h"
#include "qmmpuisettings.h"
#include "playlistmodel.h"
#include "winfileassocpage_p.h"
#include "metadataformattermenu.h"
#include "configdialog.h"

ConfigDialog::ConfigDialog (QWidget *parent) : QDialog (parent)
{
    m_ui = new Ui::ConfigDialog;
    m_ui->setupUi (this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, false);
    m_ui->preferencesButton->setEnabled(false);
    m_ui->informationButton->setEnabled(false);
    m_ui->treeWidget->setItemDelegate(new RadioItemDelegate(this));
    m_ui->treeWidget->header()->setSectionsMovable(false);
    connect (this, SIGNAL(rejected()),SLOT(saveSettings()));
    m_ui->replayGainModeComboBox->addItem (tr("Track"), QmmpSettings::REPLAYGAIN_TRACK);
    m_ui->replayGainModeComboBox->addItem (tr("Album"), QmmpSettings::REPLAYGAIN_ALBUM);
    m_ui->replayGainModeComboBox->addItem (tr("Disabled"), QmmpSettings::REPLAYGAIN_DISABLED);
    m_ui->bitDepthComboBox->addItem("16", Qmmp::PCM_S16LE);
    m_ui->bitDepthComboBox->addItem("24", Qmmp::PCM_S24LE);
    m_ui->bitDepthComboBox->addItem("32", Qmmp::PCM_S32LE);
#ifndef Q_OS_WIN
    m_ui->bitDepthComboBox->addItem("32 (float)", Qmmp::PCM_FLOAT);
#endif
    m_ui->proxyTypeComboBox->addItem(tr("HTTP"), QmmpSettings::HTTP_PROXY);
    m_ui->proxyTypeComboBox->addItem(tr("SOCKS5"), QmmpSettings::SOCKS5_PROXY);
    readSettings();
    loadPluginsInfo();
    loadLanguages();
    createMenus();
    //setup icons
    m_ui->preferencesButton->setIcon(QIcon::fromTheme("configure"));
    m_ui->informationButton->setIcon(QIcon::fromTheme("dialog-information"));
    //file associations
#ifdef Q_OS_WIN
    if(!Qmmp::isPortable())
    {
        m_ui->stackedWidget->addWidget(new WinFileAssocPage(this));
        m_ui->contentsWidget->addItem(tr("File Types"));
        m_ui->contentsWidget->item(m_ui->contentsWidget->count() - 1)->setIcon(QIcon(":associations.png"));
    }
#endif
}

ConfigDialog::~ConfigDialog()
{
    delete m_ui;
}

void ConfigDialog::addPage(const QString &name, QWidget *widget, const QIcon &icon)
{
    m_ui->stackedWidget->insertWidget(m_insert_row, widget);
    m_ui->contentsWidget->insertItem (m_insert_row, name);
    m_ui->contentsWidget->item(m_insert_row)->setIcon(icon);
    m_ui->contentsWidget->setCurrentRow(0);
    m_insert_row++;
}

void ConfigDialog::readSettings()
{
    if (MediaPlayer::instance())
    {
        //playlist options
        QmmpUiSettings *guis = QmmpUiSettings::instance();
        m_ui->groupLineEdit->setText(guis->groupFormat());
        m_ui->metaDataCheckBox->setChecked(guis->useMetaData());
        m_ui->plMetaDataCheckBox->setChecked(guis->readMetaDataForPlayLists());
        m_ui->underscoresCheckBox->setChecked(guis->convertUnderscore());
        m_ui->per20CheckBox->setChecked(guis->convertTwenty());
        m_ui->clearPrevPLCheckBox->setChecked(guis->clearPreviousPlayList());
        //resume playback on startup
        m_ui->continuePlaybackCheckBox->setChecked(guis->resumeOnStartup());
        //directory filters
        m_ui->dirRestrictLineEdit->setText(guis->restrictFilters().join(",").trimmed());
        m_ui->dirExcludeLineEdit->setText(guis->excludeFilters().join(",").trimmed());
        //default playlist
        m_ui->defaultPlayListCheckBox->setChecked(guis->useDefaultPlayList());
        m_ui->defaultPlayListLineEdit->setText(guis->defaultPlayListName());
        //playlist auto-save when modified
        m_ui->autoSavePlayListCheckBox->setChecked(guis->autoSavePlayList());
        //url dialog
        m_ui->clipboardCheckBox->setChecked(guis->useClipboard());
    }
    //proxy settings
    QmmpSettings *gs = QmmpSettings::instance();
    m_ui->enableProxyCheckBox->setChecked(gs->isProxyEnabled());
    m_ui->authProxyCheckBox->setChecked(gs->useProxyAuth());
    m_ui->hostLineEdit->setText(gs->proxy().host());
    m_ui->proxyTypeComboBox->setCurrentIndex(m_ui->proxyTypeComboBox->findData(gs->proxyType()));
    if (gs->proxy().port(0))
        m_ui->portLineEdit->setText(QString::number(gs->proxy().port(0)));
    m_ui->proxyUserLineEdit->setText(gs->proxy().userName());
    m_ui->proxyPasswLineEdit->setText(gs->proxy().password());

    m_ui->hostLineEdit->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->portLineEdit->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->proxyTypeComboBox->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->proxyUserLineEdit->setEnabled(m_ui->authProxyCheckBox->isChecked());
    m_ui->proxyPasswLineEdit->setEnabled(m_ui->authProxyCheckBox->isChecked());
    //file type determination
    m_ui->byContentCheckBox->setChecked(gs->determineFileTypeByContent());
    //cover options
    m_ui->coverIncludeLineEdit->setText(gs->coverNameFilters(true).join(","));
    m_ui->coverExcludeLineEdit->setText(gs->coverNameFilters(false).join(","));
    m_ui->coverDepthSpinBox->setValue(gs->coverSearchDepth());
    m_ui->useCoverFilesCheckBox->setChecked(gs->useCoverFiles());
    //replay gain
    m_ui->clippingCheckBox->setChecked(gs->replayGainPreventClipping());
    m_ui->replayGainModeComboBox->setCurrentIndex(m_ui->replayGainModeComboBox->findData(gs->replayGainMode()));
    m_ui->preampDoubleSpinBox->setValue(gs->replayGainPreamp());
    m_ui->defaultGainDoubleSpinBox->setValue(gs->replayGainDefaultGain());
    //audio
    m_ui->volumeStepSpinBox->setValue(gs->volumeStep());
    m_ui->softVolumeCheckBox->setChecked(gs->useSoftVolume());
    m_ui->bitDepthComboBox->setCurrentIndex(m_ui->bitDepthComboBox->findData(gs->outputFormat()));
    m_ui->ditheringCheckBox->setChecked(gs->useDithering());
    m_ui->bufferSizeSpinBox->setValue(gs->bufferSize());
    m_ui->abrCheckBox->setChecked(gs->averageBitrate());
    //equalizer
    m_ui->twoPassEqCheckBox->setChecked(gs->eqSettings().twoPasses());
    //geometry
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    resize(settings.value("ConfigDialog/window_size", QSize(700,470)).toSize());
    QList<QVariant> var_sizes = settings.value("ConfigDialog/splitter_sizes").toList();
    if(var_sizes.count() != 2)
    {
        var_sizes.clear();
        var_sizes << 180 << width()-180;
    }
    QList<int> sizes;
    sizes << var_sizes.first().toInt() << var_sizes.last().toInt();
    m_ui->splitter->setSizes(sizes);
    //fonts
    QFont font;
    font.fromString(settings.value("CueEditor/font", qApp->font("QPlainTextEdit").toString()).toString());
    m_ui->cueFontLabel->setText(font.family() + " " + QString::number(font.pointSize()));
    m_ui->cueFontLabel->setFont(font);
    m_ui->cueSystemFontCheckBox->setChecked(settings.value("CueEditor/use_system_font", true).toBool());
}

void ConfigDialog::on_contentsWidget_currentItemChanged(QListWidgetItem *current,
                                                        QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    m_ui->stackedWidget->setCurrentIndex (m_ui->contentsWidget->row (current));
}

void ConfigDialog::loadPluginsInfo()
{
    m_ui->treeWidget->blockSignals(true);
    /*
        load transport plugin information
     */
    QTreeWidgetItem *item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Transports"));
    item->setFirstColumnSpanned(true);
    for(InputSourceFactory *factory : InputSource::factories())
    {
        new PluginItem (item, factory,  InputSource::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load input plugins information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Decoders"));
    item->setFirstColumnSpanned(true);
    for(DecoderFactory *factory : Decoder::factories())
    {
        new PluginItem (item, factory,  Decoder::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load audio engines information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Engines"));
    item->setFirstColumnSpanned(true);
    for(EngineFactory *factory : AbstractEngine::factories())
    {
        new PluginItem (item, factory,  AbstractEngine::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load effect plugin information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Effects"));
    item->setFirstColumnSpanned(true);
    for(EffectFactory *factory : Effect::factories())
    {
        new PluginItem (item, factory, Effect::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load visual plugin information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Visualization"));
    item->setFirstColumnSpanned(true);
    for(VisualFactory *factory : Visual::factories())
    {
        new PluginItem (item, factory, Visual::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load general plugin information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("General"));
    item->setFirstColumnSpanned(true);
    for(GeneralFactory *factory : General::factories())
    {
        new PluginItem (item, factory, General::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load output plugins information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("Output"));
    item->setFirstColumnSpanned(true);
    for(OutputFactory *factory : Output::factories())
    {
        new PluginItem (item, factory, Output::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load file dialogs information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("File Dialogs"));
    item->setFirstColumnSpanned(true);
    for(FileDialogFactory *factory : FileDialog::factories())
    {
        new PluginItem (item, factory, FileDialog::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load user interfaces information
    */
    item = new QTreeWidgetItem (m_ui->treeWidget, QStringList() << tr("User Interfaces"));
    item->setFirstColumnSpanned(true);
    for(UiFactory *factory : UiLoader::factories())
    {
        new PluginItem (item, factory, UiLoader::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);

    m_ui->treeWidget->blockSignals(false);
    m_ui->treeWidget->resizeColumnToContents(0);
    m_ui->treeWidget->resizeColumnToContents(1);
}

void ConfigDialog::on_preferencesButton_clicked()
{
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showSettings(this);

}

void ConfigDialog::on_informationButton_clicked()
{
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showAbout(this);
}

void ConfigDialog::createMenus()
{
    MetaDataFormatterMenu *groupMenu = new MetaDataFormatterMenu(MetaDataFormatterMenu::GROUP_MENU, this);
    m_ui->groupButton->setMenu(groupMenu);
    m_ui->groupButton->setPopupMode(QToolButton::InstantPopup);
    connect(groupMenu, SIGNAL(patternSelected(QString)), SLOT(addGroupString(QString)));

    m_ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_preferencesAction = new QAction(QIcon::fromTheme("configure"),tr("Preferences"), m_ui->treeWidget);
    m_preferencesAction->setEnabled(false);
    m_ui->treeWidget->addAction(m_preferencesAction);
    m_informationAction = new QAction(QIcon::fromTheme("dialog-information"),tr("Information"), m_ui->treeWidget);
    m_informationAction->setEnabled(false);
    m_ui->treeWidget->addAction(m_informationAction);
    connect(m_preferencesAction, SIGNAL(triggered()), SLOT(on_preferencesButton_clicked()));
    connect(m_informationAction, SIGNAL(triggered()), SLOT(on_informationButton_clicked()));
}

void ConfigDialog::loadLanguages()
{
    QMap<QString, QString> l;
    l["auto"] = tr("<Autodetect>");
    l["pt_BR"] = tr("Brazilian Portuguese");
    l["zh_CN"] = tr("Chinese Simplified");
    l["zh_TW"] = tr("Chinese Traditional");
    l["cs"] = tr("Czech");
    l["nl"] = tr("Dutch");
    l["en_US"] = tr("English");
    l["fr"] = tr("French");
    l["gl_ES"] = tr("Galician");
    l["de"] = tr("German");
    l["el"] = tr("Greek");
    l["he"] = tr("Hebrew");
    l["hu"] = tr("Hungarian");
    l["id"] = tr("Indonesian");
    l["it"] = tr("Italian");
    l["ja"] = tr("Japanese");
    l["kk"] = tr("Kazakh");
    l["lt"] = tr("Lithuanian");
    l["pl_PL"] = tr("Polish");
    l["pt"] = tr("Portuguese");
    l["ru_RU"] = tr("Russian");
    l["sr_RS"] = tr("Serbian");
    l["sk"] = tr("Slovak");
    l["es"] = tr("Spanish");
    l["tr"] = tr("Turkish");
    l["uk_UA"] = tr("Ukrainian");
    l["sr_BA"] = tr("Serbian (Ijekavian)");
    l["sr_RS"] = tr("Serbian (Ekavian)");

    for(const QString &code : l.keys())
    {
        QString title = code != "auto" ? l.value(code) + " (" + code + ")" : l.value(code);
        m_ui->langComboBox->addItem(title, code);
    }

    QString code = Qmmp::uiLanguageID();
    int index = m_ui->langComboBox->findData(code);
    if(index < 0)
        index = m_ui->langComboBox->findData("auto");
    m_ui->langComboBox->setCurrentIndex(index);
}

void ConfigDialog::addGroupString(const QString &str)
{
    if (m_ui->groupLineEdit->cursorPosition () < 1)
        m_ui->groupLineEdit->insert(str);
    else
        m_ui->groupLineEdit->insert(" - "+str);
}

void ConfigDialog::saveSettings()
{
    if (QmmpUiSettings *guis = QmmpUiSettings::instance())
    {
        guis->setGroupFormat(m_ui->groupLineEdit->text().trimmed());
        guis->setUseMetaData(m_ui->metaDataCheckBox->isChecked());
        guis->setReadMetaDataForPlayLists(m_ui->plMetaDataCheckBox->isChecked());
        guis->setConvertUnderscore(m_ui->underscoresCheckBox->isChecked());
        guis->setConvertTwenty(m_ui->per20CheckBox->isChecked());
        guis->setClearPreviousPlayList(m_ui->clearPrevPLCheckBox->isChecked());
        guis->setResumeOnStartup(m_ui->continuePlaybackCheckBox->isChecked());
        guis->setRestrictFilters(m_ui->dirRestrictLineEdit->text());
        guis->setExcludeFilters(m_ui->dirExcludeLineEdit->text());
        guis->setDefaultPlayList(m_ui->defaultPlayListLineEdit->text(),
                                 m_ui->defaultPlayListCheckBox->isChecked());
        guis->setAutoSavePlayList(m_ui->autoSavePlayListCheckBox->isChecked());
        guis->setUseClipboard(m_ui->clipboardCheckBox->isChecked());
    }

    QmmpSettings *gs = QmmpSettings::instance();
    //proxy
    QUrl proxyUrl;
    proxyUrl.setHost(m_ui->hostLineEdit->text());
    proxyUrl.setPort(m_ui->portLineEdit->text().toUInt());
    proxyUrl.setUserName(m_ui->proxyUserLineEdit->text());
    proxyUrl.setPassword(m_ui->proxyPasswLineEdit->text());
    gs->setNetworkSettings(m_ui->enableProxyCheckBox->isChecked(),
                           m_ui->authProxyCheckBox->isChecked(),
                           static_cast<QmmpSettings::ProxyType>(m_ui->proxyTypeComboBox->currentData().toInt()),
                           proxyUrl);

    gs->setCoverSettings(m_ui->coverIncludeLineEdit->text().split(","),
                         m_ui->coverExcludeLineEdit->text().split(","),
                         m_ui->coverDepthSpinBox->value(),
                         m_ui->useCoverFilesCheckBox->isChecked());
    int i = m_ui->replayGainModeComboBox->currentIndex();
    gs->setReplayGainSettings((QmmpSettings::ReplayGainMode)
                              m_ui->replayGainModeComboBox->itemData(i).toInt(),
                              m_ui->preampDoubleSpinBox->value(),
                              m_ui->defaultGainDoubleSpinBox->value(),
                              m_ui->clippingCheckBox->isChecked());
    i = m_ui->bitDepthComboBox->currentIndex();
    gs->setAudioSettings(m_ui->softVolumeCheckBox->isChecked(),
                         (Qmmp::AudioFormat)m_ui->bitDepthComboBox->itemData(i).toInt(),
                         m_ui->ditheringCheckBox->isChecked());
    gs->setAverageBitrate(m_ui->abrCheckBox->isChecked());
    gs->setBufferSize(m_ui->bufferSizeSpinBox->value());
    gs->setDetermineFileTypeByContent(m_ui->byContentCheckBox->isChecked());
    gs->setVolumeStep(m_ui->volumeStepSpinBox->value());
    EqSettings eqs = gs->eqSettings();
    eqs.setTwoPasses(m_ui->twoPassEqCheckBox->isChecked());
    gs->setEqSettings(eqs);
    QList<QVariant> var_sizes;
    var_sizes << m_ui->splitter->sizes().first() << m_ui->splitter->sizes().last();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("ConfigDialog/splitter_sizes", var_sizes);
    settings.setValue("ConfigDialog/window_size", size());
    //User interface language
    int index = m_ui->langComboBox->currentIndex();
    if(index >= 0)
        Qmmp::setUiLanguageID(m_ui->langComboBox->itemData(index).toString());
    //fonts
    settings.setValue("CueEditor/font", m_ui->cueFontLabel->font().toString());
    settings.setValue("CueEditor/use_system_font", m_ui->cueSystemFontCheckBox->isChecked());
}

void ConfigDialog::on_treeWidget_itemChanged (QTreeWidgetItem *item, int column)
{
    if(column == 0 && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->setEnabled(item->checkState(0) == Qt::Checked);
}

void ConfigDialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if(current->type() >= PluginItem::TRANSPORT)
    {
        m_ui->preferencesButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasSettings());
        m_ui->informationButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasAbout());
    }
    else
    {
        m_ui->preferencesButton->setEnabled(false);
        m_ui->informationButton->setEnabled(false);
    }
    m_preferencesAction->setEnabled(m_ui->preferencesButton->isEnabled());
    m_informationAction->setEnabled(m_ui->informationButton->isEnabled());
}

void ConfigDialog::on_cueFontButton_clicked()
{
    bool ok = false;
    QFont font = m_ui->cueFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if(ok)
    {
        m_ui->cueFontLabel->setText(font.family () + " " + QString::number(font.pointSize ()));
        m_ui->cueFontLabel->setFont(font);
    }
}
