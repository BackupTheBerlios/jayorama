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

#include <SndObj/SndObj.h>
#include <SndObj/Mix.h>
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
  
  MCMixer( unsigned int sampleRate = 44100 );
  MCMixer( unsigned int sampleRate = 44100, int maxInput = 16, int maxOutput = 8 ); 
  ~MCMixer();

  protected:

  /**
   * Performs the decoding job, mixing all together.
   */
  virtual void performDsp();
  
  private:

};

}

#endif