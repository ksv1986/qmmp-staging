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
           hotkey
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

TEMPLATE = subdirs
