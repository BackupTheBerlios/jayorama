/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* aplayer.h
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

#ifndef _APLAYER_H
#define _APLAYER_H

// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif

#include <izsoundexception.h>

#define FORWARD		1
#define BACKWARD	2


using namespace std;

namespace izsound
{

/**
 * Defines an interface for an advanced player. 
 * Every decoder class will inherit APlayer.
 * 
 * WARNING! A "frame" is intended in the libsndfile way,
 * as a group of samples, one for each channel. So, if we have
 * a stereo stream, a frame would have 2 samples.
 * We are using pcm data frames. So if we have a samplerate
 * of 44100 Hz and a stereo stream, each frame plays 1/44100 
 * of second. This is less than a millisecond and our decoder
 * class must be capable to seek frames.
 *
 * @author Julien PONGE <julien@izforge.com>
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
 
 enum AStatus 
 {
 	APLAYER_OK,
	APLAYER_ERROR_UNKNOWN,
	APLAYER_FORMAT_UNKNOWN,
	APLAYER_LOAD_ERROR 
 };
 
 
class APlayer
{

public:

  /**
   * Play operation. The expected behaviour is to start playing from the
   * current position.
   */
  virtual void play() = 0;

  /**
   * Pause operation.
   */
  virtual void pause() = 0;

  /**
   * Stop operation. The expected behaviour is to go back to the beginning of
   * the source.
   */
  virtual void stop() = 0;

  /**
   * Opens a file from a filename.
   *
   * @param filename The filename.
   * @throw IzSoundException An exception is thrown when the opening operation
   *                         process encounters a failure.
   */
  virtual void open(const char* filename) throw(IzSoundException) = 0;

  /**
   * Indicates if we have reached the end of the stream or not.
   *
   * @return <code>true</code> if we have reached the end, <code>false</code>
   *         otherwise.
   */
  virtual bool isEndReached() = 0;

  /**
   * Returns the total stream playing time, in seconds.
   *
   * @return The total playing time.
   */
  virtual int getTotalTime() = 0;
  
  /**
   * Returns the total stream frames.
   *
   * @return The total frames.
   */
  virtual int getFrames() = 0;
  
  /**
   * Seek to frame.
   *
   * @param frame The frame position to seek.
   */
  virtual void setFrame(int frame) = 0;
  
  /**
   * Returns the current frame.
   *
   * @return The current frame.
   */
  virtual int getFrame() = 0;

  /**
   * Seeks to a position in seconds.
   *
   * @param pos The position in seconds.
   */
  virtual void setCurrentTime(int pos) = 0;

  /**
   * Gets the current time, in seconds.
   *
   * @return The current time, in seconds.
   */
  virtual int getCurrentTime() = 0;
  
  /**
   * Set the read direction of the audio stream.
   *
   * @param direction Could be FORWARD or BACKWARD.
   */
  virtual void setReadDirection(int direction) = 0;
  
  private:
  
  /** Status of the decoder unit. **/
  AStatus m_status;

};

}

#endif
