/* mcmixer.h
 *
 *  Copyright (C) 2004 Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 * 
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
  

#ifndef __MCMIXER_H
#define __MCMIXER_H


#include <dspunit.h>

namespace izsound
{

/**
 * The MultiChannel Mixer, mixes more inputs in some outputs.
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
class MCMixer : public DspUnit
{
  public:
  
  /**
   * The constructor.
   *
   * @param bufferSize That's the sample number to be mixed.
   * @param sampleRate DspUnit samplerate.
   */
  MCMixer( unsigned int bufferSize = 4096, unsigned int sampleRate = 44100 );
  
  /**
   * The constructor.
   *
   * @param bufferSize That's the sample number to be mixed.
   * @param sampleRate DspUnit samplerate.
   * @param maxInput The maximum DspUnit input number.
   * @param maxOutput The maximum DspUnit output number.
   */
  MCMixer( unsigned int bufferSize = 4096, unsigned int sampleRate = 44100, 
           int maxInput = 16, int maxOutput = 8 ); 
  
  /**
   * The destructor.
   */
  ~MCMixer();
  
  /**
   * Get the number of connected output channels of the MCMixer.
   *
   * @return The number of connected output channels.
   */
  int getOutputChannels();
  
  /**
   * Get the number of connected input channels of the MCMixer.
   *
   * @return The number of connected input channels.
   */
  int getInputChannels();
  
   /**
   * Resize the connection vector to the right value and init
   * connection status with new devices to disconnected for each
   * input channel.
   * 
   * WARNING!!!! After connecting a slot (a new channel) you need always to call
   * update(). If not a connection error or a
   * segfault will happen...
   */
  void update();
  
//   /**
//    * Connects the DSP unit with another one. The DSP unit will precede the
//    * unit it is connected to. If the provided slots numbers are not in the valid
//    * ranges, then nothing is done. This function override the default one
//    * (NOT to be used!) of DspUnit class.
//    *
//    * @param dspUnit The dspUnit to connect to.
//    * @param mySlot The output slot number to connect from.
//    * @param itsSlot The input slot number to connect to.
//    * @return An ID value defining the channel connected to.
//    */
//   int connectToChannel(DspUnit* dspUnit, const unsigned int &mySlot,
//                const unsigned int &itsSlot);


  /**
   * Enable input channel to be mixed over an output channel.
   *
   * @param in_channelId The input channel ID returned from connectToChannel
   *                     function.
   * @param out_channelId A valid ID for the out channel (ID can be from
                          0 to getOutputChannels() ).
   * @param itsSlot The input slot number to connect to.
   */    
  void enableChannel( int in_channelSlotId, int out_channelSlotId, bool enableStatus );

  protected:

  /**
   * Performs the job, mixing all together.
   */
  virtual void performDsp();
  
  private:
  
  /**
   * Keeps history of all mixer internal input-output connections in this way:
   * bool connected = m_connections[ input_unit ][ output_unit ]  .
   */
  vector< vector< bool > > m_connections;
  
  /*** Number of samples to be mixed every performDsp() ***/
  unsigned int m_bufferSize;
  
//   /**
//    * Resize the connection vector to the right value and init
//    * connection status with new device to disconnected.
//    */
//   void addOutputConnection();
//   
//   /**
//    * Resize the connection vector to the right value and init
//    * connection status with new device to disconnected.
//    */
//   void addInputConnection();

};

}

#endif