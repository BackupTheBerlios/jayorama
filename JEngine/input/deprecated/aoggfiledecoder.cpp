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

#include <aoggfiledecoder.h>
#include <iostream>

using namespace std;
using namespace izsound;

AOggFileDecoder::AOggFileDecoder(const char* filename,
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

AOggFileDecoder::AOggFileDecoder(const unsigned int &bufferSize,
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

AOggFileDecoder::~AOggFileDecoder()
{
  delete[] m_readBuffer;
  clearOggDescriptor();
}

void AOggFileDecoder::clearOggDescriptor()
{
  if (m_oggFile != 0)
  {
    ov_clear(m_oggFile);
    delete m_oggFile;
  }
}

void AOggFileDecoder::performDsp()
{
  switch (m_playerStatus)
  {
    case PLAY  : playNextChunk(); break;
    case PAUSE : playNothing();   break;
    case STOP  : playNothing();   break;
  }
}

void AOggFileDecoder::playNextChunk()
{
  // Check !
  if (m_oggFile == 0) return;

  // We grab a new frame
  int bitstream = 0;
  long nread = ov_read(m_oggFile, m_readBuffer, (int)m_readBufferSize,
                       0, 2, 1, &bitstream);
        
  if ( m_direction == BACKWARD )
  {
  	char* temp_buffer = new char[m_readBufferSize];
	for ( unsigned int i = 0; i < m_readBufferSize; i++)
	{
		unsigned int j = m_readBufferSize - i;
		unsigned int u = 0;
		while ( u < 4 )
		{
			temp_buffer[j + u] = m_readBuffer[i + u];
			u++;
		}
	}
	for ( unsigned int k = 0; k < m_readBufferSize; k++)
		m_readBuffer[k] = temp_buffer[k];
	
	delete[] temp_buffer;
	
	int frame = getFrame();
	setFrame( frame - (frames_in_buffer * 2));
  }
  
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

void AOggFileDecoder::playNothing()
{
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
}

void AOggFileDecoder::play()
{
  m_playerStatus = PLAY;
}

void AOggFileDecoder::pause()
{
  m_playerStatus = PAUSE;
}

void AOggFileDecoder::stop()
{
  m_playerStatus = STOP;
  ov_time_seek(m_oggFile, 0.0);
}

void AOggFileDecoder::open(const char* filename) throw(IzSoundException)
{
  // Init
  clearOggDescriptor();
  m_endReached = false;
  m_direction = FORWARD;

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
  
  // fill info structure
  m_oggInfo = ov_info(m_oggFile, -1);
  
  frames_in_buffer = (int)(m_readBufferSize / (m_oggInfo->channels * 2));
}

int AOggFileDecoder::getTotalTime()
{
  if (m_oggFile != 0)
  {
	return (int)(ov_time_total(m_oggFile, 0));
  }
  else
  	return 0;
}

void AOggFileDecoder::setCurrentTime(int pos)
{
  if (m_oggFile != 0) ov_time_seek(m_oggFile, (int)(pos));
}

int AOggFileDecoder::getCurrentTime()
{
  if (m_oggFile != 0)
  {
	return (int)(ov_time_tell(m_oggFile));
  }
  else
  	return 0;
}

void AOggFileDecoder::setReadDirection(int direction)
{
   if ( (direction == FORWARD) || (direction == BACKWARD) )
   {
   	m_direction = direction;
   }
   else
   	return;
}

void AOggFileDecoder::setFrame(int frame)
{
   if ((frame < getFrames()) && (frame >= 0))
   {
   	frame_position = frame;
   	ov_pcm_seek(m_oggFile,(ogg_int64_t)(frame * m_oggInfo->channels));
   } else
   	cerr << "Seek: parameter 'frame' out of bounds." << endl;
}

int AOggFileDecoder::getFrames()
{
   return   (int)(ov_pcm_total(m_oggFile, -1) / m_oggInfo->channels);
}

int AOggFileDecoder::getFrame()
{
   return   (int)(ov_pcm_tell(m_oggFile));
}

