/*
 * IzSound - Copyright (c) 2003, 2004 Julien Ponge - All rights reserved.
 *
 * WhiteNoise DSP unit - Copyright (c) 2004 Karl Pitrich
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

#include "whitenoise.h"

using namespace std;
using namespace izsound;

WhiteNoise::WhiteNoise(bool active, const unsigned int &sampleRate)
  : DspUnit(sampleRate, 0, 1)
{
  m_active = active;
}

WhiteNoise::~WhiteNoise()
{
}

void WhiteNoise::performDsp()
{
  SlotData* output = m_outSlots[0];

  (*output)[0].clear();
  (*output)[1].clear();
  
  if(!m_active) 
  {
    // Be silent when not activated
    return;
  }

  // Calc Gaussian random value
  double R1 = (double)rand() / (double)RAND_MAX;
  double R2 = (double)rand() / (double)RAND_MAX;
  double X = (double)sqrt(-1.0f * log(R1)) * cos(1.0f * M_PI * R2);
  
  // We send it
  (*output)[0].push_back(X);
  (*output)[1].push_back(X);
}


