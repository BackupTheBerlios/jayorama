SOURCES	+= external/izsound/izsound/dspunit.cpp \
	external/izsound/std-dsp/bandfilter.cpp \
	external/izsound/std-dsp/blackhole.cpp \
	external/izsound/std-dsp/crossfader.cpp \
	external/izsound/std-dsp/datapicker.cpp \
	external/izsound/std-dsp/delayextrastereo.cpp \
	external/izsound/std-dsp/demultiplexer.cpp \
	external/izsound/std-dsp/flanger.cpp \
	external/izsound/std-dsp/fragmenter.cpp \
	external/izsound/std-dsp/libaooutput.cpp \
	external/izsound/std-dsp/maddecoder.cpp \
	external/izsound/std-dsp/oggfiledecoder.cpp \
#	external/izsound/std-dsp/ossoutput.cpp \
	external/izsound/std-dsp/pitch.cpp \
	external/izsound/std-dsp/silencer.cpp \
	external/izsound/std-dsp/volume.cpp \
	external/izsound/std-dsp/whitenoise.cpp \
	external/rtaudio/RtAudio.cpp \
	input/decoderbase.cpp \
	input/sndfiledecoder.cpp \
	output/outputobjectbase.cpp \
	output/outputapibase.cpp \
	output/rtaudioapi.cpp \
	output/rtaudiooutobject.cpp 
HEADERS	+=  external/izsound/izsound/izsound/dspunit.h \
	external/izsound/izsound/izsound/izsoundexception.h \
	external/izsound/izsound/izsound/player.h \
	external/izsound/std-dsp/izsound/bandfilter.h \
	external/izsound/std-dsp/izsound/blackhole.h \
	external/izsound/std-dsp/izsound/crossfader.h \
	external/izsound/std-dsp/izsound/datapicker.h \
	external/izsound/std-dsp/izsound/delayextrastereo.h \
	external/izsound/std-dsp/izsound/demultiplexer.h \
	external/izsound/std-dsp/izsound/flanger.h \
	external/izsound/std-dsp/izsound/fragmenter.h \
	external/izsound/std-dsp/izsound/libaooutput.h \
	external/izsound/std-dsp/izsound/maddecoder.h \
	external/izsound/std-dsp/izsound/oggfiledecoder.h \
#	external/izsound/std-dsp/izsound/ossoutput.h \
	external/izsound/std-dsp/izsound/pitch.h \
	external/izsound/std-dsp/izsound/silencer.h \
	external/izsound/std-dsp/izsound/volume.h \
	external/izsound/std-dsp/izsound/whitenoise.h \
	external/rtaudio/RtAudio.h \
	external/rtaudio/RtError.h \
	external/rtaudio/asio/asiodrivers.h \
	external/rtaudio/asio/asio.h \
	external/rtaudio/asio/asiolist.h \
	external/rtaudio/asio/asiosys.h \
	external/rtaudio/asio/ginclude.h \
	common/common_def.h \
	input/decoderbase.h \
	input/sndfiledecoder.h \
	output/outputapibase.h \ 
	output/outputobjectbase.h \
	output/rtaudioapi.h \
	output/rtaudiooutobject.h 


unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

TEMPLATE = lib
VERSION = 0.6.pre0
CONFIG	+= warn_on release
INCLUDEPATH	+= /usr/local/include/izsound /usr/include/izsound input output dsp common external external/izsound/izsound/izsound external/izsound/std-dsp/izsound
LANGUAGE	= C++

include( main.qbas ) 

# Make install -----------------
isEmpty( PREFIX ) {
	PREFIX = /usr/local
	}
lib_objects.files = libose*
lib_objects.path = $${PREFIX}/lib
INSTALLS += lib_objects
# End of Make install ----------


# Pkg-config .pc ---------------
exists( ose.pc.in ) {
	system( rm -f ose.pc )
	system( echo prefix=$${PREFIX} >> ose.pc )
	system( cat ose.pc.in >> ose.pc )
	}
ose_pc.files = ose.pc
ose_pc.path = /usr/local/lib/pkgconfig
INSTALLS += ose_pc
# End of Make install ----------

# Cleanup
QMAKE_CLEAN += ose.pc
