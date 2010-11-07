include(../../../qmmp.pri)

CONFIG += release warn_on 
TEMPLATE = subdirs
win32:SUBDIRS += waveout
unix{

SUBDIRS += null

contains(CONFIG, JACK_PLUGIN){
    SUBDIRS += jack
    message(***********************)
    message(* JACK plugin enabled *)
    message(***********************)
}

contains(CONFIG, OSS_PLUGIN){
    SUBDIRS += oss
    message(**********************)
    message(* OSS plugin enabled *)
    message(**********************)
}

contains(CONFIG, PULSE_AUDIO_PLUGIN){
    SUBDIRS += pulseaudio
    message(******************************)
    message(* PULSE AUDIO plugin enabled *)
    message(******************************)
}

contains(CONFIG, ALSA_PLUGIN){
    SUBDIRS += alsa
    message(***********************)
    message(* ALSA plugin enabled *)
    message(***********************)
}

contains(CONFIG, OSS4_PLUGIN){
   SUBDIRS += oss4
   message(***********************)
   message(* OSS4 plugin enabled *)
   message(***********************)
}
