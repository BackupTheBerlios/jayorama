/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* rtaudiooutput.cpp
 *
 *  Copyright (C) 2004 Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


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

#include <rtaudiooutput.h>

using namespace std;
using namespace izsound;

RtAudioOutput::RtAudioOutput( /*DspUnit *_callbackUnit ,*/ 
                              const unsigned int &sampleRate ,
                              const unsigned int &bufferTime ,
			      const unsigned int &nBuffers ,
			      int device_num )
throw(IzSoundException)
  : DspUnit(sampleRate, 1, 0)
{
  // We allocate the buffer
  m_pcmBufferNSamples = (m_sampleRate * bufferTime) / 1000;
  m_pcmBufferSize = m_pcmBufferNSamples * 2;
//   m_pcmBuffer = new double[m_pcmBufferSize];
//   m_pcmBufferPosition = 0;
  m_nBuffers = nBuffers;
  
//   if ( _callbackUnit == NULL )
//   {
//   	out_mode = BLOCKING ;
//   }
//   else
//   {
//   	callbackUnit = _callbackUnit;
//   	out_mode = CALLBACK ;
//   }

  
  // Default RtAudio constructor
  try {
    audio = new RtAudio();
  }
  catch (RtError &error) {
    // Handle the exception here
    error.printMessage();
  }
  
  
  // stream opening... (always stereo ==> channels = 2!)
  try {
    audio->openStream(device_num, 2, 0, 0, RTAUDIO_FLOAT64,
                      sampleRate, &m_pcmBufferSize, m_nBuffers);
  }
  catch (RtError &error) {
    error.printMessage();
    goto fail;
  }
  
  // set audio callback and passing this object...
  audio->setStreamCallback(&rtCallback, (void *)this);

  
  // Ooops
  fail:
  	throw IzSoundException("Could not set up RtAudio device.");
}

RtAudioOutput::~RtAudioOutput()
{
  delete[] m_pcmBuffer;
  delete audio;
}

void RtAudioOutput::performDsp()
{
  // Init
  SlotData* data = m_inSlots[0];
  unsigned int nsamples =
    /* If they aren't equal, data will be lost ! But in this case there is a
       faulty DSP unit above this one ... */
    min((*data)[0].size(), (*data)[1].size());

  // Let's go
  for (unsigned int i = 0; i < nsamples; ++i)
  {
    writeToBuffer((*data)[0][i], (*data)[1][i]);
  }
}

void RtAudioOutput::writeToBuffer(double &left, double &right)
{
  // Simple-brutal-stupid Clipping
  if (left >= 1.0)
  {
    left = CLIPPING_VALUE;
  }
  else if (left <= -1.0)
  {
    left = -CLIPPING_VALUE;
  }
  if (right >= CLIPPING_VALUE)
  {
    right = CLIPPING_VALUE;
  }
  else if (right <= -1.0)
  {
    right = -CLIPPING_VALUE;
  }

  // Data conversion
  short ileft = (short)floor(left * 32768.0);
  short iright = (short)floor(right * 32768.0);
#ifdef WORDS_BIGENDIAN
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) ((ileft >> 8) & 0xff);
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) (ileft & 0xff);
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) ((iright >> 8) & 0xff);
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) (iright & 0xff);
#else
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) (ileft & 0xff);
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) ((ileft >> 8) & 0xff);
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) (iright & 0xff);
  m_pcmBuffer[m_pcmBufferPosition++] = (unsigned char) ((iright >> 8) & 0xff);
#endif

  // We write to the device if needed
  if (m_pcmBufferPosition == m_pcmBufferSize)
  {
    writeToDevice();
  }
}

// void RtAudioOutput::writeToDevice()
// {
//   write(m_device, m_pcmBuffer, m_pcmBufferPosition);
//   m_pcmBufferPosition = 0;
// }

// void RtAudioOutput::flush()
// {
//   writeToDevice();
//   fsync(m_device);
// }



//--------------------------------------------------
int rtCallback(char *outputBuffer, int framesPerBuffer, void *outputObject)
{
  
}