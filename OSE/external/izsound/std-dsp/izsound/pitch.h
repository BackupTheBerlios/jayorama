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

#ifndef IZSOUND_PITCH_H
#define IZSOUND_PITCH_H

#include <math.h>

#include <algorithm>


#include <dspunit.h>

namespace izsound
{

/**
 * This DSP unit is able to change the pitch of a stream against a ratio.
 * This unit has one input slot and one output slot. The ratio controls how
 * the unit works:
 * <ul>
 *  <li>less than 1.0 will slow down</li>
 *  <li>1.0 does not affect the sound</li>
 *  <li>greater than 1.0 will accelerate.</li>
 * </ul>
 * The frequencies will be modified by the ratio, this unit does not preserve
 * them.
 *
 * The pitching change method used is quite simple. To speed-up, some samples
 * are removed at a fixed rate. To slow down, some are added at a fixed rate
 * by computing the average of the current sample with its predecessor.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class Pitch : public DspUnit
{

private:

  /** The ratio. */
  double m_ratio;

  /** Pitching action trigger. */
  unsigned int m_actionTrigger;

  /** Pitching action counter. */
  unsigned int m_actionCounter;

protected:

  /**
   * Performs the pitching operation.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param ratio The initial ratio.
   * @param sampleRate The audio chain sample rate.
   */
  Pitch(const double &ratio = 1.0, const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~Pitch();

  /**
   * Sets the ratio.
   *
   * @param ratio The new pitching ratio.

   */
  void setRatio(const double &ratio);

  /**
   * Gets the ratio.
   *
   * @return The ratio.
   */
  inline double getRatio() const { return m_ratio; }

};

}

#endif
