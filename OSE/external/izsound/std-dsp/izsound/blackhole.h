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

#ifndef IZSOUND_BLACKHOLE_H
#define IZSOUND_BLACKHOLE_H

#include <numeric>
#include <iostream>

#include <dspunit.h>

namespace izsound
{

/**
 * This DSP unit is used on the end of a chain to grab the data. It does
 * nothing interessting, all the collected data is not used. This is mainly
 * made for testing purposes as well as to make a DSP unit sample. It has one
 * input slot and no output slots.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class Blackhole : public DspUnit
{
private:

  /** Tells wether a trace is required or not. */
  bool m_trace;

protected:

  /**
   * Performs the job. Does nothing useful. If the tracing is activated, then
   * it writes on <code>stdout</code> the number of samples received.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param trace Specifies wether the unit should trace its activity or not.
   * @param sampleRate The audio chain sample rate.
   */
  inline Blackhole(const bool &trace, const unsigned int &sampleRate = 44100)
    : DspUnit(sampleRate, 1, 0)
  {
    m_trace = trace;
  }

  /**
   * The destructor.
   */
  virtual ~Blackhole() { }

};

}

#endif
