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

/*
 * I don't have the theorical knowledge to implement these filters. As a
 * consequence, I took the formulas from the Sox butterworth effect
 * implementation.
 *
 * The Sox implementation was made by Jan Paul Schmidt who based his work on
 * Kai Lassfolk's "Sound Processing Kit" ... who himself took his inspiration
 * in the following book:
 *
 * Computer music: synthesis, composition, and performance
 * Charles Dodge, Thomas A. Jerse
 * [2nd ed.]
 * Page 214
 */

#ifndef IZSOUND_BANDFILTER_H
#define IZSOUND_BANDFILTER_H

#include <math.h>

#include <dspunit.h>

namespace izsound
{

/*
 * Used internally by the <code>BandFilter</code> class.
 */
typedef struct butterworth
{
  double x [2];
  double y [2];
  double a [3];
  double b [2];
} butterworth_t;

/**
 * A multi-purpose frequency band filter DSP unit. It can do the following
 * operations:
 * <ul>
 *  <li>low-pass filtering</li>
 *  <li>high-pass filtering</li>
 *  <li>band-pass filtering</li>
 *  <li>band-reject filtering.</li>
 * </ul>
 *
 * This DSP unit has one input slot and one output slot. By default, it will
 * work in low-pass mode at 600hz.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class BandFilter : public DspUnit
{

private:

  /** The butterworth tables (left and right). */
  butterworth_t m_bw[2];

protected:

  /**
   * Performs the filtering job.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param sampleRate The audio stream sample rate.
   */
  BandFilter(const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~BandFilter();

  /**
   * Puts the unit in low-pass mode.
   *
   * @param frequency The filtering frequency.
   */
  void setLowPassMode(const double &frequency);

  /**
   * Puts the unit in high-pass mode.
   *
   * @param frequency The filtering frequency.
   */
  void setHighPassMode(const double &frequency);

  /**
   * Puts the unit in band-pass mode.
   *
   * @param frequency The center frequency.
   * @param width The filtering bandwidth.
   */
  void setBandPassMode(const double &frequency, const double &width);

  /**
   * Puts the unit in band-reject mode.
   *
   * @param frequency The center frequency.
   * @param width The filtering bandwidth.
   */
  void setBandRejectMode(const double &frequency, const double &width);

};

}

#endif
