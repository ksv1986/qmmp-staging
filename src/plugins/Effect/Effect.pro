include (../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += crossfade stereo monotostereo

contains(CONFIG, BS2B_PLUGIN):SUBDIRS += bs2b
contains(CONFIG, SOXR_PLUGIN):SUBDIRS += soxr
contains(CONFIG, FILEWRITER_PLUGIN):SUBDIRS += filewriter

unix {
    contains(CONFIG, LADSPA_PLUGIN):SUBDIRS += ladspa
}
