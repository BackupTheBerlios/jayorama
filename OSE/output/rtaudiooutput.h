/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* rtaudiooutput.h
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

#ifndef __RTAUDIO_OUTPUT_H
#define __RTAUDIO_OUTPUT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <external/RtAudio.h>
#include <dspunit.h>
#include <izsoundexception.h>

namespace izsound
{

/**
 * This DSP unit is able to output the sound data to an OSS device. OSS is a
 * well supported sound interface on Linux and BSD based systems. The API is
 * actually very simple and efficient. The unit will delibarately run in stereo
 * mode / 16 bits. It is very loose on the initialisation errors handling, if
 * you encounter a problem, write me and I'll clean-up that. The device
 * descriptor release is made when the instance is deleted.
 *
 * The unit provides one input slot and no output slots. A clipping is performed
 * if the sound data values are out of the [-1.0 ; 1.0] range. If we don't do
 * that, the conversion to PCM data will do some weird jumps.
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
class RtAudioOutput : public DspUnit
{
  /** Callback function. Declared as "friend" will do the 
   *  processing work.
   */ 
  friend int rtCallback(char *outputBuffer, int framesPerBuffer, void *outputObject);

private:

  /** The RtAudio object. */
  RtAudio *audio;
  
  /** The first DspUnit of the chain, to be used in callback mode. */
  DspUnit *callbackUnit;
  
//    /**
//    * Output modes.
//    */
//   enum OutputMode {CALLBACK, BLOCKING};
// 
//   /**
//    * The output current mode.
//    */
//   OutputMode out_mode;

  /** The PCM data buffer. */
  char* m_pcmBuffer;
  
  /** Number of ringbuffer partitions. Here latency control. */
  int m_nBuffers;

  /** The current buffer position. */
  unsigned int m_pcmBufferPosition;

  /** The PCM data number of samples. */
  unsigned int m_pcmBufferNSamples;

  /** The PCM data buffer size. */
  unsigned int m_pcmBufferSize;

  /**
   * Writes <code>double</code> sound data to the PCM buffer, at the current
   * position. It writes to the device if needed. It also performs the
   * clipping job.
   * @param left The left value.
   * @param right The right value.
   */
  inline void writeToBuffer(double &left, double &right);

  /**
   * Writes the PCM data to the device.
   */
  inline void writeToDevice();

protected:

  /**
   * Outputs the received data to the device.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param callbackUnit The first unit of the chain, called from the 
   * 			 callback function (this should allow some
   * 			 latency improvement).
   * @param sampleRate The sample rate to use.
   * @param bufferTime The time length of the buffer, in milliseconds.
   * @throw IzSoundException An exception is thrown if it is not possible to
   *        open the OSS device.
   */
  RtAudioOutput( /*DspUnit *callbackUnit = NULL, */
            const unsigned int &sampleRate = 44100,
            const unsigned int &bufferTime = 400,
	    const unsigned int &nBuffers = 6,
	    int device_num = 0 ) throw(IzSoundException);

  /**
   * The destructor.
   */
  virtual ~RtAudioOutput();
  
  /**
   * Get the number of output devices.
   */
  inline int getDeviceNumber() { return audio->getDeviceCount(); }

  /**
   * Flushes the remaining data in the buffer.
   */
  void flush();

};



}

#endif
