/* outputobjectbase.h
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


#ifndef _OUTPUTOBJBASE_H
#define _OUTPUTOBJBASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <common_def.h>
#include <dspunit.h>
#include <string>



using namespace std;

namespace izsound
{

/**
 * 
 *
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 

  

class OutputObjectBase : public DspUnit
{
  
  /*****************************************************************
   * ---- >> Functions TO BE reimplemented... << ----
   */
   
private:

  /**
   * Write to the device a buffer of data.
   *
   * @param buffer_size Size of buffer in frames.
   * @param buffer The buffer pointer.
   */
  virtual void write( unsigned int buffer_size, void* buffer ) = 0;
  
  /*
   * ---- >> End of Functions TO BE reimplemented... << ----
   ******************************************************************/
    
public:
 
  /**
   * The constructor. 
   */
  OutputObjectBase( int bufferSize = 4096, int sampleRate = 44100 );
  
  /**
   * Destructor.
   */
  virtual ~OutputObjectBase();
  
  /**
   * The pcm worker function.
   */
  virtual void performDsp();

public:

  /** 
   * Channels. 
   */
  int getChannels() { return m_outChannels; }
  
  /** 
   * Number of buffers for the buffer partition. Not all APIs can handle this param. 
   */
  int getNumOfBuffers() { return m_outNumOfBuffers; }
  
  /**
   * Info about the status of output API.
   *
   * @return The status of the output API.
   */
  int status() { return m_status; }
  
protected:
  
  /** Channels. */
  int m_outChannels;
  
  /** Samplerate. */
  int m_outSampleRate;
  
  /** Number of buffers for the buffer partition. Not all APIs can handle this param. */
  int m_outNumOfBuffers;

  /** Status of the decoder unit. **/
  int m_status;
  
  /** The output buffer. */
  void* m_outBuffer;
  
  /** The out buffer size. */
  unsigned int m_outBufferSize;
  
  /** Internal buffer position. Used by performDsp() */
  unsigned int m_bufferPosition;
};

}

#endif
