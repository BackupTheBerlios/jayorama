/* mcmixer.cpp
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
  
#include <mcmixer.h>
 
using namespace std;
using namespace izsound;
  

MCMixer::MCMixer( unsigned int bufferSize, unsigned int sampleRate )
{
}
  
MCMixer::MCMixer( unsigned int bufferSize, unsigned int sampleRate , int maxInput , int maxOutput )
{
} 
  
MCMixer::~MCMixer()
{
}  

int MCMixer::getOutputChannels()
{
  int num = 0;
  for ( int j = 0; j < m_outSlots.size(); j++ )
  {
  	if (m_outSlots[j] != 0) num++;
  }
  return num;
}
  
int MCMixer::getInputChannels()
{
  int num = 0;
  for ( int j = 0; j < m_inSlots.size(); j++ )
  {
  	if (m_inSlots[j] != 0) num++;
  }
  return num;
}
   
void MCMixer::enableChannel( int in_channelSlotId, int out_channelSlotId, bool enableStatus )
{
  m_connections[ in_channelSlotId ][ out_channelSlotId ] = enableStatus;
}

void MCMixer::performDsp()
{
  
  for ( int j = 0; j < getOutputChannels(); j++ )
  {
  	for ( unsigned int index = 0; index < m_bufferSize; index++)
	{
		// clean up
		SlotData* output = m_outSlots[j];
		(*output)[0].clear();
		(*output)[1].clear();
		
		double sample_left  = 0.0;
		double sample_right = 0.0;
		
		for ( int z = 0; z < getInputChannels(); z++ )
		{
			if ( m_connections[ z ][ j ] == true )
			{
				SlotData* input = m_inSlots[z];
				sample_left  += (*input)[0][ (int)index ];
				sample_right += (*input)[1][ (int)index ];
			}
		}
		//finally, push in...
		(*output)[0].push_back(sample_left);
		(*output)[1].push_back(sample_right);
	}
  }  
}

void MCMixer::update()
{
  for ( int j = 0; j < getInputChannels(); j++ )
  {
  	while (getOutputChannels() > m_connections[j].size())
  	{
		m_connections[j].push_back( false );
	}
  } 
}
