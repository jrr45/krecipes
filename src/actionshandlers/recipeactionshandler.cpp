/***************************************************************************
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeactionshandler.h"

#include <qwidget.h>
#include <QClipboard>
#include <QTextStream>
#include <QList>
#include <QPointer>

#include <kapplication.h>
#include <kfiledialog.h>
#include <QTreeWidget>
#include <klocale.h>
#include <kmessagebox.h>
#include <QMenu>
#include <QAction>
#include <kprogressdialog.h>
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTemporaryDir>

#include <KConfigGroup>
#include <QStandardPaths>
#include <KSharedConfig>

#include "dialogs/recipeinput/selectcategoriesdialog.h"

#include "exporters/cookmlexporter.h"
#include "exporters/xsltexporter.h"
#include "exporters/htmlexporter.h"
#include "exporters/htmlbookexporter.h"
#include "exporters/kreexporter.h"
#include "exporters/mmfexporter.h"
#include "exporters/recipemlexporter.h"
#include "exporters/mx2exporter.h"
#include "exporters/plaintextexporter.h"
#include "exporters/rezkonvexporter.h"

#include "widgets/recipelistview.h"
#include "widgets/categorylistview.h"

#include "backends/recipedb.h"

RecipeActionsHandler::RecipeActionsHandler( QTreeWidget *_parentListView, RecipeDB *db ) :
		QObject( _parentListView ),
		parentListView( _parentListView ),
		database( db ),
		categorizeAction( 0 ),
		removeFromCategoryAction( 0 )
{
	kpop = new QMenu( parentListView );
	catPop = new QMenu( parentListView );

	connect( parentListView,
        SIGNAL( contextMenu( QTreeWidget *, QTreeWidgetItem *, const QPoint & ) ),
        SLOT( showPopup( QTreeWidget *, QTreeWidgetItem *, const QPoint & ) )
	);
	connect( parentListView,
        SIGNAL( doubleClicked( QTreeWidgetItem*, const QPoint &, int ) ),
		SLOT( open() )
	);
	connect( parentListView,
		SIGNAL( selectionChanged() ), SLOT( selectionChangedSlot() ) );

	connect( this, SIGNAL( printDone() ), 
		this, SLOT( printDoneSlot() ), Qt::QueuedConnection );
}

void RecipeActionsHandler::addRecipeAction( QAction * action )
{
	kpop->addAction( action );
}

void RecipeActionsHandler::addCategoryAction( QAction * action )
{
	catPop->addAction( action );
}

void RecipeActionsHandler::setCategorizeAction( QAction * action )
{
	categorizeAction = action;
	kpop->addAction( action );
}

void RecipeActionsHandler::setRemoveFromCategoryAction( QAction * action )
{
	removeFromCategoryAction = action;
	kpop->addAction( action );
}

void RecipeActionsHandler::exec( ItemType type, const QPoint &p )
{
	if ( type == Recipe ) {
        //if ( kpop->idAt( 0 ) != -1 )
			kpop->exec( p );
	}
	else if ( type == Category ) {
//        if ( catPop->idAt( 0 ) != -1 )//FIXME what did this belong to?
			catPop->exec( p );
	}
}

void RecipeActionsHandler::showPopup( QTreeWidget * /*l*/, QTreeWidgetItem *i, const QPoint &p )
{
	if ( i ) { // Check if the QListViewItem actually exists
		if ( i->rtti() == 1000 ) {
			if ( categorizeAction )
				categorizeAction->setVisible( i->parent() && i->parent()->rtti() == 1006 );
			if ( removeFromCategoryAction )
				removeFromCategoryAction->setVisible( i->parent() && i->parent()->rtti() == 1001 );
			exec( Recipe, p );
		}
		else if ( i->rtti() == 1001 )  //is a category... don't pop-up for an empty category though
			exec( Category, p );
	}
}

