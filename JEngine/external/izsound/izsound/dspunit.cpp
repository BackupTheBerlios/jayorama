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

#include <dspunit.h>

using namespace std;
using namespace izsound;

DspUnit::DspUnit(const unsigned int &sampleRate,
                 const unsigned int &maxIn, const unsigned int &maxOut)
{
  m_maxInSlots = maxIn;
  m_maxOutSlots = maxOut;
  m_sampleRate = sampleRate;

  m_receptionCounter = 0;
  m_receptionTrigger = 0;

  m_inSlots.resize(m_maxInSlots);
  m_outSlots.resize(m_maxOutSlots);
  m_outDspUnits.resize(m_maxOutSlots);
  m_bindedSlots.resize(m_maxInSlots);
  fill(m_bindedSlots.begin(), m_bindedSlots.end(), false);

  unsigned int i;
  for (i = 0; i < m_maxInSlots; ++i)
  {
    m_inSlots[i] = new SlotData();
    m_inSlots[i]->resize(2);
  }
  for (i = 0; i < m_maxOutSlots; ++i)
  {
    m_outSlots[i] = 0;
    m_outDspUnits[i] = 0;
  }
}

DspUnit::~DspUnit()
{
  for (unsigned int i = 0; i < m_maxInSlots; ++i)
  {
    if ((m_inSlots[i] != 0) && !(m_bindedSlots[i]))
    {
      delete m_inSlots[i];
      m_inSlots[i] = 0;
    }
  }
}

void DspUnit::connect(DspUnit* dspUnit, const unsigned int &mySlot,
                      const unsigned int &itsSlot)
{
  // Sanity checks
  if (mySlot > m_maxOutSlots || itsSlot > dspUnit->m_maxInSlots) return;
  if (m_outSlots[mySlot] != 0) disconnect(mySlot);

  // We connect
  m_outSlots[mySlot] = dspUnit->m_inSlots[itsSlot];
  m_outDspUnits[mySlot] = dspUnit;
  dspUnit->m_receptionTrigger++;
}

void DspUnit::disconnect(const unsigned int &mySlot)
{
  // Sanity checks
  if (mySlot > m_maxOutSlots || m_outSlots[mySlot] == 0) return;

  // We disconnect
  m_outSlots[mySlot] = 0;
  m_outDspUnits[mySlot]->m_receptionTrigger--;
  m_outDspUnits[mySlot] = 0;
}

void DspUnit::run()
{
  // We perform the job if we have collected all the data we need
  if (m_receptionCounter != m_receptionTrigger) return;
  performDsp();

  // We asks the others to do so
  int size = m_outDspUnits.size();
  for (int i = 0; i < size; ++i)
  {
    if (m_outDspUnits[i] != 0)
    {
      m_outDspUnits[i]->m_receptionCounter++;
      m_outDspUnits[i]->run();
    }
  }

  // Ready for next time
  m_receptionCounter = 0;
}

void DspUnit::bindInput(DspUnit* dsp, const unsigned int &mySlot,
                        const unsigned int itsSlot)
{
  if (m_inSlots[mySlot] != 0) delete m_inSlots[mySlot];
  m_inSlots[mySlot] = dsp->m_inSlots[itsSlot];
  m_bindedSlots[mySlot] = true;
}

void DspUnit::bindOutput(SlotData* data, const unsigned int &slot)
{
  m_outSlots[slot] = data;
}
