/*****************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                           *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>                 *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "categorieseditordialog.h"

#include <kdebug.h>
#include <QDialog>
#include <klocale.h>
#include <kmessagebox.h>

#include "widgets/krecategorieslistwidget.h"
#include "createcategorydialog.h"
#include "backends/recipedb.h"
#include "actionshandlers/krecategoryactionshandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <KConfigGroup>

CategoriesEditorDialog::CategoriesEditorDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	QHBoxLayout* layout = new QHBoxLayout( this );

	//Category List
	m_categoriesListWidget = new KreCategoriesListWidget( this, database );
	categoryActionsHandler = new KreCategoryActionsHandler( m_categoriesListWidget, database );
	layout->addWidget( m_categoriesListWidget );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout();
//TODO PORT QT5 	vboxl->setSpacing( QDialog::spacingHint() );
	newCategoryButton = new QPushButton( this );
	newCategoryButton->setText( i18nc( "@action:button", "Create..." ) );
	newCategoryButton->setIcon( QIcon::fromTheme(QStringLiteral("list-add")) );
	vboxl->addWidget( newCategoryButton );

	removeCategoryButton = new QPushButton( this );
	removeCategoryButton->setText( i18nc( "@action:button", "Delete" ) );
	removeCategoryButton->setIcon( QIcon::fromTheme(QStringLiteral("list-remove")) );
	vboxl->addWidget( removeCategoryButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newCategoryButton, SIGNAL( clicked() ), categoryActionsHandler, SLOT( createNew() ) );
	connect ( removeCategoryButton, SIGNAL( clicked() ), categoryActionsHandler, SLOT( remove() ) );
}

CategoriesEditorDialog::~CategoriesEditorDialog()
{}

void CategoriesEditorDialog::reload( ReloadFlags flag )
{
	m_categoriesListWidget->reload( flag );
}

KreCategoryActionsHandler* CategoriesEditorDialog::getActionsHandler () const
{
	return categoryActionsHandler;
}

void CategoriesEditorDialog::addAction( QAction * action )
{
	categoryActionsHandler->addAction( action );
}

void CategoriesEditorDialog::setCategoryPasteAction( QAction * action )
{
	categoryActionsHandler->setCategoryPasteAction( action );
}

void CategoriesEditorDialog::setPasteAsSubcategoryAction( QAction * action )
{
	categoryActionsHandler->setPasteAsSubcategoryAction( action );
}

