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

#include <ossoutput.h>

using namespace std;
using namespace izsound;

OssOutput::OssOutput(const unsigned int &sampleRate,
                     const unsigned int &bufferTime, const char* device)
throw(IzSoundException)
  : DspUnit(sampleRate, 1, 0)
{
  // We allocate the buffer
  m_pcmBufferNSamples = (m_sampleRate * bufferTime) / 1000;
  m_pcmBufferSize = m_pcmBufferNSamples * 4;
  m_pcmBuffer = new char[m_pcmBufferSize];
  m_pcmBufferPosition = 0;

  // We open and set-up the device
  unsigned int param;
  if ((m_device = open(device, O_WRONLY)) < 0) goto fail;
  param = AFMT_S16_LE;
  if (ioctl(m_device, SNDCTL_DSP_SETFMT, &param) < 0) goto fail;
  if (param != AFMT_S16_LE) goto fail;
  param = 2;
  if (ioctl(m_device, SNDCTL_DSP_CHANNELS, &param) < 0) goto fail;
  if (param != 2) goto fail;
  param = m_sampleRate;
  if (ioctl(m_device, SNDCTL_DSP_SPEED, &param) < 0) goto fail;
  if (param != m_sampleRate) goto fail;
  return;

  // Ooops
fail:
  throw IzSoundException("Could not open the OSS device.");
}

OssOutput::~OssOutput()
{
  delete[] m_pcmBuffer;
  close(m_device);
}

void OssOutput::performDsp()
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

void OssOutput::writeToBuffer(double &left, double &right)
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

void OssOutput::writeToDevice()
{
  write(m_device, m_pcmBuffer, m_pcmBufferPosition);
  m_pcmBufferPosition = 0;
}

void OssOutput::flush()
{
  writeToDevice();
  fsync(m_device);
}
