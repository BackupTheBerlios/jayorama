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

#ifndef IZSOUND_LIBAOOUTPUT_H
#define IZSOUND_LIBAOOUTPUT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <stdio.h>

#include <ao/ao.h>

#include <dspunit.h>
#include <izsoundexception.h>

#define BUFFER_SIZE 4096

namespace izsound
{

/**
 * Uses the libao facilities to output the sound data. It can output the sound
 * to various kinds of Unix-like systems (oss, alsa, sun audio, ...) and even
 * to files (raw, au, wav). It has one input slot and no output slot. You
 * can load a driver explicitely or use the default libao settings. A brutal
 * clipping is performed if the sound data is not in the [-1 ; 1] range.
 *
 * You will have to take a look at the libao documentation to pass the drivers
 * names and options. Honestly, there is no need to write C++ wrappers, the
 * libao library is a very clean and easy C API.
 *
 * This unit has one input slot and no output slot.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class LibaoOutput : public DspUnit
{

private:

  /** The driver id. */
  int m_driverId;

  /** The output device. */
  ao_device* m_device;

  /** The libao units counter. */
  static int m_instancesCounter;

  /** Our buffer. */
  char* m_pcmBuffer;

  /** The current pcm buffer position. */
  unsigned int m_pcmBufferPosition;

protected:

  /**
   * Performs the DSP job.
   */
  virtual void performDsp();

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

public:

  /**
   * The constructor for a live output device. Please refer to the libao docs.
   *
   * @param driver The short driver name. Set it to <code>0</code> if you want
   *               to use the libao default autodetected driver.
   * @param options The driver options (can be <code>0</code> for the defaults).
   * @param sampleRate The audio chain sample rate.
   * @throw IzSoundException An exception is thrown is the libao fails to open
   *        the desired device.
   */
  LibaoOutput(const char* driver, ao_option* options,
              const unsigned int &sampleRate = 44100) throw(IzSoundException);

  /**
   * The constructor for a file output device. Please refer to the libao docs.
   *
   * @param driver The short driver name.
   * @param options The driver options (can be <code>0</code> for the defaults).
   * @param filename The file name.
   * @param sampleRate The audio chain sample rate.
   * @throw IzSoundException An exception is thrown is the libao fails to open
   *        the desired device.
   */
  LibaoOutput(const char* driver, ao_option* options, const char* filename,
              const unsigned int &sampleRate = 44100) throw(IzSoundException);

  /**
   * The destructor.
   */
  virtual ~LibaoOutput();

  /**
   * Flushes the remaining data in the buffer.
   */
  void flush();

};

}

#endif
