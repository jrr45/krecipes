/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeactionshandler.h"

#include <qwidget.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kapplication.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kprogress.h>

#include "exporters/cookmlexporter.h"
#include "exporters/htmlexporter.h"
#include "exporters/kreexport.h"
#include "exporters/mmfexporter.h"
#include "exporters/recipemlexporter.h"

#include "widgets/recipelistview.h"
#include "widgets/categorylistview.h"
#include "backends/recipedb.h"

RecipeActionsHandler::RecipeActionsHandler( KListView *_parentListView, RecipeDB *db, int actions ) : QObject( _parentListView ),
		parentListView( _parentListView ),
		database( db )
{
	KIconLoader * il = new KIconLoader;

	kpop = new KPopupMenu( parentListView );
	if ( actions & Open )
		kpop->insertItem( il->loadIcon( "ok", KIcon::NoGroup, 16 ), i18n( "&Open" ), this, SLOT( open() ), Qt::CTRL + Qt::Key_L );
	if ( actions & Edit )
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Edit" ), this, SLOT( edit() ), Qt::CTRL + Qt::Key_E );
	if ( actions & Export )
		kpop->insertItem( il->loadIcon( "fileexport", KIcon::NoGroup, 16 ), i18n( "E&xport" ), this, SLOT( recipeExport() ), 0 );
	if ( actions & RemoveFromCategory )
		remove_from_cat_item = kpop->insertItem( il->loadIcon( "editshred", KIcon::NoGroup, 16 ), i18n( "Remove From &Category" ), this, SLOT( removeFromCategory() ), Qt::CTRL + Qt::Key_C );
	if ( actions & Remove )
		kpop->insertItem( il->loadIcon( "editshred", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
	if ( actions & AddToShoppingList )
		kpop->insertItem( il->loadIcon( "trolley", KIcon::NoGroup, 16 ), i18n( "&Add to Shopping List" ), this, SLOT( addToShoppingList() ), Qt::CTRL + Qt::Key_A );
	kpop->polish();

	catPop = new KPopupMenu( parentListView );
	if ( actions & ExpandAll )
		catPop->insertItem( i18n( "&Expand All" ), this, SLOT( expandAll() ), Qt::CTRL + Qt::Key_Plus );
	if ( actions & CollapseAll )
		catPop->insertItem( i18n( "&Collapse All" ), this, SLOT( collapseAll() ), Qt::CTRL + Qt::Key_Minus );
	if ( actions & CategoryExport )
		catPop->insertItem( il->loadIcon( "fileexport", KIcon::NoGroup, 16 ), i18n( "E&xport" ), this, SLOT( categoryExport() ), 0 );

	catPop->polish();

	delete il;

	connect( parentListView, SIGNAL( contextMenu( KListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, Q3ListViewItem *, const QPoint & ) ) );
	connect( parentListView, SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ), SLOT( open() ) );
}

void RecipeActionsHandler::exec( ItemType type, const QPoint &p )
{
	if ( type == Recipe ) {
		if ( kpop->idAt( 0 ) != -1 )
			kpop->exec( p );
	}
	else if ( type == Category ) {
		if ( catPop->idAt( 0 ) != -1 )
			catPop->exec( p );
	}
}

void RecipeActionsHandler::showPopup( KListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i ) { // Check if the QListViewItem actually exists
		if ( i->rtti() == 1000 ) {
			kpop->setItemVisible( remove_from_cat_item, i->parent() && i->parent()->rtti() == 1001 );
			exec( Recipe, p );
		}
		else if ( i->rtti() == 1001 )  //is a category... don't pop-up for an empty category though
			exec( Category, p );
	}
}

