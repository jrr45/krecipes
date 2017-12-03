/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "unitdelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "dialogs/createunitdialog.h"
#include "backends/recipedb.h"
#include "datablocks/unit.h"

#include <KComboBox>

//#include <kdebug.h>


UnitDelegate::UnitDelegate(QObject *parent): QStyledItemDelegate(parent)
{
	m_database = 0;
}

void UnitDelegate::loadAllUnitsList( RecipeDB * database )
{
	m_database = database;
	m_database->loadUnits( &m_unitList );
	//FIXME: it would be nice if we could get this hashmap directly from RecipeDB
	UnitList::const_iterator it = m_unitList.constBegin();
	while ( it != m_unitList.constEnd() ) {
		m_singularNameToIdMap[it->name()] = it->id();
		m_pluralNameToIdMap[it->plural()] = it->id();
		++it;
	}
	//TODO: connect database signals
	/*connect( database, SIGNAL(ingredientCreated(const Element&)),
		this, SLOT(ingredientCreatedSlot(const Element&)) );
	connect( database, SIGNAL(ingredientRemoved(int)),
		this, SLOT(ingredientRemovedSlot(int)) );*/