QList<int> RecipeActionsHandler::recipeIDs( const QList<QTreeWidgetItem *> &items ) const
{
	QList<int> ids;

    QListIterator<QTreeWidgetItem *> it(items);
    const QTreeWidgetItem *item;
	while ( it.hasNext() ) {
		item = it.next();
		if ( item->rtti() == 1000 ) { //RecipeListItem
			RecipeListItem * recipe_it = ( RecipeListItem* ) item;
			if ( ids.indexOf( recipe_it->recipeID() ) == -1 )
				ids << recipe_it->recipeID();
		}
		else if ( item->rtti() == 1001 ) {
			CategoryListItem *cat_it = ( CategoryListItem* ) item;
			ElementList list;
			database->loadRecipeList( &list, cat_it->element().id, true );
	
			for ( ElementList::const_iterator cat_it = list.constBegin(); cat_it != list.constEnd(); ++cat_it ) {
				if ( ids.indexOf( (*cat_it).id ) == -1 )
					ids << (*cat_it).id;
			}
		}
	}

	return ids;
}

void RecipeActionsHandler::open()
{
    const QList<QTreeWidgetItem*> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QList<int> ids = recipeIDs(items);
		if ( ids.count() == 1 )
			emit recipeSelected(ids.first(),0);
		else if ( ids.count() > 0 )
			emit recipesSelected(ids,0);
		#if 0
		else if ( it->rtti() == 1001 && it->firstChild() )  //CategoryListItem and not empty
		{
			QList<int> ids;

			//do this to only iterate over children of 'it'
            QTreeWidgetItem *pEndItem = NULL;
            QTreeWidgetItem *pStartItem = it;
			do
			{
				if ( pStartItem->nextSibling() )
					pEndItem = pStartItem->nextSibling();
				else
					pStartItem = pStartItem->parent();
			}
			while ( pStartItem && !pEndItem );

            QTreeWidgetItemIterator iterator( it );
			while ( iterator.current() != pEndItem )
			{
				if ( iterator.current() ->rtti() == 1000 ) {
					RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
					if ( ids.indexOf( recipe_it->recipeID() ) == -1 ) {
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

void RecipeActionsHandler::categorize()
{
    QList<QTreeWidgetItem*> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		ElementList categoryList;
		QPointer<SelectCategoriesDialog> editCategoriesDialog = new SelectCategoriesDialog( parentListView, categoryList, database );
	
		if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
			editCategoriesDialog->getSelectedCategories( &categoryList ); // get the category list chosen
			
            QListIterator<QTreeWidgetItem *> it(items);
            QTreeWidgetItem *item;
			while ( it.hasNext() ) {
				item = it.next();
				if ( item->parent() != 0 ) {
					RecipeListItem * recipe_it = ( RecipeListItem* ) item;
					int recipe_id = recipe_it->recipeID();
	
					database->categorizeRecipe( recipe_id, categoryList );
				}
			}
		}

		delete editCategoriesDialog;
	}
}

void RecipeActionsHandler::edit()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( items.count() > 1 )
        KMessageBox::sorry( kapp->activeWindow(), i18n("Please select only one recipe."), i18n("Edit Recipe") );
	else if ( items.count() == 1 && items.at(0)->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* ) items.at(0);
		emit recipeSelected( recipe_it->recipeID(), 1 );
	}
	else //either nothing was selected or a category was selected
        KMessageBox::sorry( kapp->activeWindow(), i18n("No recipes selected."), i18n("Edit Recipe") );
}

void RecipeActionsHandler::recipeExport()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QList<int> ids = recipeIDs( items );

		QString title;
		if ( items.count() == 1 && items.at(0)->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) items.at(0);
			title = recipe_it->title();
		}
		else
			title = i18n( "Recipes" );

		exportRecipes( ids, i18n( "Export Recipe" ), title, database );
	}
	else //if nothing selected, export all visible recipes
	{
		QList<int> ids = getAllVisibleItems();
		if ( ids.count() > 0 ) {
            switch ( KMessageBox::questionYesNo( kapp->activeWindow(), i18n("No recipes are currently selected.\nWould you like to export all recipes in the current view?")) )
			{
			case KMessageBox::Yes:
				exportRecipes( ids, i18n( "Export Recipes" ), i18n( "Recipes" ), database );
				break;
			default: break;
			}
		}
		else
            KMessageBox::sorry( kapp->activeWindow(), i18n("No recipes selected."), i18n("Export") );
	}
}

