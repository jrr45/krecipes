/***************************************************************************
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "actionshandlerbase.h"

#include <KLocale>
#include <QListWidget>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QPointer>
#include <KMessageBox>

#include "widgets/dblistviewbase.h"

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"


ActionsHandlerBase::ActionsHandlerBase( DBListViewBase *_parentListView, RecipeDB *db ):
	QObject( _parentListView ), parentListView( _parentListView ), database( db )
{

	kpop = new QMenu( parentListView );
	connect( parentListView,
        SIGNAL( contextMenu( QListWidget *, QListWidgetItem *, const QPoint & ) ),
        SLOT( showPopup( QListWidget *, QListWidgetItem *, const QPoint & ) )
	);
	connect( parentListView,
        SIGNAL( doubleClicked( QListWidgetItem*, const QPoint &, int ) ),
        SLOT( renameElement( QListWidgetItem*, const QPoint &, int ) )
	);
	connect( parentListView,
        SIGNAL( itemRenamed( QListWidgetItem* ) ),
        SLOT( saveElement( QListWidgetItem* ) )
	);
}

void ActionsHandlerBase::rename()
{
    QListWidgetItem * item = parentListView->currentItem();

	if ( item )
		parentListView->rename( item, 0 );
}

void ActionsHandlerBase::addAction( QAction * action )
{
	kpop->addAction( action );
}

void ActionsHandlerBase::showPopup( QListWidget * /*l*/, QListWidgetItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}


void ActionsHandlerBase::renameElement( QListWidgetItem* i, const QPoint &/*p*/, int c )
{
	if ( i )
		parentListView->rename( i, c );
}
