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

 /*
  * Contributors:
  * - Karl Pitrich (balancing)
  */

#include <volume.h>

using namespace std;
using namespace izsound;

/*
 * Unary function that multiplies by a factor.
 */
struct Multiply_OF
{
  double factor;

  Multiply_OF(const double &f) { factor = f; }

  double operator() (const double &val)
  {
    return val * factor;
  }
};

Volume::Volume(const double &factor, const double &balance,
               const unsigned int &sampleRate)
  : DspUnit(sampleRate, 1, 1)
{
  m_factor = factor;
  m_balance = balance;
}

Volume::~Volume()
{

}

void Volume::performDsp()
{
  // Init
  SlotData* input  = m_inSlots[0];
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
  back_insert_iterator<deque<double> > biLeft((*output)[0]);
  back_insert_iterator<deque<double> > biRight((*output)[1]);

  // Balancing
  double vLeft  = m_factor;
  double vRight = m_factor;
  if(m_balance != 0.0)
  {
    if(m_balance < -1.0)
    {
      m_balance = -1.0;
    }
    if(m_balance > 1.0)
    {
      m_balance = 1.0;
    }
    if(m_balance < 0.0)
    {
      vRight = m_factor * (1.0 - (m_balance * -1.0));
    }
    else
    {
      vLeft = m_factor * (1.0 - m_balance);
    }
  }

  // Sound processing
  transform((*input)[0].begin(), (*input)[0].end(),
            biLeft, Multiply_OF(vLeft));
  transform((*input)[1].begin(), (*input)[1].end(),
            biRight, Multiply_OF(vRight));
}