void RecipeActionsHandler::open()
{
	Q3ListViewItem * it = parentListView->selectedItem();
	if ( it ) {
		if ( it->rtti() == 1000 ) { //RecipeListItem
			RecipeListItem * recipe_it = ( RecipeListItem* ) it;
			emit recipeSelected( recipe_it->recipeID(), 0 );
		}
		#if 0
		else if ( it->rtti() == 1001 && it->firstChild() )  //CategoryListItem and not empty
		{
			Q3ValueList<int> ids;

			//do this to only iterate over children of 'it'
			Q3ListViewItem *pEndItem = NULL;
			Q3ListViewItem *pStartItem = it;
			do
			{
				if ( pStartItem->nextSibling() )
					pEndItem = pStartItem->nextSibling();
				else
					pStartItem = pStartItem->parent();
			}
			while ( pStartItem && !pEndItem );

			Q3ListViewItemIterator iterator( it );
			while ( iterator.current() != pEndItem )
			{
				if ( iterator.current() ->rtti() == 1000 ) {
					RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
					if ( ids.find( recipe_it->recipeID() ) == ids.end() ) {
						ids.append( recipe_it->recipeID() );
					}
				}
				++iterator;
			}
			emit recipesSelected( ids, 0 );
		}
		#endif
	}
}

void RecipeActionsHandler::edit()
{
	Q3ListViewItem * it = parentListView->selectedItem();
	if ( it && it->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* ) it;
		emit recipeSelected( recipe_it->recipeID(), 1 );
	}
	else
		KMessageBox::sorry( kapp->mainWidget(), i18n("No recipe selected!"), i18n("Edit Recipe") );
}

void RecipeActionsHandler::recipeExport()
{
	if ( parentListView->selectedItem() && parentListView->selectedItem()->rtti() >= 1000 ) {
		if ( parentListView->selectedItem()->rtti() == 1001 )
			categoryExport();
		else if ( parentListView->selectedItem() ->rtti() == 1000 ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) parentListView->selectedItem();
				exportRecipe( recipe_it->recipeID(), i18n( "Export Recipe" ), recipe_it->title(), database );
		}
	}
	else //if nothing selected, export all visible recipes
	{
		Q3ValueList<int> ids = getAllVisibleItems();
		if ( ids.count() > 0 ) {
			switch ( KMessageBox::questionYesNo( kapp->mainWidget(), i18n("No recipes are currently selected.\nWould you like to export all recipes in the current view?")) )
			{
			case KMessageBox::Yes:
				exportRecipes( ids, i18n( "Export Recipes" ), i18n( "Recipes" ), database );
				break;
			default: break;
			}
		}
		else
			KMessageBox::sorry( kapp->mainWidget(), i18n("No recipe selected!"), i18n("Export") );
	}
}

void RecipeActionsHandler::removeFromCategory()
{
	Q3ListViewItem * it = parentListView->selectedItem();
	if ( it && it->rtti() == 1000 ) {
		if ( it->parent() != 0 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) it;
			int recipe_id = recipe_it->recipeID();

			CategoryListItem *cat_it = ( CategoryListItem* ) it->parent();
			database->removeRecipeFromCategory( recipe_id, cat_it->categoryId() );
		}
	}
}

void RecipeActionsHandler::remove
	()
{
	Q3ListViewItem * it = parentListView->selectedItem();
	if ( it && it->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* ) it;
		emit recipeSelected( recipe_it->recipeID(), 2 );
	}
}

void RecipeActionsHandler::addToShoppingList()
{
	Q3ListViewItem * it = parentListView->selectedItem();
	if ( it && it->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* ) it;
		emit recipeSelected( recipe_it->recipeID(), 3 );
	}
}

void RecipeActionsHandler::expandAll()
{
	Q3ListViewItemIterator it( parentListView );
	while ( it.current() ) {
		Q3ListViewItem * item = it.current();
		item->setOpen( true );
		++it;
	}
}

void RecipeActionsHandler::collapseAll()
{
	Q3ListViewItemIterator it( parentListView );
	while ( it.current() ) {
		Q3ListViewItem * item = it.current();
		item->setOpen( false );
		++it;
	}
}

