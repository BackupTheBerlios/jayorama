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

#include <libaooutput.h>

using namespace std;
using namespace izsound;

int LibaoOutput::m_instancesCounter = 0;

LibaoOutput::LibaoOutput(const char* driver, ao_option* options,
                         const unsigned int &sampleRate)
throw(IzSoundException)
 : DspUnit(sampleRate, 1, 0)
{
  // Inits
  m_device = 0;
  m_pcmBuffer = new char[BUFFER_SIZE];
  m_pcmBufferPosition = 0;
  if (m_instancesCounter++ == 0) ao_initialize();

  // LibAO part
  if (driver != 0)
  {
    if ((m_driverId = ao_driver_id(driver)) < 0)
    {
      throw IzSoundException("There is no default driver.");
    }
  }
  else
  {
    m_driverId = ao_default_driver_id();
  }
  ao_sample_format format =  { 16, (int)sampleRate, 2, AO_FMT_LITTLE };
  if ((m_device = ao_open_live(m_driverId, &format, options)) == 0)
  {
    throw IzSoundException("Device opening failure.");
  }
}

LibaoOutput::LibaoOutput(const char* driver, ao_option* options,
                         const char* filename,
                         const unsigned int &sampleRate)
throw(IzSoundException)
  : DspUnit(sampleRate, 1, 0)
{
  // Inits
  m_device = 0;
  m_pcmBuffer = new char[BUFFER_SIZE];
  m_pcmBufferPosition = 0;
  if (m_instancesCounter++ == 0) ao_initialize();

  // LibAO part
  if ((m_driverId = ao_driver_id(driver)) < 0)
  {
    throw IzSoundException("Bad driver ID.");
  }
  ao_sample_format format =  { 16, (int)sampleRate, 2, AO_FMT_LITTLE };
  if ((m_device = ao_open_file(m_driverId, filename, 1,
                               &format, options)) == 0)
  {
    throw IzSoundException("Device opening failure.");
  }
}

LibaoOutput::~LibaoOutput()
{
  if (m_device != 0) ao_close(m_device);
  if (--m_instancesCounter == 0) ao_shutdown();
  delete[] m_pcmBuffer;
}

void LibaoOutput::performDsp()
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

void LibaoOutput::writeToBuffer(double &left, double &right)
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
  if (right >= 1.0)
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
  if (m_pcmBufferPosition == BUFFER_SIZE)
  {
    writeToDevice();
  }
}

void LibaoOutput::writeToDevice()
{
  ao_play(m_device, m_pcmBuffer, m_pcmBufferPosition);
  m_pcmBufferPosition = 0;
}

void LibaoOutput::flush()
{
  writeToDevice();
}