void RecipeActionsHandler::recipePrint()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( !items.isEmpty() ) {
		QList<int> ids = recipeIDs( items );
		printRecipes( ids, database );
	}
	else //if nothing selected, print all visible recipes
	{
		QList<int> ids = getAllVisibleItems();
		if ( !ids.isEmpty() ) {
            switch ( KMessageBox::questionYesNo( kapp->activeWindow(),
			i18n("No recipes are currently selected.\n"
			"Would you like to print all recipes in the current view?")) )
			{
			case KMessageBox::Yes:
				printRecipes( ids, database );
				break;
			default: break;
			}
		}
		else
            KMessageBox::sorry( kapp->activeWindow(), i18n("No recipes selected."), i18n("Print") );
	}
}

void RecipeActionsHandler::removeFromCategory()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
        QListIterator<QTreeWidgetItem *> it(items);
        QTreeWidgetItem *item;
		while ( it.hasNext() ) {
			item = it.next();
			if ( item->parent() != 0 ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) item;
				int recipe_id = recipe_it->recipeID();
	
				CategoryListItem *cat_it = ( CategoryListItem* ) item->parent();
				database->removeRecipeFromCategory( recipe_id, cat_it->categoryId() );
			}
		}
	}
}

void RecipeActionsHandler::remove()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QList<int> recipe_ids;
        QListIterator<QTreeWidgetItem*> it(items);
        QTreeWidgetItem *item;
		while ( it.hasNext() ) {
			item = it.next();
			if ( item->rtti() == RECIPELISTITEM_RTTI ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) item;
				recipe_ids.append( recipe_it->recipeID() );
			}
		}
		emit recipesSelected( recipe_ids, 2 );
	}
}

void RecipeActionsHandler::addToShoppingList()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
        QListIterator<QTreeWidgetItem *> it(items);
        QTreeWidgetItem *item;
		while ( it.hasNext() ) {
			item = it.next();
			if ( item->parent() != 0 ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) item;
				emit recipeSelected( recipe_it->recipeID(), 3 );
			}
		}
	}
}

void RecipeActionsHandler::expandAll()
{
    QTreeWidgetItemIterator it( parentListView );
	while ( it.current() ) {
        QTreeWidgetItem * item = it.current();
		item->setOpen( true );
		++it;
	}
}

void RecipeActionsHandler::collapseAll()
{
    QTreeWidgetItemIterator it( parentListView );
	while ( it.current() ) {
        QTreeWidgetItem * item = it.current();
		item->setOpen( false );
		++it;
	}
}

void RecipeActionsHandler::exportRecipe( int id, const QString & caption, const QString &selection, RecipeDB *db )
{
	QList<int> ids;
	ids.append( id );

	exportRecipes( ids, caption, selection, db );
}

