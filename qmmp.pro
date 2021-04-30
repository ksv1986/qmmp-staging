CONFIG += ordered

include (qmmp.pri)

TEMPLATE = subdirs
SUBDIRS = src/qmmp src/qmmpui src/app #src/plugins


unix:exists($$[QT_INSTALL_BINS]/lrelease){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease
}

unix:exists($$[QT_INSTALL_BINS]/lrelease-qt5){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease-qt5
}


win32:exists($$[QT_INSTALL_BINS]/lrelease.exe){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease.exe
}


isEmpty(LRELEASE_EXECUTABLE){
error(Could not find lrelease executable)
}
else {
message(Found lrelease executable: $$LRELEASE_EXECUTABLE)
}

message(generating translations)

unix {
    system(find . -name *.ts | xargs $$LRELEASE_EXECUTABLE -compress)
    QMAKE_DISTCLEAN += $$system(find . -name *.qm)
    QMAKE_DISTCLEAN += r lib
}

win32:system(for /r %B in (*.ts) do $$LRELEASE_EXECUTABLE -compress %B)

message("******* BUILD CONFIGURATION *******")
for(out, CONFIG):message($$out)
message(PREFIX=$$PREFIX)
message(BIN_DIR=$$BIN_DIR)
message(DATA_DIR=$$DATA_DIR)
message(PLUGIN_DIR=$$PLUGIN_DIR)
message("*** END OF BUILD CONFIGURATION ****")
