/* rtaudioapi.h
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

#ifndef __RTAUDIO_API_H
#define __RTAUDIO_API_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rtaudio/RtAudio.h>
#include <rtaudio/RtError.h>
#include <outputapibase.h>
#include <rtaudiooutobject.h>

#define		DEFAULT_NUM_OF_BUFFERS		4

namespace izsound
{

/**
 * 
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
class RtAudioAPI : public OutputAPIBase
{

public:  
 
  /**
   * The constructor.
   */
  RtAudioAPI();
  
  /**
   * Destructor.
   */
  ~RtAudioAPI();

public:  
  
  /**
   * Get an output DspUnit object.
   */
  virtual OutputObjectBase* getOutputUnit( int channels, int bufferSize, 
                                           int numOfBuffers, int sampleRate,
					   int device = -1 );
  
  /**
   * Retrieve info of a specific device.
   *
   * @param device_id The device id.
   */
  virtual void getDeviceInfo( int device_id, OSEDeviceInfo* info );

private:

  /** Audio format. */
  RtAudioFormat rt_audioFormat;
  
  int last_rt_device_id;
  
  int free_output_channels_on_last_id;
  
  
};



}

#endif