void RecipeActionsHandler::exportRecipes( const QList<int> &ids, const QString & caption, const QString &selection, RecipeDB *database )
{
	KFileDialog * fd = new KFileDialog( QUrl(),
		QString( "*.kre|%1 (*.kre)\n"
		"*.kreml|Krecipes (*.kreml)\n"
		"*.txt|%3 (*.txt)\n"
		//"*.cml|CookML (*.cml)\n"
		"*|%4\n"
		"*.html|%2 (*.html)\n"
		"*.mmf|Meal-Master (*.mmf)\n"
		"*.xml|RecipeML (*.xml)\n"
		"*.mx2|MasterCook (*.mx2)\n"
		"*.rk|Rezkonv (*.rk)"
		).arg( i18n( "Compressed Krecipes format" ) )
		.arg( i18n( "Web page" ) )
		.arg( i18n("Plain Text") )
		.arg( i18n("Web Book") ),
	0 );
	fd->setObjectName( "export_dlg" );
	fd->setModal( true );
	fd->setWindowTitle( caption );
	fd->setOperationMode( KFileDialog::Saving );
	fd->setSelection( selection );
	fd->setMode( KFile::File | KFile::Directory );
	if ( fd->exec() == KFileDialog::Accepted ) {
		QString fileName = fd->selectedFile();
		if ( !fileName.isEmpty() ) {
			BaseExporter * exporter;
			if ( fd->currentFilter() == "*.xml" )
				exporter = new RecipeMLExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.mx2" )
			    exporter = new MX2Exporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.mmf" )
				exporter = new MMFExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*" ) {
				CategoryTree *cat_structure = new CategoryTree;
				database->loadCategories( cat_structure );
				exporter = new HTMLBookExporter( cat_structure, fd->baseUrl().path(), "*.html" );
			}
			else if ( fd->currentFilter() == "*.html" ) {
				exporter = new HTMLExporter( fileName, fd->currentFilter() );
				XSLTExporter exporter_junk( fileName, "*.html" ); // AGH, i don't get build systems...
			}
			else if ( fd->currentFilter() == "*.cml" )
				exporter = new CookMLExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.txt" )
				exporter = new PlainTextExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.rk" )
				exporter = new RezkonvExporter( fileName, fd->currentFilter() );
			else {
				CategoryTree *cat_structure = new CategoryTree;
				database->loadCategories( cat_structure );
				exporter = new KreExporter( cat_structure, fileName, fd->currentFilter() );
			}

			int overwrite = -1;
			if ( QFile::exists( exporter->fileName() ) ) {
				overwrite = KMessageBox::warningYesNo( 0, i18n( "File \"%1\" exists.  Are you sure you want to overwrite it?" , exporter->fileName()), i18nc( "@title:window", "Saving recipe" ) );
			}

			if ( overwrite == KMessageBox::Yes || overwrite == -1 ) {
				KProgressDialog progress_dialog( 0, QString(), i18nc( "@info:progress", "Saving recipes..." ) );
				progress_dialog.setObjectName("export_progress_dialog");
				exporter->exporter( ids, database, &progress_dialog );
			}
			delete exporter;
		}
	}
	delete fd;
}

void RecipeActionsHandler::printRecipes( const QList<int> &ids, RecipeDB *database )
{
	//Create the temporary directory.
	m_tempdir = new QTemporaryDir(QDir::tempPath() + QLatin1Char('/') +  "krecipes-data-print"));
	QString tmp_filename = m_tempdir->path() + "krecipes_recipe_view.html";
	//Export to HTML in the temporary directory.
	XSLTExporter html_generator( tmp_filename, "html" );
	KConfigGroup config(KSharedConfig::openConfig(), "Page Setup" );
	QString styleFile = config.readEntry( "PrintLayout", QStandardPaths::locate(QStandardPaths::DataLocation, "layouts/None.klo" ) );
	if ( !styleFile.isEmpty() && QFile::exists( styleFile ) )
		html_generator.setStyle( styleFile );

	QString templateFile = config.readEntry( "PrintTemplate", QStandardPaths::locate(QStandardPaths::DataLocation, "layouts/Default.xsl" ) );
	if ( !templateFile.isEmpty() && QFile::exists( templateFile ) )
		html_generator.setTemplate( templateFile );
	html_generator.exporter( ids, database );
	//Load the generated HTML. When loaded, RecipeActionsHandlerView::print(...) will be called.
    m_printPage = new QWebEnginePage;
	connect(m_printPage, SIGNAL(loadFinished(bool)), SLOT(print(bool)));
    m_printPage->load( QUrl::fromLocalFile(tmp_filename) );
}

