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

#ifndef IZSOUND_OSSOUTPUT_H
#define IZSOUND_OSSOUTPUT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_SYS_FCNTL_H
#include <sys/fcntl.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#endif

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
 * @author Julien PONGE <julien@izforge.com>
 */
class OssOutput : public DspUnit
{

private:

  /** The device descriptor. */
  int m_device;

  /** The PCM data buffer. */
  char* m_pcmBuffer;

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
   * @param sampleRate The sample rate to use.
   * @param bufferTime The time length of the buffer, in milliseconds.
   * @param device The device to open.
   * @throw IzSoundException An exception is thrown if it is not possible to
   *        open the OSS device.
   */
  OssOutput(const unsigned int &sampleRate = 44100,
            const unsigned int &bufferTime = 500,
            const char* device = "/dev/dsp") throw(IzSoundException);

  /**
   * The destructor.
   */
  virtual ~OssOutput();

  /**
   * Flushes the remaining data in the buffer.
   */
  void flush();

};

}

#endif
