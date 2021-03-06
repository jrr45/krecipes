/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientsdialog.h"

//FIXME: Check which ones are actually needed.
#include "backends/recipedb.h"
#include "createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "widgets/kreingredientlistwidget.h"
#include "dialogs/ingredientgroupsdialog.h"
#include "dialogs/editpropertiesdialog.h"
#include "widgets/kregenericlistwidget.h"
#include "actionshandlers/kreingredientactionshandler.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include <QTabWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPointer>

IngredientsDialog::IngredientsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{
	// Store pointer to database
	database = db;

	// Design dialog

	QHBoxLayout* page_layout = new QHBoxLayout( this );

	tabWidget = new QTabWidget( this );

	ingredientTab = new QWidget( tabWidget );

	layout = new QHBoxLayout( ingredientTab );

	listLayout = new QVBoxLayout;

	ingredientListWidget = new KreIngredientListWidget( ingredientTab, database );
	ingredientActionsHandler = new KreIngredientActionsHandler( ingredientListWidget, database );
	listLayout->addWidget( ingredientListWidget );

	QVBoxLayout *buttonLayout = new QVBoxLayout();

	addIngredientButton = new QPushButton( ingredientTab );
	addIngredientButton->setText( i18nc( "@action:button", "Create..." ) );
	addIngredientButton->setIcon( QIcon::fromTheme(QStringLiteral("list-add")) );
	buttonLayout->addWidget( addIngredientButton );

	removeIngredientButton = new QPushButton( ingredientTab );
	removeIngredientButton->setText( i18nc( "@action:button", "Delete" ) );
	removeIngredientButton->setIcon( QIcon::fromTheme(QStringLiteral("list-remove")) );
	buttonLayout->addWidget( removeIngredientButton );

	buttonLayout->addStretch();
	
	QPushButton *propertyButton = new QPushButton( i18nc("@action:button", "Property Information"), ingredientTab );
	propertyButton->setIcon( QIcon::fromTheme(QStringLiteral("document-properties")) );
	listLayout->addWidget( propertyButton );

	layout->addLayout( listLayout );
	layout->addLayout( buttonLayout );

	tabWidget->insertTab( -1, ingredientTab, i18nc( "@title:tab", "Ingredients" ) );

	groupsDialog = new IngredientGroupsDialog(database,tabWidget,"groupsDialog");
	tabWidget->insertTab( -1, groupsDialog, i18nc( "@title:tab", "Headers" ) );

	page_layout->addWidget( tabWidget );

	// Signals & Slots
	connect( addIngredientButton, SIGNAL( clicked() ), ingredientActionsHandler, SLOT( createNew() ) );
	connect( removeIngredientButton, SIGNAL( clicked() ), ingredientActionsHandler, SLOT( remove() ) );
	connect( propertyButton, SIGNAL( clicked() ), this, SLOT( showPropertyEdit() ) );

}


IngredientsDialog::~IngredientsDialog()
{
}

void IngredientsDialog::reloadIngredientList( ReloadFlags flag )
{
	ingredientListWidget->reload( flag );
}

void IngredientsDialog::reload( ReloadFlags flag )
{
	reloadIngredientList( flag );
	groupsDialog->reload( flag );
}

KreGenericActionsHandler * IngredientsDialog::getActionsHandler() const
{
	if ( tabWidget->currentWidget() == ingredientTab )
		return ingredientActionsHandler;
	else //if ( tabWidget->currentWidget() == groupsDialog )
		return groupsDialog->getActionsHandler();
}

void IngredientsDialog::addAction( QAction * action )
{
	ingredientActionsHandler->addAction( action );
	groupsDialog->addAction( action );
}

void IngredientsDialog::showPropertyEdit()
{
	int id = ingredientListWidget->selectedRowId();
	QString ingredientName = ingredientListWidget->getData(
		ingredientListWidget->currentRow(), 1 ).toString();
	if (id != -1 ) {
		QPointer<EditPropertiesDialog> d = new EditPropertiesDialog( id, ingredientName, database, this );
		d->exec();
		delete d;
	} else {
		KMessageBox::information( this, i18nc( "@info", "No ingredient selected." ), QString() );
	}
}

