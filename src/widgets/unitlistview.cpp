/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "unitlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "DBBackend/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
 
UnitListView::UnitListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	connect(database,SIGNAL(unitCreated(const Element &)),SLOT(createUnit(const Element &)));
	connect(database,SIGNAL(unitRemoved(int)),SLOT(removeUnit(int)));

	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);
}

void UnitListView::reload()
{
	clear();

	ElementList unitList;
	database->loadUnits(&unitList);

	for ( ElementList::const_iterator ing_it = unitList.begin(); ing_it != unitList.end(); ++ing_it )
		createUnit(*ing_it);
}



StdUnitListView::StdUnitListView( QWidget *parent, RecipeDB *db, bool editable ) : UnitListView(parent,db)
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );

	addColumn(i18n("Unit"));

	setSorting(1);

	if ( editable ) {
		setRenameable(1, true);

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon("filenew", KIcon::NoGroup,16),i18n("&Create"), this, SLOT(createNew()), CTRL+Key_C );
		kpop->insertItem( il->loadIcon("editdelete", KIcon::NoGroup,16),i18n("&Delete"), this, SLOT(remove()), Key_Delete );
		kpop->insertItem( il->loadIcon("edit", KIcon::NoGroup,16), i18n("&Rename"), this, SLOT(rename()), CTRL+Key_R );
		kpop->polish();

		delete il;

		connect(this,SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)), SLOT(showPopup(KListView *, QListViewItem *, const QPoint &)));
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modUnit( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(saveUnit(QListViewItem*)));
	}
}

void StdUnitListView::showPopup(KListView */*l*/, QListViewItem *i, const QPoint &p)
{
	if ( i )
		kpop->exec(p);
}

void StdUnitListView::createNew()
{
	CreateElementDialog* elementDialog=new CreateElementDialog(this,QString(i18n("New Unit")));
	
	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();
	
		//check bounds first
		if ( checkBounds(result) )
			database->createNewUnit(result); // Create the new unit in database
	}
	delete elementDialog;
}

void StdUnitListView::remove()
{
	// Find selected unit item
	QListViewItem *it = currentItem();
	
	if ( it ) {
		int unitID=it->text(0).toInt();

		ElementList recipeDependancies, propertyDependancies;
		database->findUnitDependancies(unitID,&propertyDependancies,&recipeDependancies);
		
		if (recipeDependancies.isEmpty() && propertyDependancies.isEmpty()) database->removeUnit(unitID);
		else {// need warning!
			DependanciesDialog *warnDialog=new DependanciesDialog(0,&recipeDependancies,0,&propertyDependancies);
			if (warnDialog->exec()==QDialog::Accepted) database->removeUnit(unitID);
			delete warnDialog;
		}
	}
}

void StdUnitListView::rename()
{
	QListViewItem *item = currentItem();
	
	if ( item )
		UnitListView::rename( item, 1 );
}

void StdUnitListView::createUnit(const Element &ing)
{
	(void)new QListViewItem(this,QString::number(ing.id),ing.name);
}

void StdUnitListView::removeUnit(int id)
{
	QListViewItem *item = findItem(QString::number(id),0);
	
	Q_ASSERT(item);
	
	delete item;
}

void StdUnitListView::modUnit(QListViewItem* i)
{
	if ( i )
		UnitListView::rename(i, 1);
}

void StdUnitListView::saveUnit(QListViewItem* i)
{
	if ( !checkBounds(i->text(1)) ) {
		reload(); //reset the changed text
		return;
	}

	int existing_id = database->findExistingUnitByName( i->text(1) );
	int unit_id = i->text(0).toInt();
	if ( existing_id != -1 && existing_id != unit_id ) { //category already exists with this label... merge the two
		switch (KMessageBox::warningContinueCancel(this,i18n("This unit already exists.  Continuing will merge these two units into one.  Are you sure?")))
		{
		case KMessageBox::Continue:
		{
			database->mergeUnits(existing_id,unit_id);
			break;
		}
		default: reload(); break;
		}
	}
	else {
		database->modUnit((i->text(0)).toInt(), i->text(1));
	}
}

bool StdUnitListView::checkBounds( const QString &name )
{
	if ( name.length() > database->maxUnitNameLength() ) {
		KMessageBox::error(this,QString(i18n("Unit name cannot be longer than %1 characters.")).arg(database->maxUnitNameLength()));
		return false;
	}

	return true;
}

#include "unitlistview.moc"