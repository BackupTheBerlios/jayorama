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

#include <crossfader.h>

using namespace std;
using namespace izsound;

/*
 * Binary functor that mixes 2 samples against factors.
 */
struct Mixer_OF
{
  double f1;
  double f2;

  Mixer_OF(const double &a, const double &b)
  {
    f1 = a;
    f2 = b;
  }

  double operator() (const double &x, const double &y)
  {
    return (x * f1) + (y * f2);
  }
};

CrossFader::CrossFader(const double &pos, const unsigned int &sampleRate)
  : DspUnit(sampleRate, 2, 1)
{
  setPosition(pos);
  m_buffer1.resize(2);
  m_buffer2.resize(2);
}

CrossFader::~CrossFader()
{

}

void CrossFader::setPosition(const double &pos)
{
  if (pos > 1.0)
    m_crossfaderPosition = 1.0;
  else if (pos < -1.0)
    m_crossfaderPosition = -1.0;
  else
    m_crossfaderPosition = pos;

  m_factor1 = -0.5 * m_crossfaderPosition + 0.5;
  m_factor2 =  0.5 * m_crossfaderPosition + 0.5;
}

void CrossFader::performDsp()
{
  // Init
  SlotData* input1 = m_inSlots[0];
  SlotData* input2 = m_inSlots[1];
  SlotData* output = m_outSlots[0];
  back_insert_iterator<deque<double> > biLeft1(m_buffer1[0]);
  back_insert_iterator<deque<double> > biRight1(m_buffer1[1]);
  back_insert_iterator<deque<double> > biLeft2(m_buffer2[0]);
  back_insert_iterator<deque<double> > biRight2(m_buffer2[1]);

  // We first grab the data and put it in the buffers
  copy((*input1)[0].begin(), (*input1)[0].end(), biLeft1);
  copy((*input1)[1].begin(), (*input1)[1].end(), biRight1);
  copy((*input2)[0].begin(), (*input2)[0].end(), biLeft2);
  copy((*input2)[1].begin(), (*input2)[1].end(), biRight2);

  // 0-filling if necessary
  int inSize1 = (*input1)[0].size();
  int inSize2 = (*input2)[0].size();
  if (inSize1 == 0)
  {
    int toFill = inSize2 - m_buffer1[0].size();
    fill_n(biLeft1, toFill, 0.0);
    fill_n(biRight1, toFill, 0.0);
  } else if (inSize2 == 0)
  {
    int toFill = inSize1 - m_buffer2[0].size();
    fill_n(biLeft2, toFill, 0.0);
    fill_n(biRight2, toFill, 0.0);
  }

  // Let's mix
  int nsamples = min(m_buffer1[0].size(), m_buffer2[0].size());
  (*output)[0].clear();
  (*output)[1].clear();
  deque<double>::iterator b1;
  deque<double>::iterator b2;
  back_insert_iterator<deque<double> > lit((*output)[0]);
  back_insert_iterator<deque<double> > rit((*output)[1]);
  b1 = m_buffer1[0].begin();
  b2 = m_buffer2[0].begin();
  transform(b1, b1 + nsamples, b2, lit, Mixer_OF(m_factor1, m_factor2));
  b1 = m_buffer1[1].begin();
  b2 = m_buffer2[1].begin();
  transform(b1, b1 + nsamples, b2, rit, Mixer_OF(m_factor1, m_factor2));

  // Clean-up
  for (int i = 0; i < nsamples; ++i)
  {
    m_buffer1[0].pop_front();
    m_buffer1[1].pop_front();
    m_buffer2[0].pop_front();
    m_buffer2[1].pop_front();
  }
}
