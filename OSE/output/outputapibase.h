/* outputapibase.h
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


#ifndef _OUTPUTAPIBASE_H
#define _OUTPUTAPIBASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <common_def.h>
#include <vector>
#include <string>
#include <outputobjectbase.h>



using namespace std;

namespace izsound
{

/**
 * 
 *
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 


typedef struct OSEDeviceInfo
{
    int deviceId;                 // Device number or id
    std::string name;             // Character string device identifier.
    bool probed;                  // true if the device capabilities were successfully probed.
    int outputChannels;           // Maximum output channels supported by device.
    int inputChannels;            // Maximum input channels supported by device.
    int duplexChannels;           // Maximum simultaneous input/output channels supported by device.
    bool isDefault;               // true if this is the default output or input device.
    std::vector<int> sampleRates; // Supported sample rates.
//     RtAudioFormat nativeFormats;  // Bit mask of supported data formats.
};
  
  

class OutputAPIBase 
{
  
  /*****************************************************************
   * ---- >> Functions TO BE reimplemented... << ----
   */
   
public:  
  
  /**
   * Get an output DspUnit object.
   */
  virtual OutputObjectBase* getOutputUnit( int channels, int bufferSize, 
                                           int numOfBuffers, int sampleRate,
					   int device = -1 ) = 0;
  
  /**
   * Retrieve info of a specific device.
   *
   * @param device_id The device id.
   */
  virtual void getDeviceInfo( int device_id, OSEDeviceInfo* info ) = 0;
  
  /*
   * ---- >> End of Functions TO BE reimplemented... << ----
   ******************************************************************/
    
public:
 
 /**
   * The constructor. 
   */
  OutputAPIBase();
  
  /**
   * Destructor.
   */
  virtual ~OutputAPIBase();
  
public:
  
  /**
   * Info about the status of output API.
   *
   * @return The status of the output API.
   */
  int status() { return m_status; }
  
  /**
   * Retrieve the API support name.
   */
  virtual string getAPIName()	{ return m_apiName; }
  
  /**
   * Retrieve the number of available devices.
   */
  virtual int getDeviceCount() { return m_nDevices; }
  
  
protected:
  
  /** Status of the decoder unit. **/
  int m_status;
  
  /** API name. **/
  string m_apiName;
  
  /** The output objects vector. */
  vector< OutputObjectBase* > m_outputObjects;
  
  /** Number of devices. */
  int m_nDevices;

};

}

#endif
