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

#ifndef IZSOUND_SILENCER_H
#define IZSOUND_SILENCER_H

#include <algorithm>

#include <dspunit.h>

namespace izsound
{

/**
 * This unit produces silence, ie buffers made of 0.0 values. It has no input
 * slots and one output slot.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class Silencer : public DspUnit
{

private:

  /** The number of samples to produce each time. */
  unsigned int m_dataLength;

protected:

  /**
   * Produces the silence.
   */
  virtual void performDsp();
  
public:

  /**
   * The constructor.
   *
   * @param dataLength The number of samples to produce each time.
   * @param sampleRate The audio chain sample rate.
   */
  Silencer(const unsigned int &dataLength = 4096,
          const unsigned int &sampleRate = 44100)
    : DspUnit(sampleRate, 0, 1)
  {
    m_dataLength = dataLength;
  }

  /**
   * The destructor.
   */
  virtual ~Silencer() { }
  
  /**
   * Gets the number of samples to produce each time.
   *
   * @return The number of produced samples.
   */
  inline unsigned int getDataLength() { return m_dataLength; }

  /**
   * Sets the number of samples to produce each time.
   *
   * @param dataLength The new number of samples to produce each time.
   */
  inline void setDataLength(const unsigned int &dataLength)
  {
    m_dataLength = dataLength;
  }
        
};
        
}

#endif
