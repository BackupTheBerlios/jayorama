/* oseconfig.h
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

#ifndef __OSE_CONFIG_H
#define __OSE_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <vector>


/*****	Parameter types		*****/
#define		TYPE_STRING		0
#define		TYPE_INT		1
#define		TYPE_FLOAT		2
#define		TYPE_DOUBLE		3

/*****	Config retvalues	*****/
#define		CONFIG_ERROR		0
#define		CONFIG_OK		1
#define		CONFIG_NOT_FOUND	2

/*****	Default file name	*****/
#define		DEFAULT_CONFIG_FILENAME		".ose.config"


using namespace std;


class Parameter
{

public:

  Parameter( string _parameter_group, string _parameter_name, 
           void _parameter_value, int _parameter_type )
  { 
  	parameter_group = _parameter_group;
	parameter_name  = _parameter_name; 
	parameter_value = _parameter_value;
	parameter_type  = _parameter_type;
  }
  ~Parameter();

  string parameter_group;
  string parameter_name;
  void parameter_value; 
  int parameter_type;
};



/**
 * OSE configuration class
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
class OSEConfigManager
{

public:  
 
  /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @param sampleRate DspUnit samplerate parameter. Actually UNUSED.
   */
  OSEConfigManager( char *filename = NULL );
  
  /**
   * Destructor.
   */
  ~OSEConfigManager();

public:  
  
  /**
   * Get a parameter.
   */
  int getParameterValue( string parameter_group = "none", 
                         string parameter_name = "none",
			 void &parameter_value == NULL );
  
  /**
   * Get a parameter.
   */
  Parameter* getParameter( string parameter_group  = "none", 
                           string parameter_name = "none" );
  
  /**
   * Add a parameter.
   */
  int addParameter( string parameter_group, string parameter_name, 
                     void parameter_value, int parameter_type );
  
  /**
   * Add a parameter.
   */
  int addParameter( Parameter &p );

  /**
   * Save.
   */
  int saveFile();
  
  /**
   * Save.
   */
  int saveAsFile( string filename );
  
  /**
   * Load.
   */
  int loadFile( string filename );
  
private:


  /**
   * Find parameters with values as indicated and add them to results vector.
   */
  int findParameter( vector< Parameter* > &results,
                      string parameter_group = "none", string parameter_name = "none" );

  vector< Parameter* > parameters;
  
  /*** Internal use for saving and loading Parameters. ***/
  string temp_group;
  
  /*** Internal use for saving. ***/
  bool saved;
};





#endif
