SOURCES	+= main.cpp \
	../external/izsound/izsound/dspunit.cpp \
	../external/izsound/std-dsp/bandfilter.cpp \
	../external/izsound/std-dsp/blackhole.cpp \
	../external/izsound/std-dsp/crossfader.cpp \
	../external/izsound/std-dsp/datapicker.cpp \
	../external/izsound/std-dsp/delayextrastereo.cpp \
	../external/izsound/std-dsp/demultiplexer.cpp \
	../external/izsound/std-dsp/flanger.cpp \
	../external/izsound/std-dsp/fragmenter.cpp \
	../external/izsound/std-dsp/libaooutput.cpp \
	../external/izsound/std-dsp/maddecoder.cpp \
	../external/izsound/std-dsp/oggfiledecoder.cpp \
	../external/izsound/std-dsp/ossoutput.cpp \
	../external/izsound/std-dsp/pitch.cpp \
	../external/izsound/std-dsp/silencer.cpp \
	../external/izsound/std-dsp/volume.cpp \
	../external/izsound/std-dsp/whitenoise.cpp \
	../external/rtaudio/RtAudio.cpp \
	../input/decoderbase.cpp \
	../input/sndfiledecoder.cpp \
	../output/outputbase.cpp \ 
	../output/rtaudiooutput.cpp
# 	../input/mpegdecoder.cpp \
#	../input/decmpadecoder.cpp 
HEADERS	+= ../external/izsound/izsound/izsound/dspunit.h \
	../external/izsound/izsound/izsound/izsoundexception.h \
	../external/izsound/izsound/izsound/player.h \
	../external/izsound/std-dsp/izsound/bandfilter.h \
	../external/izsound/std-dsp/izsound/blackhole.h \
	../external/izsound/std-dsp/izsound/crossfader.h \
	../external/izsound/std-dsp/izsound/datapicker.h \
	../external/izsound/std-dsp/izsound/delayextrastereo.h \
	../external/izsound/std-dsp/izsound/demultiplexer.h \
	../external/izsound/std-dsp/izsound/flanger.h \
	../external/izsound/std-dsp/izsound/fragmenter.h \
	../external/izsound/std-dsp/izsound/libaooutput.h \
	../external/izsound/std-dsp/izsound/maddecoder.h \
	../external/izsound/std-dsp/izsound/oggfiledecoder.h \
	../external/izsound/std-dsp/izsound/ossoutput.h \
	../external/izsound/std-dsp/izsound/pitch.h \
	../external/izsound/std-dsp/izsound/silencer.h \
	../external/izsound/std-dsp/izsound/volume.h \
	../external/izsound/std-dsp/izsound/whitenoise.h \
	../external/rtaudio/RtAudio.h \
	../external/rtaudio/RtError.h \
	../external/rtaudio/asio/asiodrivers.h \
	../external/rtaudio/asio/asio.h \
	../external/rtaudio/asio/asiolist.h \
	../external/rtaudio/asio/asiosys.h \
	../external/rtaudio/asio/ginclude.h \
	../common/common_def.h \
	../input/decoderbase.h \
	../input/sndfiledecoder.h \
	../output/outputbase.h \ 
	../output/rtaudiooutput.h
# 	../input/mpegdecoder.h \
#	../input/decmpadecoder.h 
include( main.qbas )

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
TEMPLATE	=app
CONFIG	+= qt warn_on release
INCLUDEPATH	+= /usr/local/include/izsound /usr/include/izsound ../input ../output ../dsp ../common ../external ../external/izsound/izsound/izsound ../external/izsound/std-dsp/izsound
LANGUAGE	= C++
#LIBS += ../external/decmpa/libdecmpa.a
