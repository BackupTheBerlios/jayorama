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

#ifndef IZSOUND_CROSSFADER_H
#define IZSOUND_CROSSFADER_H

#include <math.h>

#include <list>
#include <iterator>
#include <algorithm>

#include <dspunit.h>

using namespace std;

namespace izsound
{

/**
 * A crossfader DSP unit. It mixes 2 sound streams together. The crossfader
 * lets you specify how much each stream must contribute to the output. This
 * unit has two input slots and one output slot.
 *
 * Some buffering is performed because IzSound does not work with fixed size
 * data chuncks. Each slot data first gets inside its buffer. If an input slot
 * does not have any incoming data, then the buffer will be filled with silence.
 * This can happen in case of a buffer underrun or when a unit does not have
 * more sound to produce.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class CrossFader : public DspUnit
{

private:

  /** The first buffer. */
  SlotData m_buffer1;

  /** The second buffer. */
  SlotData m_buffer2;

  /** The crossfader position. */
  double m_crossfaderPosition;

  /** The first stream factor. */
  double m_factor1;

  /** The second stream factor. */
  double m_factor2;

protected:

  /**
   * Mixes the two streams together.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param pos The initial crossfader position.
   * @param sampleRate The audio chain sample rate.
   */
  CrossFader(const double &pos = 0.0,
             const unsigned int &sampleRate = 44100);
  /**
   * The destructor.
   */
  virtual ~CrossFader();

  /**
   * Gets the crossfader position.
   *
   * @return The crossfader position.
   */
  inline double getPosition() const { return m_crossfaderPosition; }

  /**
   * Sets the crossfader position. A value of -1.0 means 100% to the first
   * stream while a value of 1.0 means 100% to the second one. The value 0.0
   * stands for the middle position (50% / 50%).
   *
   * @param pos The new position. If <code>pos</code> is out of the [-1.0 ; 1.0]
   *            range, it will be clipped.
   */
  void setPosition(const double &pos);

};

}

#endif
