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

#include <demultiplexer.h>

using namespace std;
using namespace izsound;

DeMultiplexer::DeMultiplexer(const unsigned int &outCount,
                             const unsigned int &sampleRate)
  : DspUnit(sampleRate, 1, outCount)
{

}

DeMultiplexer::~DeMultiplexer()
{

}

void DeMultiplexer::performDsp()
{
  // Inits
  SlotData* input = m_inSlots[0];
  SlotData* output;

  // Let's go
  for (unsigned int i = 0; i < m_maxOutSlots; ++i)
  {
    // We get the output slot
    if (m_outDspUnits[i] == 0) continue;
    output = m_outSlots[i];

    // We copy
    (*output)[0].clear();
    (*output)[1].clear();
    back_insert_iterator<deque<double> > bi_left((*output)[0]);
    back_insert_iterator<deque<double> > bi_right((*output)[1]);
    copy((*input)[0].begin(), (*input)[0].end(), bi_left);
    copy((*input)[1].begin(), (*input)[1].end(), bi_right);
  }
}
