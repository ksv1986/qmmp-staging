include(../../plugins.pri)

TARGET=$$PLUGINS_PREFIX/Visual/analyzer
QMAKE_CLEAN =$$PLUGINS_PREFIX/Visual/libanalyzer.so


FORMS += settingsdialog.ui
HEADERS += analyzer.h \
           fft.h \
           visualanalyzerfactory.h \
           inlines.h \
           colorwidget.h \
           settingsdialog.h
SOURCES += analyzer.cpp \
           fft.c \
           visualanalyzerfactory.cpp \
           colorwidget.cpp \
           settingsdialog.cpp

TRANSLATIONS = translations/analyzer_plugin_cs.ts \
               translations/analyzer_plugin_de.ts \
               translations/analyzer_plugin_zh_CN.ts \
               translations/analyzer_plugin_zh_TW.ts \
               translations/analyzer_plugin_ru.ts \
               translations/analyzer_plugin_pl.ts \
               translations/analyzer_plugin_uk_UA.ts \
               translations/analyzer_plugin_it.ts \
               translations/analyzer_plugin_tr.ts \
               translations/analyzer_plugin_lt.ts \
               translations/analyzer_plugin_nl.ts \
               translations/analyzer_plugin_ja.ts \
               translations/analyzer_plugin_es.ts \
               translations/analyzer_plugin_sr_BA.ts \
               translations/analyzer_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc
unix{
 target.path = $$LIB_DIR/qmmp/Visual
 INSTALLS += target
}
