/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* mp3filedecoder.h
 *
 *  Copyright (C) 2004 Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
 /*
 * IzSound - Copyright (c) 2003, 2004 Julien PONGE - All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 

#ifndef	_MP3FILEDECODER_H
#define	_MP3FILEDECODER_H



#include <libmpeg3/libmpeg3.h>
#include <aplayer.h>
#include <dspunit.h>
#include <vector>



using namespace std;

namespace izsound
{

/**
 * Decodes an mp3 audio stream using libmpeg3 (http://www.heroinewarrior.com).
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
class Mp3FileDecoder : public DspUnit, public virtual APlayer
{

private:
  
  /** File pointer. */
  mpeg3_t* file;

  /** Stream channels. */
  int channels;
  
  /** Stream samplerate. */
  int samplerate;
  
  /** Stream frames. */
  int frames;
  
  /** Position in frames */
  int frame_position;
  
  /** Nr of frames contained in the buffer */
  int frames_in_buffer;
  
  /** The reading buffer size. */
  unsigned int m_readBufferSize;
  
  /** The reading buffer size in bytes. */
  int m_readBufferSizeBytes;

  /** The reading buffer. */
  float* m_readBuffer_left;
  float* m_readBuffer_right;

  /** Tells wether we've reached the end of the stream or not. */
  bool m_endReached;
  
  /** Stream direction. */
  int m_direction;

  /**
   * Playing status enumeration.
   */
  enum PlayingState {PLAY, PAUSE, STOP};

  /**
   * The player current status.
   */
  PlayingState m_playerStatus;
  

protected:

  /**
   * Performs the decoding job, sending to the next unit one chunk of data.
   */
  virtual void performDsp();

  /**
   * Plays the next current file decoded chunk.
   */
  void playNextChunk();

  /**
   * Plays nothing (clears the next DSP unit buffers).
   */
  void playNothing();

public:

  /**
   * The constructor. This one starts with a file and in the playing state.
   *
   * @param filename The valid path to the file to read.
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @throw IzSoundException An exception is thrown when the file opening
   *         operation fails.
   */
  Mp3FileDecoder(const char* filename,
                 const unsigned int &bufferSize = 4096,
                 const unsigned int &sampleRate = 44100)
  throw(IzSoundException);

  /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   */
  Mp3FileDecoder(const unsigned int &bufferSize = 4096,
                 const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~Mp3FileDecoder();

  /**
   * Tells wether the end of the stream has been reached or not.
   *
   * @return <code>true</code> if we have reached the end, <code>false</code>
   *         otherwise.
   */
  virtual bool isEndReached() { return m_endReached; }

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
   * Returns the total stream playing time, in seconds.
   *
   * @return The total playing time.
   */
  virtual int getTotalTime();
  
  /**
   * Returns the total stream frames.
   *
   * @return The total frames.
   */
  virtual int getFrames();
  
  /**
   * Seek to frame.
   *
   * @param frame The frame position to seek.
   */
  virtual void setFrame(int frame);
  
  /**
   * Returns the current frame.
   *
   * @return The current frame.
   */
  virtual int getFrame();

  /**
   * Seeks to a position.
   *
   * @param pos The position in seconds.
   */
  virtual void setCurrentTime(int pos);

  /**
   * Gets the current time, in seconds.
   *
   * @return The current time, in seconds.
   */
  virtual int getCurrentTime();
  
  /**
   * Set the read direction of the audio stream.
   *
   * @param direction Could be FORWARD or BACKWARD.
   */
  virtual void setReadDirection(int direction);
  

};


}


#endif

