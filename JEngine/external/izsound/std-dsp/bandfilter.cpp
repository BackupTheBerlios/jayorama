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

#include <bandfilter.h>

using namespace std;
using namespace izsound;

BandFilter::BandFilter(const unsigned int &sampleRate)
   : DspUnit(sampleRate, 1, 1)
{
  // BW Init
  m_bw[0].x[0] = 0.0;
  m_bw[0].x[1] = 0.0;
  m_bw[0].y[0] = 0.0;
  m_bw[0].y[1] = 0.0;
  m_bw[1].x[0] = 0.0;
  m_bw[1].x[1] = 0.0;
  m_bw[1].y[0] = 0.0;
  m_bw[1].y[1] = 0.0;

  // Default mode
  setLowPassMode(600.0);
}

BandFilter::~BandFilter()
{

}

void BandFilter::performDsp()
{
  // Init
  double inLeft  = 0.0;
  double inRight = 0.0;
  double outLeft  = 0.0;
  double outRight = 0.0;
  SlotData* input  = m_inSlots[0];
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();

  // Let's go
  for (unsigned int i = 0; i < (*input)[0].size(); ++i)
  {
    // Init
    inLeft  = (*input)[0][i];
    inRight = (*input)[1][i];

    // Left channel
    outLeft =  m_bw[0].a[0] * inLeft +
               m_bw[0].a[1] * m_bw[0].x[0] +
               m_bw[0].a[2] * m_bw[0].x[1] -
               m_bw[0].b[0] * m_bw[0].y[0] -
               m_bw[0].b[1] * m_bw[0].y[1];
    m_bw[0].x[1] = m_bw[0].x[0];
    m_bw[0].x[0] = inLeft;
    m_bw[0].y[1] = m_bw[0].y[0];
    m_bw[0].y[0] = outLeft;

    // Right channel
    outRight =  m_bw[1].a[0] * inRight +
                m_bw[1].a[1] * m_bw[1].x[0] +
                m_bw[1].a[2] * m_bw[1].x[1] -
                m_bw[1].b[0] * m_bw[1].y[0] -
                m_bw[1].b[1] * m_bw[1].y[1];
    m_bw[1].x[1] = m_bw[1].x[0];
    m_bw[1].x[0] = inRight;
    m_bw[1].y[1] = m_bw[1].y[0];
    m_bw[1].y[0] = outRight;

    // Clipping
    if (outLeft >= 1.0)
    {
      outLeft = CLIPPING_VALUE;
    }
    else if (outLeft <= -1.0)
    {
      outLeft = -CLIPPING_VALUE;
    }
    if (outRight >= 1.0)
    {
      outRight = CLIPPING_VALUE;
    }
    else if (outRight <= -1.0)
    {
      outRight = -CLIPPING_VALUE;
    }

    // Push it back
    (*output)[0].push_back(outLeft);
    (*output)[1].push_back(outRight);
  }
}

void BandFilter::setLowPassMode(const double &frequency)
{
  double c = 1.0 / tan(M_PI * frequency / (double)m_sampleRate);

  m_bw[0].a[0] = 1.0 / (1.0 + sqrt(2.0) * c + c * c);
  m_bw[0].a[1] = 2.0 * m_bw[0].a[0];
  m_bw[0].a[2] = m_bw[0].a[0];
  m_bw[0].b[0] = 2 * (1.0 - c * c) * m_bw[0].a[0];
  m_bw[0].b[1] = (1.0 - sqrt(2.0) * c + c * c) * m_bw[0].a[0];

  m_bw[1].a[0] = 1.0 / (1.0 + sqrt(2.0) * c + c * c);
  m_bw[1].a[1] = 2.0 * m_bw[1].a[0];
  m_bw[1].a[2] = m_bw[1].a[0];
  m_bw[1].b[0] = 2 * (1.0 - c * c) * m_bw[1].a[0];
  m_bw[1].b[1] = (1.0 - sqrt(2.0) * c + c * c) * m_bw[1].a[0];
}

void BandFilter::setHighPassMode(const double &frequency)
{
  double c = tan(M_PI *frequency / (double)m_sampleRate);

  m_bw[0].a[0] = 1.0 / (1.0 + sqrt(2.0) * c + c * c);
  m_bw[0].a[1] = -2.0 * m_bw[0].a[0];
  m_bw[0].a[2] = m_bw[0].a[0];
  m_bw[0].b[0] = 2 * (c * c - 1.0) * m_bw[0].a[0];
  m_bw[0].b[1] = (1.0 - sqrt(2.0) * c + c * c) * m_bw[0].a[0];

  m_bw[1].a[0] = 1.0 / (1.0 + sqrt(2.0) * c + c * c);
  m_bw[1].a[1] = -2.0 * m_bw[1].a[0];
  m_bw[1].a[2] = m_bw[1].a[0];
  m_bw[1].b[0] = 2 * (c * c - 1.0) * m_bw[1].a[0];
  m_bw[1].b[1] = (1.0 - sqrt(2.0) * c + c * c) * m_bw[1].a[0];
}

void BandFilter::setBandPassMode(const double &frequency, const double &width)
{
  double c = 1.0 / tan(M_PI * width / (double)m_sampleRate);
  double d = 2.0 * cos(2.0 * M_PI * frequency / (double)m_sampleRate);

  m_bw[0].a[0] = 1.0 / (1.0 + c);
  m_bw[0].a[1] = 0.0;
  m_bw[0].a[2] = -m_bw[0].a[0];
  m_bw[0].b[0] = -c * d * m_bw[0].a[0];
  m_bw[0].b[1] = (c - 1.0) * m_bw[0].a[0];

  m_bw[1].a[0] = 1.0 / (1.0 + c);
  m_bw[1].a[1] = 0.0;
  m_bw[1].a[2] = -m_bw[1].a[0];
  m_bw[1].b[0] = -c * d * m_bw[1].a[0];
  m_bw[1].b[1] = (c - 1.0) * m_bw[1].a[0];
}

void BandFilter::setBandRejectMode(const double &frequency, const double &width)
{
  double c = tan(M_PI * width / (double)m_sampleRate);
  double d = 2.0 * cos(2.0 * M_PI * frequency / (double)m_sampleRate);

  m_bw[0].a[0] = 1.0 / (1.0 + c);
  m_bw[0].a[1] = -d * m_bw[0].a[0];
  m_bw[0].a[2] = m_bw[0].a[0];
  m_bw[0].b[0] = m_bw[0].a[1];
  m_bw[0].b[1] = (1.0 - c) * m_bw[0].a[0];

  m_bw[1].a[0] = 1.0 / (1.0 + c);
  m_bw[1].a[1] = -d * m_bw[1].a[0];
  m_bw[1].a[2] = m_bw[1].a[0];
  m_bw[1].b[0] = m_bw[1].a[1];
  m_bw[1].b[1] = (1.0 - c) * m_bw[1].a[0];
}
