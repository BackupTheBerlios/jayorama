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
  thread_running = false;
  
  // init mutexes
  pthread_mutex_init( &service_mutex, NULL );
  pthread_mutex_init( &decoder_mutex, NULL );
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
  stopThread();
  // destroy mutexes
  pthread_mutex_destroy( &service_mutex );
  pthread_mutex_destroy( &decoder_mutex );
  delete[] m_readBuffer;
}


void DecoderBase::startThread()
{
  int error_code = pthread_create( &decoder_thread, NULL, DecoderBase::threadLoop, (void*)this );
  if ( error_code )
  {
  	cerr <<"ERROR: return code from pthread_create() is " <<  error_code << endl;
  	m_status = DECODER_THREAD_ERROR;
	return;
  }
  
  // Set the right value for thread statement register
  // safe lock...
  pthread_mutex_lock(&service_mutex);
  thread_running = true;
  // unlock...
  pthread_mutex_unlock(&service_mutex);
}
 
  
void DecoderBase::stopThread()
{
  // Set the right value for thread statement register
  // safe lock...
  pthread_mutex_lock(&service_mutex);
  thread_running = false;
  // unlock...
  pthread_mutex_unlock(&service_mutex);
  
  pthread_exit( NULL );
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
	 
	// lock decoder
	decoder->lock();
	// safe copy of our setup...
	OperationRegister do_operation = decoder->m_operation;
	// error prevention...
	if ( decoder->m_status != DECODER_OK )
	{
		do_operation = decoder->m_operation = STOP;
	}
	// unlock...
	decoder->unlock();
	/********************************************/
	
// #ifdef DEBUG
// 	cout << "Loop thread working..." << endl;
// #endif 
	
	switch (do_operation)
	{
	// If op = decode, we call run() function that make all the chain work,
	// calling performDsp() on the decoder object and, one by one
	// on all chain connected DspUnits
	case DECODE  : decoder->run();   break;
	case PAUSE : /*playNothing();*/   break;
	case STOP  : /*playNothing();*/   break;
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
  lock();

  // safe copy of our setup...
  int direction = m_direction;
  // save decoding position
  unsigned int f_position = m_frame_position;
  unsigned int frames = m_frames;
  int channels = m_channels;

  // unlock...
  unlock();
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
  decode( direction, f_position, max_frames_in_buffer, m_readBuffer );


  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
  
  for( unsigned int i = 0; i < (frames_in_buffer * channels)/*m_readBufferSize*/; i++)
  {
    // We add it
    // left
    (*output)[0].push_back(m_readBuffer[i++]);
    // right
    (*output)[1].push_back(m_readBuffer[i]);
  }
  
  
  /****
   * BUG ( FIXED ): always pushing frames_in_buffer and updating
   * with this value... We try to have a full blocksize but
   * decode function may be unable to retrieve it!
   ****/
  
  lock(); /*************/
  // position update
  if ( direction == FORWARD )
	m_frame_position += frames_in_buffer;
  else
	m_frame_position -= frames_in_buffer;
  unlock();/************/
	
	
  // reset frames_in_buffer counter
  frames_in_buffer = 0;
  
// #ifdef DEBUG
// 	cout << "performDsp ok..." << endl;
// #endif 
}


void DecoderBase::play()
{
  // set up the right operation and detach the decoding thread...
  // safe lock...
  lock();
  m_operation = DECODE;
  // unlock...
  unlock();
  
  if ( isThreadRunning() == false )
  	startThread();
}


void DecoderBase::stop()
{
  // safe lock...
  lock();
  m_operation = STOP;
  // unlock...
  unlock();
  
  if ( isThreadRunning() == true )
  	stopThread();
  
  /****
   * BUG ( FIXED ): if setFrame between lock operations
   * above there is no solution to exit from a blocking state!
   ****/
  setFrame( 0 );
  
#ifdef DEBUG
	cout << "Stop-op ok..." << endl;
#endif 
}


void DecoderBase::pause()
{
  // safe lock...
  lock();
  m_operation = PAUSE;
  // unlock...
  unlock();
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
  lock();
  
  if (( frame >= 0 ) && ( frame <= m_frames ))
  	m_frame_position = frame;
  else
  	cerr << "ERROR: frame parameter out of bounds" << endl;

  // unlock...
  unlock();

}


unsigned int DecoderBase::getFrame()
{
  // safe lock...
  lock();
  unsigned int tmp = m_frame_position;
  // unlock...
  unlock();
  return tmp;
}


void DecoderBase::setCurrentTime(int pos)
{
  unsigned int frs = 0, time = 0;
  frs = getFrames();
  time = getTotalTime();
  if (( frs != 0) && ( time != 0 ))
  {
  	unsigned int to_pos = (pos * getFrames()) / getTotalTime() ;
  	setFrame( to_pos );
  }
  else
  	return;
}


int DecoderBase::getCurrentTime()
{
  int time = 0;
  // safe lock...
  lock();
  if (( m_frame_position != 0 ) && ( m_samplerate != 0 ))
  {
  	time = (int)(m_frame_position / m_samplerate);
  }
  // unlock...
  unlock();
  
  return time;
}


void DecoderBase::setReadDirection(int direction)
{
  if ( (direction == FORWARD) || (direction == BACKWARD) )
  {
  	// safe lock...
  	lock();
   	m_direction = direction;
	// unlock...
  	unlock();
  }
  else
  	cerr << "ERROR: wrong direction parameter" << endl;
}


bool DecoderBase::isThreadRunning()
{
  bool is_running = false;
  // safe lock...
  pthread_mutex_lock(&service_mutex);
  is_running = thread_running;
  // unlock...
  pthread_mutex_unlock(&service_mutex);
  return is_running;
}


void DecoderBase::lock()
{
  // safe lock...
  pthread_mutex_lock(&decoder_mutex);
}


void DecoderBase::unlock()
{
  // unlock...
  pthread_mutex_unlock(&decoder_mutex);
}
