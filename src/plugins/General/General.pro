include(../../../qmmp.pri)

SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager \
           streambrowser \
           trackchange \
           copypaste \
           rgscan \
           hotkey \
           listenbrainz

unix:SUBDIRS += mpris \
                kdenotify \
                converter \
                gnomehotkey

contains(CONFIG, UDISKS2_PLUGIN){
    unix:SUBDIRS += udisks2
}

contains(CONFIG, HAL_PLUGIN){
    unix:SUBDIRS += hal
}

contains(CONFIG, HISTORY_PLUGIN){
    SUBDIRS += history
}

win32:SUBDIRS += taskbar \
                 rdetect

TEMPLATE = subdirs
