/* decoderbase.cpp
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


#include <decoderbase.h>
#include <iostream>


using namespace std;
using namespace izsound;

DecoderBase::DecoderBase(const unsigned int &bufferSize,
                         const unsigned int &sampleRate)
:DspUnit(sampleRate, 0, 1)
{
  // Init
  m_endReached = false;
  m_status = DECODER_OK;
  m_readBufferSize = bufferSize;
  m_readBuffer = new double[m_readBufferSize];
  m_frame_position = 0;
  m_direction = FORWARD;
  // Init file info to zero-values
  m_channels = 0;
  m_samplerate = 0;
  m_frames = 0;
  max_frames_in_buffer = 0;
  frames_in_buffer = 0;

  // decoder is stopped
  m_operation = STOP;
  
  // start thread
  startup();
}


DecoderBase::~DecoderBase()
{
//   doOperation( SHUTDOWN );
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  m_operation = SHUTDOWN;
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
  //shutdown thread...
  shutdown();
  delete[] m_readBuffer;
}


void DecoderBase::startup()
{
  int error_code = pthread_create( &decoder_thread, NULL, DecoderBase::threadLoop, (void*)this );
  if ( error_code )
  {
  	cerr <<"ERROR: return code from pthread_create() is " <<  error_code << endl;
  	m_status = DECODER_THREAD_ERROR;
	return;
  }
  
  pthread_mutex_init( &decoder_mutex, NULL );
}
 
  
void DecoderBase::shutdown()
{
  pthread_exit( NULL );
  pthread_mutex_destroy( &decoder_mutex );
}
  
  
void* DecoderBase::threadLoop(void *_decoder)
{
  bool exit_loop = false;
  DecoderBase* decoder = (DecoderBase*) _decoder;
  
  while(1)
  {
	/********************************************
	* The following codelines save the actual decoder
	* situation in temp fields, avoiding to have mutex
	* errors.
	*/ 
	 
	// safe lock...
	pthread_mutex_lock(&decoder->decoder_mutex);
	
	// safe copy of our setup...
	OperationRegister do_operation = decoder->m_operation;
	
	// error prevention...
	if ( decoder->m_status != DECODER_OK )
	{
		do_operation = decoder->m_operation = STOP;
	}
	
	// unlock...
	pthread_mutex_unlock(&decoder->decoder_mutex);
	/********************************************/
	
// #ifdef DEBUG
// 	cout << "Loop thread working..." << endl;
// #endif 
	
	switch (do_operation)
	{
	case PLAY  : decoder->run();   break;
	case PAUSE : /*playNothing();*/   break;
	case STOP  : /*playNothing();*/   break;
// 	case SEEK  : seek();   break;
	case SHUTDOWN : exit_loop = true; break;
	}
	
	if ( exit_loop == true )
		break;
  };
}


void DecoderBase::performDsp()
{
  /********************************************
  * The following codelines save the actual decoder
  * situation in temp fields, avoiding to have mutex
  * errors. (fields not saved SHOULD have no issues...)
  */ 
	
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);

  // safe copy of our setup...
  int direction = m_direction;
  // save decoding position
  unsigned int f_position = m_frame_position;
  unsigned int frames = m_frames;

  // position update
  if ( direction == FORWARD )
	m_frame_position += max_frames_in_buffer;
  else
	m_frame_position -= max_frames_in_buffer;

  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
  /********************************************/
	
  
  if (( f_position > frames ) && ( direction == FORWARD ))
  {
	m_endReached = true;
	stop();
  }
  if (( f_position == 0 ) && ( direction == BACKWARD ))
  {
	m_endReached = true;
	stop();
  }
  
  // and now decoding...
  decode( direction, f_position, 
	  max_frames_in_buffer,
	  m_readBuffer );


  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
  
  for( unsigned int i = 0; i < /*max_frames_in_buffer*/m_readBufferSize; i++)
  {
    // We add it
    // left
    (*output)[0].push_back(m_readBuffer[i++]);
    // right
    (*output)[1].push_back(m_readBuffer[i]);
  }
  
// #ifdef DEBUG
// 	cout << "performDsp ok..." << endl;
// #endif 
}


void DecoderBase::play()
{
//   doOperation( PLAY );
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  m_operation = PLAY;
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
}


void DecoderBase::stop()
{
//   doOperation( STOP );
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  m_operation = STOP;
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
  
  /*
   * BUG solved: if setFrame between pthread_mutex operations
   * above there is no solution to exit from a blocking state!
   */
  setFrame( 0 );
  
#ifdef DEBUG
	cout << "Stop-op ok..." << endl;
#endif 
}


void DecoderBase::pause()
{
//   doOperation( PAUSE );
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  m_operation = PAUSE;
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
}


unsigned int DecoderBase::getTotalTime()
{
  if (( m_frames != 0 ) && ( m_samplerate != 0 ))
  	return (m_frames / m_samplerate);
  else
  	return 0;
}


unsigned int DecoderBase::getFrames()
{
  if ( m_frames != 0 )
  	return m_frames;
  else
  	return 0;
}


void DecoderBase::setFrame(unsigned int frame)
{
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  if (( frame >= 0 ) && ( frame <= m_frames ))
  	m_frame_position = frame;
  else
  	cerr << "ERROR: frame parameter out of bounds" << endl;
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
}


unsigned int DecoderBase::getFrame()
{
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  unsigned int tmp = m_frame_position;
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
  return tmp;
}


void DecoderBase::setCurrentTime(int pos)
{
  unsigned int to_pos = (pos * getFrames()) / getTotalTime() ;
  setFrame( to_pos );
}


int DecoderBase::getCurrentTime()
{
  int time = 0;
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
  if (( m_frame_position != 0 ) && ( m_samplerate != 0 ))
  {
  	time = (int)(m_frame_position / m_samplerate);
  }
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
  
  return time;
}


void DecoderBase::setReadDirection(int direction)
{
  if ( (direction == FORWARD) || (direction == BACKWARD) )
  {
  	// safe lock...
  	pthread_mutex_lock(&decoder_mutex);
   	m_direction = direction;
	// unlock...
  	pthread_mutex_unlock(&decoder_mutex);
  }
  else
  	cerr << "ERROR: wrong direction parameter" << endl;
}


// void DecoderBase::doOperation( opcode )
// {
//   // safe lock...
//   pthread_mutex_lock(&decoder_mutex);
//   m_operation = opcode;
//   // unlock...
//   pthread_mutex_unlock(&decoder_mutex);
// }