void RecipeActionsHandler::categoryExport()
{
	if ( parentListView->selectedItem() ) {
		CategoryListItem * cat_it = ( CategoryListItem* ) parentListView->selectedItem();
		Q3ValueList<int> ids;

		//do this to only iterate over children of 'cat_it'
		Q3ListViewItem *pEndItem = NULL;
		Q3ListViewItem *pStartItem = cat_it;
		do {
			if ( pStartItem->nextSibling() )
				pEndItem = pStartItem->nextSibling();
			else
				pStartItem = pStartItem->parent();
		}
		while ( pStartItem && !pEndItem );

		Q3ListViewItemIterator iterator( cat_it );
		while ( iterator.current() != pEndItem ) {
			if ( iterator.current() ->rtti() == 1000 ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
				int recipe_id = recipe_it->recipeID();

				if ( ids.find( recipe_id ) == ids.end() )
					ids.append( recipe_id );
			}
			++iterator;
		}

		exportRecipes( ids, i18n( "Export Recipes" ), cat_it->categoryName(), database );
	}
}

void RecipeActionsHandler::exportRecipe( int id, const QString & caption, const QString &selection, RecipeDB *db )
{
	Q3ValueList<int> ids;
	ids.append( id );

	exportRecipes( ids, caption, selection, db );
}

void RecipeActionsHandler::exportRecipes( const Q3ValueList<int> &ids, const QString & caption, const QString &selection, RecipeDB *database )
{
	KFileDialog * fd = new KFileDialog( QString::null,
	                                    QString( "*.kre|%1 (*.kre)\n"
	                                             "*.kreml|Krecipes (*.kreml)\n"
	                                             //"*.cml|CookML (*.cml)\n"
	                                             "*.html|%2 (*.html)\n"
	                                             "*.mmf|Meal-Master (*.mmf)\n"
	                                             "*.xml|RecipeML (*.xml)" ).arg( i18n( "Compressed Krecipes format" ) ).arg( i18n( "Web page" ) ),
	                                    0, "export_dlg", true );
	fd->setCaption( caption );
	fd->setOperationMode( KFileDialog::Saving );
	fd->setSelection( selection );
	if ( fd->exec() == KFileDialog::Accepted ) {
		QString fileName = fd->selectedFile();
		if ( !fileName.isNull() ) {
			BaseExporter * exporter;
			if ( fd->currentFilter() == "*.xml" )
				exporter = new RecipeMLExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.mmf" )
				exporter = new MMFExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.html" )
				exporter = new HTMLExporter( database, fileName, fd->currentFilter(), 650 );
			else if ( fd->currentFilter() == "*.cml" )
				exporter = new CookMLExporter( fileName, fd->currentFilter() );
			else {
				CategoryTree *cat_structure = new CategoryTree;
				database->loadCategories( cat_structure );
				exporter = new KreExporter( cat_structure, fileName, fd->currentFilter() );
			}

			int overwrite = -1;
			if ( QFile::exists( exporter->fileName() ) ) {
				overwrite = KMessageBox::warningYesNo( 0, QString( i18n( "File \"%1\" exists.  Are you sure you want to overwrite it?" ) ).arg( exporter->fileName() ), i18n( "Saving recipe" ) );
			}

			if ( overwrite == KMessageBox::Yes || overwrite == -1 ) {
				KProgressDialog progress_dialog( 0, "export_progress_dialog", QString::null, i18n( "Saving recipes..." ) );
				exporter->exporter( ids, database, &progress_dialog );
			}
			delete exporter;
		}
	}
	delete fd;
}

Q3ValueList<int> RecipeActionsHandler::getAllVisibleItems()
{
	Q3ValueList<int> ids;

	Q3ListViewItemIterator iterator( parentListView );
	while ( iterator.current() ) {
		if ( iterator.current() ->isVisible() && iterator.current() ->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
			int recipe_id = recipe_it->recipeID();

			if ( ids.find( recipe_id ) == ids.end() )
				ids.append( recipe_id );
		}

		++iterator;
	}

	return ids;
}

#include "recipeactionshandler.moc"

