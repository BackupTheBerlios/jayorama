/* oseconfig.cpp
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

#include <oseconfig.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

OSEConfigManager::OSEConfigManager( char *filename = NULL )
{
  if ( filename != NULL )
  	loadFile( filename );
  else
  	saved = false;
}


OSEConfigManager::~OSEConfigManager()
{
  for ( int i = 0; i < parameters.size(); i++)
  {
  	delete parameters[i];
  }
}


int OSEConfigManager::addParameter( string parameter_group, string parameter_name, 
                                     void parameter_value, int parameter_type )
{
  Parameter *p = new Parameter( parameter_group, parameter_name, 
                                parameter_value, parameter_type );
  parameters.push_back( p );
  saved = false;
  return CONFIG_OK;
}


int OSEConfigManager::addParameter( Parameter &p )
{
  Parameter *p = new Parameter( p.parameter_group, p.parameter_name, 
                                p.parameter_value, p.parameter_type );
  parameters.push_back( p );
  return CONFIG_OK;
}
  

int OSEConfigManager::findParameter( vector< Parameter* > &results,
                                      string parameter_group, string parameter_name )
{
  for ( int j = 0; j < parameters.size(); j++ )
  {
  	if (( parameter_group == "none" ) && ( parameter_name == "none" ))
	{
		cerr << "Error: search without valid argoments. Abort." << endl;
		return CONFIG_ERROR; 
	}
	
	if ( parameter_group == "none" )
	{
		if ( parameters[j].parameter_name == parameter_name )
		{
			results.push_back( parameters[j] );
		}
	}
	else if ( parameter_name == "none" )
	{
		if ( parameters[j].parameter_group == parameter_group )
		{
			results.push_back( parameters[j] );
		}
	}
	else if ( parameters[j].parameter_group == parameter_group )
	{
		if ( parameters[j].parameter_name == parameter_name )
		{
			results.push_back( parameters[j] );
		}
	}
  }
  return CONFIG_OK;
}



int OSEConfigManager::getParameterValue( string parameter_group , string parameter_name,
                                         void &parameter_value )
{
  vector< Parameter* > results;
  if (( parameter_group == "none" ) && ( parameter_name == "none" ))
  {
	cerr << "Error: OSEConfigManager::getParameterValue() without valid argoments. Abort." << endl;
	return CONFIG_ERROR; 
  }
  if (( parameter_value == NULL ))
  {
	cerr << "Error: OSEConfigManager::getParameterValue() without valid parameter_value pointer. Abort." << endl;
	return CONFIG_ERROR; 
  }
  findParameter( results, parameter_group, parameter_name );
  if (( results.size() == 0 ))
  {
	return CONFIG_NOT_FOUND; 
  }
  else
  {
  	if (( results.size() != 1 ))
  	{
		return CONFIG_ERROR; 
 	}
	else
		parameter_value = results[0].parameter_value;
  }
  return CONFIG_OK;
}


Parameter* OSEConfigManager::getParameter( string parameter_group , string parameter_name )
{
  vector< Parameter* > results;
  if (( parameter_group == "none" ) && ( parameter_name == "none" ))
  {
	cerr << "Error: OSEConfigManager::getParameter() without valid argoments. Abort." << endl;
	return CONFIG_ERROR; 
  }
  
  findParameter( results, parameter_group, parameter_name );
  if (( results.size() == 0 ))
  {
	return NULL; 
  }
  else
  {
  	if (( results.size() != 1 ))
  	{
		return NULL; 
 	}
	else
		return results[0];
  }
  return NULL;
}


int OSEConfigManager::saveFile()
{
  if ( saved == true )
  	return CONFIG_OK;
  void config_file;
  if ( getParameterValue( "Main", "config_file", config_file ) == CONFIG_NOT_FOUND )
  	return saveAsFile( "none" );
  else
  	return saveAsFile( (string)config_file );
}


int OSEConfigManager::saveAsFile( string filename )
{
  if ( saved == true )
  	return CONFIG_OK;
  
  if ( filename == "none" )
  {
  	string home_dir;
  	&home_dir[0] = getenv( HOME );
	
	filename = home_dir + "/." + DEFAULT_CONFIG_FILENAME;
	addParameter( "Main", "config_file", filename, TYPE_STRING );
  }
  
  vector< Parameter* > results;
  ofstream  OUT( filename );
  
  if ( OUT != NULL )
  {
  	
	findParameter( results, "Main" );
	if ( results.size() == 0 )
	{
		cerr << "Error: OSEConfigManager::saveAsFiler() error with Main group. Abort." << endl;
		return CONFIG_ERROR; 
	}
	for ( int u = 0; u < results.size(); u++)
		writeParameter( OUT, results[u] );
	results.clear();
	
	findParameter( results, "Input" );
	for ( int u = 0; u < results.size(); u++)
		writeParameter( OUT, results[u] );
	results.clear();
	
	findParameter( results, "Dsp" );
	for ( int u = 0; u < results.size(); u++)
		writeParameter( OUT, results[u] );
	results.clear();
	
	findParameter( results, "Output" );
	for ( int u = 0; u < results.size(); u++)
		writeParameter( OUT, results[u] );
	results.clear();
	
	findParameter( results, "Misc" );
	for ( int u = 0; u < results.size(); u++)
		writeParameter( OUT, results[u] );
  }
  saved = true;
  return CONFIG_OK;
  
}



int OSEConfigManager::writeParameter( ofstream &stream, Parameter* p )
{
  if ( temp_group != p->parameter_group )
  {
  	stream << "< " <<  p->parameter_group << " >" << endl;
	stream << endl;
	temp_group = p->parameter_group;
  }
  
  stream << p->parameter_name << " = " << p->parameter_value << " ::: " << p->parameter_type << endl;
  return CONFIG_OK;
}



int OSEConfigManager::loadFile( string filename )
{
  ifstream IN( filename );
  
  if ( IN != NULL )
  {
  }
}