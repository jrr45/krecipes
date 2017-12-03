/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeviewdialog.h"

#include <QFile>
//Added by qt3to4:

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <khtmlview.h>
#include <khtml_part.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kprogressdialog.h>

#include <kstatusbar.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kvbox.h>
#include <KTempDir>
#include <QFrame>
#include <QToolButton>
#include <QHBoxLayout>
#include <QTextDocument>

#include "datablocks/mixednumber.h"
#include "backends/recipedb.h"
#include "exporters/xsltexporter.h"

RecipeViewDialog::RecipeViewDialog( QWidget *parent, RecipeDB *db, int recipeID ) : KVBox( parent ),
	database(db)
{
	// Initialize UI Elements
	khtmlpartBox = new KVBox( this );
	recipeView = new KHTMLPart( khtmlpartBox );
	recipeView->setJScriptEnabled(false);
	recipeView->setJavaEnabled(false);
	recipeView->setMetaRefreshEnabled(false);
	recipeView->setPluginsEnabled(false);
	recipeView->setOnlyLocalReferences(true);

	connect( database, SIGNAL(recipeRemoved(int)), SLOT(recipeRemoved(int)) );

	m_tempdir = new KTempDir(QDir::tempPath() + QLatin1Char('/') +  "krecipes-data-view"));
	tmp_filename = m_tempdir->name() + "krecipes_recipe_view.html";

	// Functions Box
	QHBoxLayout* functionsLayout = new QHBoxLayout;

	functionsBox = new QFrame ( this );
	//functionsBox->setFrameStyle( QFrame::NoFrame );
	functionsBox->setLayout( functionsLayout );

	closeButton = new QToolButton;
	closeButton->setIcon( QIcon::fromTheme(QStringLiteral("window-close")) );

	closeButton->setText( i18nc( "@action:button", "Close" ) );
	closeButton->setToolTip( i18nc( "@info:tooltip", "Close" ) );
	closeButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

	editButton = new QToolButton;
	editButton->setIcon( QIcon::fromTheme(QStringLiteral("document-edit")) );

	editButton->setText( i18nc( "@action:button", "Edit Recipe" ) );
	editButton->setToolTip( i18nc( "@info:tooltip", "Edit Recipe" ) );
	editButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

	functionsLayout->layout()->addItem( new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
	functionsLayout->addWidget( editButton );
	functionsLayout->addWidget( closeButton );

	//Connect the signals.
	connect ( editButton, SIGNAL( clicked() ), this, SIGNAL( editRecipe() ) );
	connect ( closeButton, SIGNAL( clicked() ), this, SIGNAL( closeRecipeView() ) );
	
	//----------Load  the recipe --------
	if ( recipeID != -1 )
		loadRecipe( recipeID );
}

RecipeViewDialog::~RecipeViewDialog()
{
	delete m_tempdir;
}

bool RecipeViewDialog::loadRecipe( int recipeID )
{
	QList<int> ids;
	ids.append( recipeID );
	return loadRecipes( ids );
}

bool RecipeViewDialog::loadRecipes( const QList<int> &ids )
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	// Remove any files created by the last recipe loaded
	delete m_tempdir;
	m_tempdir = new KTempDir(QDir::tempPath() + QLatin1Char('/') +  "krecipes-data-view"));
	tmp_filename = m_tempdir->name() + "krecipes_recipe_view.html";

	ids_loaded = ids; //need to save these ids in order to delete the html files later...make sure this comes after the call to removeOldFiles()
	recipe_loaded = ( ids.count() > 0 && ids[ 0 ] >= 0 );

	bool success = showRecipes( ids );

	KApplication::restoreOverrideCursor();
	return success;
}

bool RecipeViewDialog::showRecipes( const QList<int> &ids )
{
	KProgressDialog * progress_dialog = 0;

	if ( ids.count() > 1 )  //we don't want a progress bar coming up when there is only one recipe... it may show up during the splash screen
	{
		progress_dialog = new KProgressDialog( this, QString(), i18nc( "@title:window", "Loading recipes, please wait..." ) );
		progress_dialog->setObjectName( "open_progress_dialog" );
		progress_dialog->setModal( true );
		progress_dialog->resize( 240, 80 );
		//Hide the edit button when more than one recipe is displayed
		editButton->setVisible( false );
	} else {
		//If it's just one recipe, show the edit button
		editButton->setVisible( true );
	}

	XSLTExporter html_generator( tmp_filename, "html" );

	html_generator.exporter( ids, database, progress_dialog ); //writes the generated HTML to 'tmp_filename'
	if ( progress_dialog && progress_dialog->wasCancelled() ) {
		delete progress_dialog;
		return false;
	}

    QUrl url;
    url.fromLocalFile( tmp_filename );
	recipeView->openUrl( url );
	recipeView->show();
    kDebug() << "Opening URL: " << Qt::escape( QUrl::toDisplayString(url) ) ;

	delete progress_dialog;
	return true;
}

void RecipeViewDialog::reload()
{
	loadRecipes( ids_loaded );
}

void RecipeViewDialog::showButtons()
{
	functionsBox->setVisible( true );
}

void RecipeViewDialog::hideButtons()
{
	functionsBox->setVisible( false );
}

void RecipeViewDialog::recipeRemoved( int id )
{
	//if the deleted recipe is loaded, clean the view up
	if ( ids_loaded.indexOf(id) != -1 ) {
		ids_loaded.removeAll(id);
		reload();
	}
}

#include "recipeviewdialog.moc"
