/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* mpgsfiledecoder.cpp
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
 
 
#include <mpgsfiledecoder.h>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace izsound;



MpgsFileDecoder::MpgsFileDecoder(const char* filename,
                               const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
throw(IzSoundException)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  m_endReached = false;
  m_playerStatus = PLAY;
  m_readBufferSize = bufferSize;
  m_readBuffer_left = new float[m_readBufferSize / 2];
  m_readBuffer_right = new float[m_readBufferSize / 2];
  frame_position = 0;

 
  
  // We open
  open(filename);
}

MpgsFileDecoder::MpgsFileDecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  m_endReached = false;
  m_playerStatus = STOP;
  m_readBufferSize = bufferSize;
  m_readBuffer_left = new float[m_readBufferSize];
  m_readBuffer_right = new float[m_readBufferSize];
  frame_position = 0;
}

MpgsFileDecoder::~MpgsFileDecoder()
{
  mpeg3_close( file );
  delete[] m_readBuffer_left;
  delete[] m_readBuffer_right;
  
}

void MpgsFileDecoder::performDsp()
{
  switch (m_playerStatus)
  {
    case PLAY  : playNextChunk(); break;
    case PAUSE : playNothing();   break;
    case STOP  : playNothing();   break;
  }
}

void MpgsFileDecoder::playNextChunk()
{
  // Check !
  if (file == NULL) return;
  
  
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
  
  if ( m_direction == FORWARD )
  {
  	// left channel
  	mpeg3_read_audio( file,  m_readBuffer_left, NULL, 0, (long)frames_in_buffer, 0);
	// right channel
  	mpeg3_reread_audio( file,  m_readBuffer_right, NULL, 1, (long)frames_in_buffer, 0);
	
	for (int i = 0; i < frames_in_buffer; i++)
	{
		(*output)[0].push_back( (double)m_readBuffer_left[i]);
		(*output)[1].push_back( (double)m_readBuffer_right[i]);
	}
	
	// position update
	frame_position += frames_in_buffer;
	
// #ifdef DEBUG
// 	cout << "Frame position: " << frame_position << endl;
// #endif
	
	// Check if end reached...
  	if ( frame_position >= getFrames() ) 
  	{
  		m_endReached = true;
		stop();
  	}
  }
  else 
  {
  	// going backward...
	
	// left channel
  	mpeg3_read_audio( file,  m_readBuffer_left, NULL, 0, (long)frames_in_buffer, 0);
	// right channel 
  	mpeg3_reread_audio( file,  m_readBuffer_right, NULL, 1, (long)frames_in_buffer, 0);
	for (int i = frames_in_buffer; i > 0; i--)
	{
		(*output)[0].push_back( (double)m_readBuffer_left[i]);
		(*output)[1].push_back( (double)m_readBuffer_right[i]);
	}
	
	// position update
	setFrame( frame_position - (frames_in_buffer * 2));
	frame_position -= frames_in_buffer;
	
	// Check if end reached...
  	if ( frame_position <= 0 ) 
  	{
  		m_endReached = true;
		stop();
  	}
  } 
}

void MpgsFileDecoder::playNothing()
{
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
}

void MpgsFileDecoder::play()
{
  m_playerStatus = PLAY;
}

void MpgsFileDecoder::pause()
{
  m_playerStatus = PAUSE;
}

void MpgsFileDecoder::stop()
{
  m_playerStatus = STOP;
  setFrame( 0 );
}

void MpgsFileDecoder::open(const char* filename) throw(IzSoundException)
{
  // Init
  m_endReached = false;
  frame_position = 0;
  m_direction = FORWARD;
  char file_n[1024];
  
  sprintf(file_n, "%s", filename);
  
  file = mpeg3_open( file_n );
  
  if ( file == NULL )
  {
  	cerr << "libmpeg3: error opening file. " << endl;
	return;
  }
  
  if ( int y = mpeg3_total_astreams( file ) > 1 )
  {
  	cerr << "libmpeg3: too much streams ( " << y << " )"<< endl;
	return;
  }
  
  channels = mpeg3_audio_channels( file, 0 );
  samplerate = mpeg3_sample_rate( file, 0 );
  frames = mpeg3_audio_samples( file, 0 );

  // set up frame number in our input buffer...
  frames_in_buffer = (int)( m_readBufferSize);
  
  mpeg3_set_mmx( file, 0 );
  mpeg3_set_cpus( file, 0 );
  
#ifdef DEBUG
	cout << "Channels: " << channels << endl;
	cout << "Samplerate: " << samplerate << endl;
	cout << "Streams: " << mpeg3_total_astreams( file ) << endl;
	cout << "Buffer frames: " << frames_in_buffer << endl;
#endif

}

int MpgsFileDecoder::getTotalTime()
{
  return (int)(frames / samplerate);
}

void MpgsFileDecoder::setCurrentTime(int pos)
{
  int to_pos = (pos * getFrames()) / getTotalTime() ;
  frame_position = to_pos;
  setFrame( to_pos );
}

int MpgsFileDecoder::getCurrentTime()
{
  return (int)(getFrame() / samplerate);
}

int MpgsFileDecoder::getFrames()
{
   return   (int)(frames);
}

int MpgsFileDecoder::getFrame()
{
   return   (int)(mpeg3_get_frame( file, 0 ));
}

void MpgsFileDecoder::setFrame(int frame)
{
   if ((frame < getFrames()) && (frame >= 0))
   {
	mpeg3_set_sample( file, (long)(frame), 0 );;
	frame_position = frame;
   } else
   	cerr << "Seek: parameter 'frame' out of bounds." << endl;
}

void MpgsFileDecoder::setReadDirection(int direction)
{
   if ( (direction == FORWARD) || (direction == BACKWARD) )
   {
   	m_direction = direction;
   }
   else
   	return;
}

