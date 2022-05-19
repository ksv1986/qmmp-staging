include(../../../qmmp.pri)

TEMPLATE = subdirs
SUBDIRS += vorbis sndfile wavpack cue

contains(CONFIG, WITH_MAD)|contains(CONFIG, WITH_MPG123){
    SUBDIRS += mpeg
}

contains(CONFIG, FLAC_PLUGIN){
    SUBDIRS += flac
}

contains(CONFIG, MUSEPACK_PLUGIN){
    SUBDIRS += mpc
}

contains(CONFIG, XMP_PLUGIN){
    SUBDIRS += xmp
}

contains(CONFIG, FFMPEG_PLUGIN){
    SUBDIRS += ffmpeg
}

contains(CONFIG, GME_PLUGIN){
    SUBDIRS += gme
}

contains(CONFIG, OPUS_PLUGIN){
    SUBDIRS += opus
}

contains(CONFIG, CDAUDIO_PLUGIN){
    SUBDIRS += cdaudio
}

contains(CONFIG, SID_PLUGIN){
    SUBDIRS += sid
}

unix{

contains(CONFIG, AAC_PLUGIN){
    SUBDIRS += aac
}


contains(CONFIG, WILDMIDI_PLUGIN){
    SUBDIRS += wildmidi
}

contains(CONFIG, ARCHIVE_PLUGIN){
    SUBDIRS += archive
}
}
