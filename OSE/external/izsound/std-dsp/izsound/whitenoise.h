/*
 * IzSound - Copyright (c) 2003, 2004 Julien Ponge - All rights reserved.
 *
 * WhiteNoise DSP unit - Copyright (c) 2004 Karl Pitrich
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

#ifndef IZSOUND_WHITENOISE_H
#define IZSOUND_WHITENOISE_H

#include "math.h"

#include <deque>
#include <dspunit.h>

namespace izsound
{

/**
 * A Gaussian White Noise Generator.
 *
 * This unit has one input slot and one output slot.
 *
 * @author Karl PITRICH <pit@root.at>
 */
class WhiteNoise : public DspUnit
{

private:

  /** States wether to produce the noise immediatly or not. */
  bool m_active;

protected:

  /**
   * Generates the Gaussian white noise.
   */
  virtual void performDsp();

public:

  /**
   * Constructs a Whitenoise source.
   *
   * @param active Activate noise immediately
   *        (deactivated by default to protect your ears).
   * @param sampleRate The audio chain sample rate.
   */
  WhiteNoise(bool active = false, const unsigned int &sampleRate = 44100);

  /**
   * The desctructor.
   */
  virtual ~WhiteNoise();

  /**
   * See if the noise source is active.
   *
   * @return Wether the noise source is active or not.
   */
  virtual const bool isActive(void) const { return m_active; }

  /**
   * Activate white noise output
   *
   * @param active Sets wether the white noise is produced or not.
   */
  virtual void setActive(bool active=true) { m_active = active; }

};

}

#endif
