/*****************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                           *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>                 *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2008 Montel Laurent <montel@kde.org>                         *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "authorsdialog.h"
#include "createelementdialog.h"
#include "backends/recipedb.h"
#include "widgets/kreauthorlistwidget.h"
#include "actionshandlers/kreauthoractionshandler.h"

#include <QDialog>
#include <klocale.h>
#include <kmessagebox.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <KConfigGroup>

AuthorsDialog::AuthorsDialog( QWidget* parent, RecipeDB *db )
	 : QWidget( parent )
{

	// Store pointer to database
	database = db;
	QHBoxLayout* layout = new QHBoxLayout( this );

	//Author List
	authorListWidget = new KreAuthorListWidget( this, database );
	authorActionsHandler = new KreAuthorActionsHandler( authorListWidget, database );
	layout->addWidget( authorListWidget );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout();
//TODO PORT QT5 	vboxl->setSpacing( QDialog::spacingHint() );

	newAuthorButton = new QPushButton( this );
	newAuthorButton->setText( i18nc( "@action:button", "Create..." ) );
	newAuthorButton->setIcon( QIcon::fromTheme(QStringLiteral("list-add")) );
	vboxl->addWidget( newAuthorButton );

	removeAuthorButton = new QPushButton( this );
	removeAuthorButton->setText( i18nc( "@action:button", "Delete" ) );
	removeAuthorButton->setIcon( QIcon::fromTheme(QStringLiteral("list-remove")) );
	vboxl->addWidget( removeAuthorButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newAuthorButton, SIGNAL( clicked() ), authorActionsHandler, SLOT( createNew() ) );
	connect ( removeAuthorButton, SIGNAL( clicked() ), authorActionsHandler, SLOT( remove() ) );
}

AuthorsDialog::~AuthorsDialog()
{}

// (Re)loads the data from the database
void AuthorsDialog::reload( ReloadFlags flag )
{
	authorListWidget->reload( flag );
}

KreGenericActionsHandler* AuthorsDialog::getActionsHandler() const
{
	return authorActionsHandler;
}

void AuthorsDialog::addAction( QAction * action )
{
	authorActionsHandler->addAction( action );
}

