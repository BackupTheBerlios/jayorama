/* outputbase.cpp
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


#include <outputbase.h>
#include <iostream>
#include <math.h>


using namespace std;
using namespace izsound;

OutputBase::OutputBase(  const unsigned int &bufferSize,
                         const unsigned int &sampleRate)
:DspUnit(sampleRate, MAX_STEREO_OUTPUTS, 0)
{
  // Init
  m_status = OUTPUT_OK;
  m_outBufferSize = bufferSize;
#ifdef _FLOAT_ENGINE
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	m_outBuffer[i] = new float[m_outBufferSize];
  }
#endif 
#ifdef _DOUBLE_ENGINE
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	m_outBuffer[i] = new double[m_outBufferSize];
  }
#endif
}


OutputBase::~OutputBase()
{
#ifdef _FLOAT_ENGINE
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	if ( m_outBuffer[i] != NULL )
		delete[] (float*)m_outBuffer[i];
  }
#endif 
#ifdef _DOUBLE_ENGINE
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	if ( m_outBuffer[i] != NULL )
  		delete[] (double*)m_outBuffer[i];
  }
#endif
}


void OutputBase::performDsp()
{ 
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
	// Init
	SlotData* data = m_inSlots[i];
	unsigned int nframes = min((*data)[0].size(), (*data)[1].size());
	
	// Let's go
	for (unsigned int j = 0; j < nframes; ++j)
	{
#ifdef _FLOAT_ENGINE
		float *_buffer = (float*)m_outBuffer[i];
#endif 
#ifdef _DOUBLE_ENGINE
		double *_buffer = (double*)m_outBuffer[i];
#endif
		_buffer[j] = (*data)[0][j++];
		_buffer[i]++;
		_buffer[j--] = (*data)[1][j];
		_buffer[i]++;
		
		// We write to the device if needed
		if (m_bufferPosition[i] == m_outBufferSize)
		{
			write( i, m_outBufferSize, m_outBuffer[i] );
		}
	}
  }
}

