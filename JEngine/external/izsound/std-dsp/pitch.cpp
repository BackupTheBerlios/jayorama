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

#include <pitch.h>

using namespace std;
using namespace izsound;

Pitch::Pitch(const double &ratio, const unsigned int &sampleRate)
  : DspUnit(sampleRate, 1, 1)
{
  setRatio(ratio);
}

Pitch::~Pitch()
{

}

void Pitch::setRatio(const double &ratio)
{
  m_ratio = ratio;
  m_actionCounter = 0;

  double rate = (double)m_sampleRate;
  double diff =  fabs(rate - (ratio * rate));
  if (diff < 0.1 && diff > -0.1)
  {
    m_actionTrigger = 0;
  }
  else
  {
    if (ratio < 0)
    {
      m_actionTrigger = (unsigned int)floor(rate / diff);
    }
    else
    {
      m_actionTrigger = (unsigned int)floor(rate * 2.0 / diff);
    }
  }

}

void Pitch::performDsp()
{
  // Init
  SlotData* input  = m_inSlots[0];
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
  int length = (*input)[0].size();

  // Trivial copy if ratio == 1.0
  if (m_actionTrigger == 0)
  {
    (*output)[0].resize(length);
    (*output)[1].resize(length);
    copy((*input)[0].begin(), (*input)[0].end(), (*output)[0].begin());
    copy((*input)[1].begin(), (*input)[1].end(), (*output)[1].begin());
    return;
  }

  // We process the data
  double left, right;
  if (m_ratio > 1.0) // Tests have a cost, so it's better splitting the loops
  {
    for (int i = 0; i < length; ++i)
    {
      if ((m_actionCounter++ % m_actionTrigger) != 0)
      {
        (*output)[0].push_back((*input)[0][i]);
        (*output)[1].push_back((*input)[1][i]);
      }
    }
  }
  else
  {
    for (int i = 0; i < length; ++i)
    {
      if ((m_actionCounter++ % m_actionTrigger) != 0)
      {
        (*output)[0].push_back((*input)[0][i]);
        (*output)[1].push_back((*input)[1][i]);
      }
      else
      {
        if (i != 0)
        {
          left  = ((*input)[0][i] + (*input)[0][i-1]) / 2.0;
          right = ((*input)[1][i] + (*input)[1][i-1]) / 2.0;
          --i;
        }
        else
        {
          left  = (*input)[0][i];
          right = (*input)[1][i];
        }
        (*output)[0].push_back(left);
        (*output)[1].push_back(right);
      }
    }
  }
}
