SOURCES	+= src/DecMPA.cpp \
	src/DecMPAFileAccess.cpp \
	src/MPADecoder.cpp \
	src/MPAFrameFinder.cpp \
	src/MPAInfo.cpp \
	src/frame/audioFrame.cpp \
	src/frame/floatFrame.cpp \
	src/frame/frame.cpp \
	src/frame/pcmFrame.cpp \
	src/frame/rawDataBuffer.cpp \
# 	src/hip/DecodeEngine_Hip.cpp \
# 	src/hip/VbrTag.c \
# 	src/hip/common.c \
# 	src/hip/dct64_i386.c \
# 	src/hip/decode_i386.c \
# 	src/hip/interface.c \
# 	src/hip/layer1.c \
# 	src/hip/layer2.c \
# 	src/hip/layer3.c \
# 	src/hip/tabinit.c \
	src/mpegAudioFrame/dxHead.cpp \
	src/mpegAudioFrame/mpegAudioHeader.cpp \
	src/splay/DecodeEngine_SPlay.cpp \
	src/splay/dct36_12.cpp \
	src/splay/dct64.cpp \
	src/splay/dct64_down.cpp \
	src/splay/huffmanlookup.cpp \
	src/splay/huffmantable.cpp \
	src/splay/mpegAudioBitWindow.cpp \
	src/splay/mpegAudioStream.cpp \
	src/splay/mpeglayer1.cpp \
	src/splay/mpeglayer2.cpp \
	src/splay/mpeglayer3.cpp \
	src/splay/mpegtable.cpp \
	src/splay/mpegtoraw.cpp \
	src/splay/splayDecoder.cpp \
	src/splay/synth_Down.cpp \
	src/splay/synth_Std.cpp \
	src/splay/synth_filter.cpp \
	src/splay/synthesis.cpp \
	src/splay/window.cpp
HEADERS	+= include/decmpa.h \
	src/DecMPAFileAccess.h \
	src/DecodeEngine.h \
	src/DefInc.h \
	src/IFileAccess.h \
	src/MPADecoder.h \
	src/MPAFrameFinder.h \
	src/MPAInfo.h \
	src/MemBuffer.h \
	src/frame/audioFrame.h \
	src/frame/floatFrame.h \
	src/frame/frame.h \
	src/frame/pcmFrame.h \
	src/frame/rawDataBuffer.h \
# 	src/hip/HIPDefines.h \
# 	src/hip/VbrTag.h \
# 	src/hip/common.h \
# 	src/hip/dct64_i386.h \
# 	src/hip/decode_i386.h \
# 	src/hip/huffman.h \
# 	src/hip/interface.h \
# 	src/hip/l2tables.h \
# 	src/hip/layer1.h \
# 	src/hip/layer2.h \
# 	src/hip/layer3.h \
# 	src/hip/mpg123.h \
# 	src/hip/mpglib.h \
# 	src/hip/tabinit.h \
	src/mpegAudioFrame/dxHead.h \
	src/mpegAudioFrame/mpegAudioHeader.h \
	src/splay/attribute.h \
	src/splay/common.h \
	src/splay/dct.h \
	src/splay/huffmanlookup.h \
	src/splay/mpeg2tables.h \
	src/splay/mpegAudioBitWindow.h \
	src/splay/mpegAudioStream.h \
	src/splay/mpegsound.h \
	src/splay/op.h \
	src/splay/splayDecoder.h \
	src/splay/synthesis.h \
	src/splay/window.h

include( main.qbas )

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
} 

TEMPLATE	= lib
TARGET		= decmpa
VERSION		= 0.4.1
LANGUAGE	= C++
CONFIG		= staticlib
