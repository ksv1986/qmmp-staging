include(../../../qmmp.pri)

TEMPLATE = subdirs
win32:SUBDIRS += waveout
win32:SUBDIRS += directsound
win32:SUBDIRS += wasapi

SUBDIRS += null

unix{

contains(CONFIG, JACK_PLUGIN){
    SUBDIRS += jack
}

contains(CONFIG, OSS_PLUGIN){
    SUBDIRS += oss
}

contains(CONFIG, PULSE_AUDIO_PLUGIN){
    SUBDIRS += pulseaudio
}

contains(CONFIG, ALSA_PLUGIN){
    SUBDIRS += alsa
}

contains(CONFIG, OSS4_PLUGIN){
    SUBDIRS += oss4
}

contains(CONFIG, SHOUT_PLUGIN){
   SUBDIRS += shout
}

contains(CONFIG, PIPEWIRE_PLUGIN){
   SUBDIRS += pipewire
}

}

#all platforms
qtHaveModule(multimedia){
  contains(CONFIG, QTMULTIMEDIA_PLUGIN){
     SUBDIRS += qtmultimedia
  }
}
