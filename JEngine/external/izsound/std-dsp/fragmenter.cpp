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

#include <fragmenter.h>

using namespace std;
using namespace izsound;

Fragmenter::Fragmenter(const unsigned int &length,
                       const unsigned int &sampleRate)
  : DspUnit(sampleRate, 1, 1)
{
  // Inits
  m_buffer.resize(2);
  setLength(length);
}

Fragmenter::~Fragmenter()
{

}

void Fragmenter::performDsp()
{
  // Inits
  SlotData *input  = m_inSlots[0];
  SlotData *output = m_outSlots[0];

  // We copy to the buffer
  back_insert_iterator<deque<double> > biLeft1(m_buffer[0]);
  back_insert_iterator<deque<double> > biRight1(m_buffer[1]);
  copy((*input)[0].begin(), (*input)[0].end(), biLeft1);
  copy((*input)[1].begin(), (*input)[1].end(), biRight1);

  // We deliver the data
  (*output)[0].clear();
  (*output)[1].clear();
  if (m_buffer[0].size() > m_trigger)
  {
    back_insert_iterator<deque<double> > biLeft2((*output)[0]);
    back_insert_iterator<deque<double> > biRight2((*output)[1]);
    copy(m_buffer[0].begin(), m_buffer[0].begin() + m_trigger, biLeft2);
    copy(m_buffer[1].begin(), m_buffer[1].begin() + m_trigger, biRight2);
    for (unsigned int i = 0; i < m_trigger; ++i)
    {
      m_buffer[0].pop_front();
      m_buffer[1].pop_front();
    }
  }
}

void Fragmenter::setLength(const unsigned int &length)
{
  m_trigger = (m_sampleRate * length) / 1000;
}
