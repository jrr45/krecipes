/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "datablocks/elementlist.h"
//Added by qt3to4:
#include <Q3ValueList>

ElementList::ElementList() : Q3ValueList <Element>()
{}

ElementList::~ElementList()
{}

Element ElementList::getElement( int index ) const
{
	return * ( at( index ) );
}

Element ElementList::findByName( const QString &name ) const
{
	ElementList::const_iterator it_end = end();
	for ( ElementList::const_iterator it = begin(); it != it_end; ++it ) {
		if ( ( *it ).name == name )
			return * it;
	}

	Element el;
	el.id = -1;
	return el;
}

Element ElementList::findByName( const QRegExp &rx ) const
{
	ElementList::const_iterator it_end = end();
	for ( ElementList::const_iterator it = begin(); it != it_end; ++it ) {
		if ( ( *it ).name.find(rx) != -1 )
			return * it;
	}

	Element el;
	el.id = -1;
	return el;
}

bool ElementList::containsId( int id ) const // Search by id (which uses search by item, with comparison defined on header)
{
	Element i;
	i.id = id;
	return ( find( i ) != end() );
}

bool ElementList::containsSubSet( ElementList &el )
{
	ElementList::const_iterator it_end = el.end();
	ElementList::const_iterator it;

	for ( it = el.begin(); it != it_end; ++it ) {
		if ( !containsId( ( *it ).id ) )
			return false;
	}
	return true;
}

