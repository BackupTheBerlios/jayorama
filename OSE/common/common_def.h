/* common_def.h
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

#ifndef   __OSE_COMMON_DEFINITIONS_
#define   __OSE_COMMON_DEFINITIONS_


 
/*****         Return values and status             ****/
 
#define		OSE_OK			0
#define		OSE_ERROR_UNKNOWN	1
#define		OSE_ERROR_OPENING	2
#define		OSE_ERROR_WRITE		3
#define		OSE_ERROR_READ		4
#define		OSE_FORMAT_UNKNOWN	5
#define		OSE_THREAD_ERROR	6 


 
/*****         Formats                              ****/

#define	OSE_NULL_FORMAT			0
#define	OSE_AUDIO_PCM_16		1		/* Signed 16 bit data */
#define	OSE_AUDIO_PCM_24		2		/* Signed 24 bit data */
#define	OSE_AUDIO_PCM_32		3		/* Signed 32 bit data */
#define	OSE_AUDIO_FLOAT			4		/* 32 bit float data */
#define	OSE_AUDIO_DOUBLE		5		/* 64 bit float data */
#define	OSE_AUDIO_PCM_INT16		6		/* Signed 16 bit integer data */



/*** Default label for audio data */
typedef	int	audioDataType ;



#endif

