/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* sndfiledecoder.cpp
 *
 *  Copyright (C) 2004 Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 *  Some parts: 
 *  Copyright (C) 1998-2003 Andy Lo A Foe <andy_AT_alsaplayer.org>
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
 
 
#include <sndfiledecoder.h>
#include <iostream>

using namespace std;
using namespace izsound;



SndFileDecoder::SndFileDecoder(const char* filename,
                               const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
throw(IzSoundException)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  m_endReached = false;
  m_playerStatus = PLAY;
  m_readBufferSize = bufferSize;
  m_readBuffer = new double[m_readBufferSize];
  frame_position = 0;
  m_seeking = false;
  // format must be 0 if file is opened in read mode... (from sndfile docs)
  sfinfo.format = 0;

  
 
  
  // We open
  open(filename);
}

SndFileDecoder::SndFileDecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  m_endReached = false;
  m_playerStatus = STOP;
  m_readBufferSize = bufferSize;
  m_readBuffer = new double[m_readBufferSize];
  frame_position = 0;
  m_seeking = false;
  // format must be 0 if file is opened in read mode... (from sndfile docs)
  sfinfo.format = 0;
  
}

SndFileDecoder::~SndFileDecoder()
{
  int close_error = sf_close( file );
  if ( close_error )
  {
  	cerr << "Error while closing file: " << sf_error_number(close_error) << endl;
  }
  delete[] m_readBuffer;
  
}

void SndFileDecoder::performDsp()
{
  switch (m_playerStatus)
  {
    case PLAY  : playNextChunk(); break;
    case PAUSE : playNothing();   break;
    case STOP  : playNothing();   break;
  }
}

void SndFileDecoder::playNextChunk()
{
  // Check !
  if (file == NULL) return;
  

  
  if ( m_direction == FORWARD )
  {
  	sf_count_t frames_decoded = 0;
	
  	// To decode we use internal double conversion of libsndfile...
  	frames_decoded =   sf_readf_double(file, m_readBuffer, (sf_count_t)frames_in_buffer);
	frame_position += frames_in_buffer;
	
	// Check if end reached...
  	if ( frames_decoded < (sf_count_t)frames_in_buffer ) 
  	{
  		m_endReached = true;
		stop();
  	}
  }
  else 
  {
  	// going backward...
	sf_count_t frs = 0;
	int offset = 0;
	for ( int j = 0; j < frames_in_buffer; j++ )
	{
		frs = sf_readf_double(file, m_readBuffer + offset, 1);
		if ( frs == 1 )
		{
			frame_position-= 1;
			if ( frame_position > 0 )
			{
				setFrame( frame_position );
				offset += sfinfo.channels;
			}
			else
			{
				m_endReached = true;
				stop();
			}
		}
		else
		{
			m_endReached = true;
			stop();
		}
	}
	
  } 
  
  
  	
  
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();

  
  for( int i = 0; i < (int)m_readBufferSize; i++)
  {
    // We add it
    // left
    (*output)[0].push_back(m_readBuffer[i++]);
    // right
    (*output)[1].push_back(m_readBuffer[i]);
  }
}

void SndFileDecoder::playNothing()
{
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
}

void SndFileDecoder::play()
{
  m_playerStatus = PLAY;
}

void SndFileDecoder::pause()
{
  m_playerStatus = PAUSE;
}

void SndFileDecoder::stop()
{
  m_playerStatus = STOP;
  setFrame( 0 );
}

void SndFileDecoder::open(const char* filename) throw(IzSoundException)
{
  // Init
  m_endReached = false;
  frame_position = 0;
  m_direction = FORWARD;
  
  file = sf_open( filename, SFM_READ, &sfinfo);
  
  if (file == NULL)
  {
  	cerr << "Libsndfile error: " <<  sf_strerror(file) << endl;
	return;
  }
  
  // set up frame number in our input buffer...
  frames_in_buffer = (int)( m_readBufferSize / sfinfo.channels );

}

int SndFileDecoder::getTotalTime()
{
  return ((int)(sfinfo.frames / sfinfo.samplerate) );
}

void SndFileDecoder::setCurrentTime(int pos)
{
  int to_pos = (pos * getFrames()) / getTotalTime() ;
  frame_position = to_pos;
  setFrame( to_pos );
}

int SndFileDecoder::getCurrentTime()
{
  return (frame_position / (int)(sfinfo.samplerate) );
}

int SndFileDecoder::getFrames()
{
   return   (int)sfinfo.frames;
}

int SndFileDecoder::getFrame()
{
   return   frame_position;
}

void SndFileDecoder::setFrame(int frame)
{
   if ((frame < getFrames()) && (frame >= 0))
   {
   	frame_position = frame;
	m_seeking = true;
   	sf_seek(file, (sf_count_t) frame, SEEK_SET);
   } else
   	cerr << "Seek: parameter 'frame' out of bounds." << endl;
}

void SndFileDecoder::setReadDirection(int direction)
{
   if ( (direction == FORWARD) || (direction == BACKWARD) )
   {
   	m_direction = direction;
   }
   else
   	return;
}

