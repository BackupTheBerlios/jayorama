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
#include <delayextrastereo.h>

using namespace std;
using namespace izsound;

DelayExtraStereo::DelayExtraStereo(const bool &delayOnRight,
                                   const unsigned int &delayLength,
                                   const unsigned int &sampleRate)
  : DspUnit(sampleRate, 1, 1)
{
  // Inits
  m_delayOnRight = delayOnRight;
  m_delayLength  = delayLength;
  m_buffer.resize(2);

  // We fill the channel used for the delay
  unsigned int nsamples = (m_sampleRate * m_delayLength) / 1000;
  int side = (m_delayOnRight) ? 1 : 0;
  back_insert_iterator<deque<double> > bi(m_buffer[side]);
  fill_n(bi, nsamples, 0.0);
}

DelayExtraStereo::~DelayExtraStereo()
{

}

void DelayExtraStereo::performDsp()
{
  // Init
  SlotData* input = m_inSlots[0];
  SlotData* output = m_outSlots[0];
  back_insert_iterator<deque<double> > bi_left(m_buffer[0]);
  back_insert_iterator<deque<double> > bi_right(m_buffer[1]);

  // We fill the buffer
  copy((*input)[0].begin(), (*input)[0].end(), bi_left);
  copy((*input)[1].begin(), (*input)[1].end(), bi_right);

  // 0-filling if necessary
  int leftSize  = (*input)[0].size();
  int rightSize = (*input)[1].size();
  if (leftSize == 0)
  {
    fill_n(bi_left, rightSize, 0.0);
    leftSize = rightSize;
  }
  else if (rightSize == 0)
  {
    fill_n(bi_right, leftSize, 0.0);
    rightSize = leftSize;
  }

  // We deliver a sound stream chunk
  int nsamples = min(leftSize, rightSize);
  (*output)[0].clear();
  (*output)[1].clear();
  back_insert_iterator<deque<double> > out_left((*output)[0]);
  back_insert_iterator<deque<double> > out_right((*output)[1]);
  copy(m_buffer[0].begin(), m_buffer[0].begin() + nsamples, out_left);
  copy(m_buffer[1].begin(), m_buffer[1].begin() + nsamples, out_right);

  // Clean-up
  for (int i = 0; i < nsamples; ++i)
  {
    m_buffer[0].pop_front();
    m_buffer[1].pop_front();
  }
}
