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

#ifndef IZSOUND_FLANGER_H
#define IZSOUND_FLANGER_H

#include <iterator>
#include <algorithm>
#include <functional>

#include <dspunit.h>

namespace izsound
{

/**
 * A DSP unit that applies a flanger effect on a sound stream.
 * This unit has one input slot and one output slot.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class Flanger : public DspUnit
{

private:

  /** The flanger frequency. */
  double m_frequency;

  /** The flanger effect amplitude. */
  double m_amplitude;

  /** The flanger wet ratio. */
  double m_wet;

  /** The flanger dry ratio. */
  double m_dry;
  
  /** The internal original data buffer. */
  SlotData m_originalBuffer;

  /** The internal pitched data buffer. */
  SlotData m_pitchedBuffer;
  
  /** The internal flanger state. */
  int m_internalState;
  
  /** The next number of samples per period. */
  unsigned int m_periodNextSamplesCount;
  
  /** The number of samples per period. */
  unsigned int m_periodSamplesCount;
  
  /** The number of samples for a half period. */
  unsigned int m_halfPeriodSamplesCount;

  /** The internal samples counter. */
  unsigned int m_samplesCounter;

  /** Tells the number of samples to process before a pitching is done. */
  unsigned int m_pitchTrigger;

protected:

  /**
   * Processes the sound and applies the flanger effect.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param frequency The flanger frequency.
   * @param amplitude The effect amplitude in the [0, 1] range.
   * @param wet The wet amount in the [0, 1] range.
   * @param sampleRate The audio chain sample rate.
   */
  Flanger(const double &frequency = 0.25,
          const double &amplitude = 0.001,
          const double &wet = 0.5,
          const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~Flanger();

  /**
   * Gets the flanger frequency.
   *
   * @return The flanger frequency.
   */
  inline double getFrequency() const { return m_frequency; }

  /**
   * Gets the flanger amplitude.
   *
   * @return The flanger amplitude.
   */
  inline double getAmplitude() const { return m_amplitude; }

  /**
   * Gets the flanger wet amount.
   *
   * @return The flanger wet amount.
   */
  inline double getWet() const { return m_wet; }

  /**
   * Sets the flanger frequency.
   *
   * @param frequency The new frequency.
   */
  void setFrequency(const double &frequency);

  /**
   * Sets the flanger amplitude. A low amplitude such as 0.001 gives great
   * results. It is used to compute the pitching amount. Must be in the [0, 1]
   * range.
   *
   * @param amplitude The new amplitude.
   */
  void setAmplitude(const double &amplitude);

  /**
   * Sets the flanger wet amount. The dry ratio will be (1 - wet). Must be in
   * the [0, 1] range.
   *
   * @param wet The new wet amount.
   */
  void setWet(const double &wet);

  /**
   * Resets the unit.
   * Redefines <code>izsound::DspUnit::reset()</code>.
   */
  virtual void reset();

};

}

#endif
