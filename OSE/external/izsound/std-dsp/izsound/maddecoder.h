/*
 * IzSound - Copyright (c) 2003, 2004 Julien Ponge - All rights reserved.
 *
 * Portions are:
 * - Copyright (c) 2000-2003 Underbit Technologies, Inc. [minimad.c]
 * - Copyright (c) 1995-1999 Michael Hipp. [xmms mpg123 plugin]
 * - Copyright (c) 2004 Karl Pitrich.
 *
 * The MadDecoder DSP unit (izsound/maddecoder.h + maddecoder.cpp) is published
 * under the terms of the following license agreement:
 *
 * -----------------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -----------------------------------------------------------------------------
 *
 * A copy of the GNU General Public License is available in the IzSound source
 * code distribution (COPYING.GPL).
 */

/*
 * The MadDecoder DSP unit implementation is based on the madlld program from
 * Bertrand Petit: http://www.bsd-dk.dk/~elrond/audio/madlld/ .
 */

/****************************************************************************
 * madlld.c -- A simple program decoding an mpeg audio stream to 16-bit     *
 * PCM from stdin to stdout. This program is just a simple sample           *
 * demonstrating how the low-level libmad API can be used.                  *
 *--------------------------------------------------------------------------*
 * (c) 2001, 2002 Bertrand Petit                                            *
 *                                                                          *
 * Redistribution and use in source and binary forms, with or without       *
 * modification, are permitted provided that the following conditions       *
 * are met:                                                                 *
 *                                                                          *
 * 1. Redistributions of source code must retain the above copyright        *
 *    notice, this list of conditions and the following disclaimer.         *
 *                                                                          *
 * 2. Redistributions in binary form must reproduce the above               *
 *    copyright notice, this list of conditions and the following           *
 *    disclaimer in the documentation and/or other materials provided       *
 *    with the distribution.                                                *
 *                                                                          *
 * 3. Neither the name of the author nor the names of its contributors      *
 *    may be used to endorse or promote products derived from this          *
 *    software without specific prior written permission.                   *
 *                                                                          *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''       *
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED        *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A          *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR       *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,             *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF         *
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,       *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT       *
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       *
 * SUCH DAMAGE.                                                             *
 *                                                                          *
 ****************************************************************************/

#ifndef IZSOUND_MADDECODER_H
#define IZSOUND_MADDECODER_H

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <mad.h>

#include <deque>

#include <player.h>
#include <dspunit.h>

#define INPUT_BUFFER_SIZE	(5*8192)

namespace izsound
{

/**
 * A MPEG files decoder DSP unit. It uses the Mad library to perform the job.
 *
 * This unit has no input slot and one output slot.
 *
 * @author Julien PONGE <julien@izforge.com>
 * @author Karl PITRICH <pit@root.at>
 */
class MadDecoder : public DspUnit, public virtual Player
{

private:
        
  /** Playing status enumeration. */
  enum PlayingState {PLAY, PAUSE, STOP};

  /** The player current status. */
  PlayingState m_playerStatus;

  /** Memory mapped file pointer. */
  unsigned char *m_fdm;
  
  /** Offset from start of mmap area to currently playing part. */
  unsigned long m_offset;

  /** The frames counter. */
  unsigned long m_frameCount;

  /** libmad stream. */
  struct mad_stream	m_stream;

  /** libmad frame. */
  struct mad_frame m_frame;

  /** libmad synth. */
  struct mad_synth m_synth;

  /** libmad timer. */
  mad_timer_t m_timer;

  /** The input file. */
  int m_inputFile;
  
  /** The file size (counted in number of 'unsigned int'). */
  unsigned long m_fileSize;

  /** True when the end of the file is reached. */
  bool m_endReached;

  /** The current file total time, in seconds. */
  double m_totalTime;

  /**
   * Performs the constructors common initialisation job.
   */
  void commonInits();

  /**
   * Initializes the libmad structures.
   */
  void initMadStructs();

  /**
   * Releases the libmad structures.
   */
  void releaseMadStructs();

  /**
   * Closes the current file.
   */
  void closeFile();

  /**
   * Plays the next chunck of data.
   */
  void playNextChunk();

  /**
   * Plays nothing.
   */
  void playNothing();

  /**
   * Converts a sample from mad's fixed point number format to an unsigned
   * short (16 bits).
   *
   * @param Fixed The fixed point number to convert.
   * @return The converted number.
   */
  inline int scale(mad_fixed_t sample);

  /**
   * Gets the song length.
   *
   * @return The song length, in bytes.
   */
  long getSongLength();

  /**
   * Gets the song time.
   *
   * @return The song time, in seconds.
   */
  double getSongTime();
       
protected:

  /**
   * Performs the decoding job.
   */
  virtual void performDsp();

public:

  /**
   * Constructs a decoder with no file loaded.
   *
   * @param sampleRate The audio chain sample rate.
   */
  MadDecoder(const unsigned int &sampleRate = 44100);

  /**
   * Constructs a decoder with a file to open.
   *
   * @param filename The file to open.
   * @param sampleRate The audio chain sample rate.
   * @throw IzSoundException An exception is thrown if the file opening cannot
   *        be properly done.
   */
  MadDecoder(const char* filename,
             const unsigned int &sampleRate = 44100) throw(IzSoundException) ;

  /**
   * The desctructor.
   */
  virtual ~MadDecoder();

  /**
   * Play operation. The expected behaviour is to start playing from the
   * current position.
   */
  virtual void play();

  /**
   * Pause operation.
   */
  virtual void pause();

  /**
   * Stop operation. The expected behaviour is to go back to the beginning of
   * the source.
   */
  virtual void stop();

  /**
   * Opens a file from a filename.
   *
   * @param filename The filename.
   * @throw IzSoundException An exception is thrown when the opening operation
   *                         process encounters a failure.
   */
  virtual void open(const char* filename) throw(IzSoundException);

  /**
   * Indicates if we have reached the end of the stream or not.
   *
   * @return <code>true</code> if we have reached the end, <code>false</code>
   *         otherwise.
   */
  virtual bool isEndReached();

  /**
   * Returns the total stream playing time, in seconds.
   *
   * @return The total playing time.
   */
  virtual double getTotalTime();

  /**
   * Seeks to a position.
   *
   * @param pos The position in seconds.
   */
  virtual void seek(const double &pos);

  /**
   * Gets the current time, in seconds.
   *
   * @return The current time, in seconds.
   */
  virtual double getCurrentTime();
};

}

#endif