void RecipeActionsHandler::print(bool ok)
{
	Q_UNUSED(ok)
	QPrinter printer;
	QPointer<QPrintPreviewDialog> previewdlg = new QPrintPreviewDialog(&printer);
	//Show the print preview dialog.
	connect(previewdlg, SIGNAL(paintRequested(QPrinter *)),
        m_printPage, SLOT(print(QPrinter *)));
	previewdlg->exec();
	delete previewdlg;
	//Remove the temporary directory which stores the HTML and free memory.
	m_tempdir->remove();
	delete m_tempdir;
	emit( printDone() );
}

void RecipeActionsHandler::printDoneSlot()
{
	delete m_printPage;
}

void RecipeActionsHandler::recipesToClipboard( const QList<int> &ids, RecipeDB *db )
{
	KConfigGroup config = KSharedConfig::openConfig()->group("Export");
	QString formatFilter = config.readEntry("ClipboardFormat");

	BaseExporter * exporter;
	if ( formatFilter == "*.xml" )
		exporter = new RecipeMLExporter( QString(), formatFilter );
	else if ( formatFilter == "*.mx2" )
		exporter = new MX2Exporter( QString(), formatFilter );
	else if ( formatFilter == "*.mmf" )
		exporter = new MMFExporter( QString(), formatFilter );
	else if ( formatFilter == "*.cml" )
		exporter = new CookMLExporter( QString(), formatFilter );
	else if ( formatFilter == "*.rk" )
		exporter = new RezkonvExporter( QString(), formatFilter );
	else if ( formatFilter == "*.kre" || formatFilter == "*.kreml" ) {
		CategoryTree *cat_structure = new CategoryTree;
		db->loadCategories( cat_structure );
		exporter = new KreExporter( cat_structure, QString::null, formatFilter );
	}
	else //default to plain text
		exporter = new PlainTextExporter( QString(), "*.txt" );

	RecipeList recipeList;
	db->loadRecipes( &recipeList, exporter->supportedItems(), ids );

	QString buffer;
	QTextStream stream(&buffer,QIODevice::WriteOnly);
	exporter->writeStream(stream,recipeList);

	delete exporter;

	QApplication::clipboard()->setText(buffer);
}

void RecipeActionsHandler::recipesToClipboard()
{
    QList<QTreeWidgetItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QList<int> ids = recipeIDs( items );

		recipesToClipboard(ids,database);
	}
}

QList<int> RecipeActionsHandler::getAllVisibleItems()
{
	QList<int> ids;

    QTreeWidgetItemIterator iterator( parentListView );
	while ( iterator.current() ) {
		if ( iterator.current() ->isVisible() ) {
			if ( iterator.current() ->rtti() == RECIPELISTITEM_RTTI ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
				int recipe_id = recipe_it->recipeID();
	
				if ( ids.indexOf( recipe_id ) == -1 )
					ids.append( recipe_id );
			}
			//it is a category item and isn't populated, so get the unpopulated data from the database
			else if ( iterator.current()->rtti() == CATEGORYLISTITEM_RTTI && !iterator.current()->firstChild() ) {
				int cat_id = (( CategoryListItem* ) iterator.current())->element().id;
				ElementList list;
				database->loadRecipeList( &list, cat_id, true );
		
				for ( ElementList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it ) {
					if ( ids.indexOf( (*it).id ) == -1 )
						ids << (*it).id;
				}
			}
		}

		++iterator;
	}

	return ids;
}

QList<int> RecipeActionsHandler::recipeIDs() const
{
    const QList<QTreeWidgetItem*> items = parentListView->selectedItems();
	QList<int> ids = recipeIDs(items);
	return ids;
}

void RecipeActionsHandler::selectionChangedSlot()
{
    const QList<QTreeWidgetItem*> items = parentListView->selectedItems();
    if ( (items.count() == 1) && (items.first()->rtti() == 1000) ) {
		// We have a single recipe as our selection
		RecipeListItem * recipe_it = ( RecipeListItem* ) items.first();
		emit recipeSelected( recipe_it->recipeID(), 4 );
		emit recipeSelected( true );
	}
	else {
		emit recipeSelected( 0, 5 ); //id doesn't matter here
		emit recipeSelected( false );
	}
}


