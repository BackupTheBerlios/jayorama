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

#ifndef IZSOUND_DELAYEXTRASTEREO_H
#define IZSOUND_DELAYEXTRASTEREO_H

#include <iterator>
#include <algorithm>

#include <dspunit.h>

namespace izsound
{

/**
 * A DSP unit that adds some stereo-like effect by delaying one of the stereo
 * channels. This is a famous effect that works well with voices for instance.
 *
 * This DSP unit has one input slot and one output slot.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class DelayExtraStereo : public DspUnit
{

private:

  /**
   * Indicates wether the delay should be applied on the right channel
   * or not.
   */
  bool m_delayOnRight;

  /** The delay length, in milliseconds. */
  unsigned int m_delayLength;

  /** The buffer. */
  SlotData m_buffer;

protected:

  /**
   * Processes the sound stream to apply the delaying effect.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param delayOnRight <code>true</code> if you want the delay to be on the
   *                     right channel, <code>false</code> if you want it on
   *                     the left channel.
   * @param delayLength The length of the delay to apply.
   * @param sampleRate The audio chain sample rate.
   */
  DelayExtraStereo(const bool &delayOnRight = true,
                   const unsigned int &delayLength = 25,
                   const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~DelayExtraStereo();

};

}

#endif
