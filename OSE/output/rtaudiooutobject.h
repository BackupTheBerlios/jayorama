/* rtaudiooutobject.h
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

#ifndef __RTAUDIO_OBJ_H
#define __RTAUDIO_OBJ_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rtaudio/RtAudio.h>
#include <rtaudio/RtError.h>
#include <outputobjectbase.h>

#define		DEFAULT_NUM_OF_BUFFERS		4

namespace izsound
{

/**
 * 
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
class RtAudioOutObject : public OutputObjectBase
{

public:  
 
  /**
   * The constructor.
   */
  RtAudioOutObject( RtAudio *audio, int bufferSize = 4096, int sampleRate = 44100 );
  
  /**
   * Destructor.
   */
  ~RtAudioOutObject();
  
  /**
   * Get the stream object pointer.
   */
  RtAudio* getStreamObject() {return m_streamObject;}
  
private:

  /**
   * Write to the device a buffer of data.
   *
   * @param buffer_size Size of buffer in frames.
   * @param buffer The buffer pointer.
   */
  virtual void write( unsigned int buffer_size, void* buffer );  
  
  /** RtAudio objects rack. Everyone represents a device. */
  RtAudio *m_streamObject;
};



}

#endif
