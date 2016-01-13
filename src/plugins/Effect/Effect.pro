include (../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += crossfade stereo

contains(CONFIG, BS2B_PLUGIN){
SUBDIRS += bs2b
}

unix {
    contains(CONFIG, SOXR_PLUGIN):SUBDIRS += soxr
    contains(CONFIG, LADSPA_PLUGIN):SUBDIRS += ladspa
}
