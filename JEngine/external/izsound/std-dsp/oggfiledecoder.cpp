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

#include <oggfiledecoder.h>

using namespace std;
using namespace izsound;

OggFileDecoder::OggFileDecoder(const char* filename,
                               const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
throw(IzSoundException)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  m_oggFile = 0;
  m_endReached = false;
  m_playerStatus = PLAY;
  m_readBufferSize = bufferSize;
  m_readBuffer = new char[m_readBufferSize];

  // We open
  open(filename);
}

OggFileDecoder::OggFileDecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  m_endReached = false;
  m_playerStatus = STOP;
  m_oggFile = 0;
  m_readBufferSize = bufferSize;
  m_readBuffer = new char[m_readBufferSize];
}

OggFileDecoder::~OggFileDecoder()
{
  delete[] m_readBuffer;
  clearOggDescriptor();
}

void OggFileDecoder::clearOggDescriptor()
{
  if (m_oggFile != 0)
  {
    ov_clear(m_oggFile);
    delete m_oggFile;
  }
}

void OggFileDecoder::performDsp()
{
  switch (m_playerStatus)
  {
    case PLAY  : playNextChunk(); break;
    case PAUSE : playNothing();   break;
    case STOP  : playNothing();   break;
  }
}

void OggFileDecoder::playNextChunk()
{
  // Check !
  if (m_oggFile == 0) return;

  // We grab a new frame
  int bitstream = 0;
  long nread = ov_read(m_oggFile, m_readBuffer, (int)m_readBufferSize,
                       0, 2, 1, &bitstream);
  m_endReached = (nread == 0);
  if (m_endReached)
  {
    stop();
  }
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();

  // We put that on the stream
  int i = 0;
  short ileft, iright;
  double left, right;
  while (i < nread)
  {
    // Data unpacking
    ileft  = ((short)m_readBuffer[i++]) & 0xff;
    ileft  = ileft | (short)(((short)(m_readBuffer[i++])) << 8);
    iright = ((short)m_readBuffer[i++]) & 0xff;
    iright = iright | (short)(((short)(m_readBuffer[i++])) << 8);

    // Bijection to [-1.0 ; 1.0]
    left   = ((double)ileft) / 32768.0;
    right  = ((double)iright) / 32768.0;

    // We add it
    (*output)[0].push_back(left);
    (*output)[1].push_back(right);
  }
}

void OggFileDecoder::playNothing()
{
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
}

void OggFileDecoder::play()
{
  m_playerStatus = PLAY;
}

void OggFileDecoder::pause()
{
  m_playerStatus = PAUSE;
}

void OggFileDecoder::stop()
{
  m_playerStatus = STOP;
  ov_time_seek(m_oggFile, 0.0);
}

void OggFileDecoder::open(const char* filename) throw(IzSoundException)
{
  // Init
  clearOggDescriptor();
  m_endReached = false;

  // We open the file
  FILE* handle = fopen(filename, "r");
  if (handle == 0)
  {
    m_oggFile = 0;
    throw IzSoundException("Could not open the file.");
  }
  m_oggFile = new OggVorbis_File();
  if (ov_open(handle, m_oggFile, 0, 0) != 0)
  {
    m_oggFile = 0;
    throw IzSoundException("Ogg/Vorbis: could not open the file.");
  }
}

double OggFileDecoder::getTotalTime()
{
  return (m_oggFile == 0) ? 0.0 : ov_time_total(m_oggFile, 0);
}

void OggFileDecoder::seek(const double &pos)
{
  if (m_oggFile != 0) ov_time_seek(m_oggFile, pos);
}

double OggFileDecoder::getCurrentTime()
{
  return (m_oggFile == 0) ? 0.0 : ov_time_tell(m_oggFile);
}
