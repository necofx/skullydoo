/************************************************************************/
/* SkullyDoo - Segmentador y visualizador de imagenes tridimensionales  */	
/* CVS Id: $Id: TCLFilterStringParameter.h,v 1.2 2003/05/23 19:18:59 sebasfiorent Exp $								
/* Donado a la comunidad Open Source					*/	
/* (c) 2002 Software del Centro http://www.softwaredelcentro.com.ar	*/
/* Sebastián Fiorentini (sebas@softwaredelcentro.com.ar)		*/
/* Ignacio Larrabide (nacho@softwaredelcentro.com.ar)			*/
/* Argentina								*/
/************************************************************************/

#ifndef _TCLFilterStringParameter_h_
#define _TCLFilterStringParameter_h_
#ifdef _MSC_VER
	#pragma warning  ( disable : 4786 )
#endif

#include "TCLFilterParameter.h"
#include <string>
#include <vector>

class TCLFilterStringParameter : public TCLFilterParameter  {
public:
	NewMacro(TCLFilterStringParameter);
};

#endif
