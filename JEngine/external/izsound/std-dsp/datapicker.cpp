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

#include <datapicker.h>

using namespace std;
using namespace izsound;

DataPicker::DataPicker(const unsigned int &dataPickingInterval,
                       const unsigned int &sampleRate)
  : DspUnit(sampleRate, 1, 1)
{
  setDataPickingInterval(dataPickingInterval);
}

DataPicker::~DataPicker()
{

}

void DataPicker::performDsp()
{
  // Init
  static unsigned int counter = 0;
  double left  = 0.0;
  double right = 0.0;
  SlotData* input  = m_inSlots[0];
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();

  // Let's go
  for (unsigned int i = 0; i < (*input)[0].size(); ++i)
  {
    left  = (*input)[0][i];
    right = (*input)[1][i];
    if (counter++ % m_dataPickingInterval == 0)
    {
      m_leftQueue.push(left);
      m_rightQueue.push(right);
      counter = 0;
    }
    (*output)[0].push_back(left);
    (*output)[1].push_back(right);
  }
}

void DataPicker::reset()
{
  // Why aren't there clear() methods for a queue ???
  while (!m_leftQueue.empty())
  {
    m_leftQueue.pop();
  }
  while (!m_leftQueue.empty())
  {
    m_rightQueue.pop();
  }
}
