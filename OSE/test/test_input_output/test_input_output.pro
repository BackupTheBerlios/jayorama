include( ../.configure.def )  

SOURCES	+= main.cpp 

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

TEMPLATE = app
CONFIG	+= qt warn_on release
INCLUDEPATH	+= ../../test /usr/local/include/izsound /usr/include/izsound ../../input ../../output ../../dsp ../../common ../../external ../../external/izsound/izsound/izsound ../../external/izsound/std-dsp/izsound
LANGUAGE	= C++
