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

#ifndef IZSOUND_DATAPICKER_H
#define IZSOUND_DATAPICKER_H

#include <queue>

#include <dspunit.h>

using namespace std;

namespace izsound
{

/**
 * A data picker unit. It periodically picks some data samples. It can be used
 * for instance to draw a graph of the sound stream. The periodicity of the
 * data picking can be put in relation with the desired 'zooming factor'. No
 * extra sound processing is performed, so the sound data remains intact. The
 * data is put in queues that the calling application can use to pop the data.
 *
 * This unit has 1 input slot and 1 output slot.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class DataPicker : public DspUnit
{

private:

  /** The left channel data queue. */
  queue<double> m_leftQueue;

  /** The right channel data queue. */
  queue<double> m_rightQueue;

  /** The data picking interval. */
  unsigned int m_dataPickingInterval;

protected:

  /**
   * Processes the sound stream.
   */
  virtual void performDsp();

public:

  /**
   * The constructor.
   *
   * @param dataPickingInterval The data picking interval.
   * @param sampleRate The sound stream sample rate.
   */
  DataPicker(const unsigned int &dataPickingInterval = 20,
             const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~DataPicker();

  /**
   * Gets the data picking interval, in number of samples.
   *
   * @return The data picking interval.
   */
  inline unsigned int getDataPickingInterval() const
  { return m_dataPickingInterval; }

  /**
   * Sets the data picking interval, in number of samples.
   *
   * @param dataPickingInterval The data picking interval.
   */
  inline void setDataPickingInterval(const unsigned int &dataPickingInterval)
  { m_dataPickingInterval = dataPickingInterval; }

  /**
   * Indicates wether there is any data to pick or not.
   *
   * @return Wether there is any data to pick or not.
   */
  inline bool isDataAvailable() const
  { return !m_leftQueue.empty(); }

  /**
   * Pops a left channel value. Don't call if there is no data available !
   *
   * @return A left channel value, in the [-1;1] range.
   */
  inline double popLeft()
  {
    double data = m_leftQueue.front();
    m_leftQueue.pop();
    return data;
  }

  /**
   * Pops a right channel value. Don't call if there is no data available !
   *
   * @return A right channel value, in the [-1;1] range.
   */
  inline double popRight()
  {
    double data = m_leftQueue.front();
    m_leftQueue.pop();
    return data;
  }

  /**
   * Resets the unit.
   * Redefines <code>izsound::DspUnit::reset()</code>.
   */
  virtual void reset();


};

}

#endif
