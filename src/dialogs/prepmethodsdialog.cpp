/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "prepmethodsdialog.h"

#include <QDialog>
#include <KLocale>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <KConfigGroup>

#include "widgets/kreprepmethodlistwidget.h"
#include "actionshandlers/kreprepmethodactionshandler.h"
#include "actionshandlers/kregenericactionshandler.h"


PrepMethodsDialog::PrepMethodsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	QHBoxLayout* layout = new QHBoxLayout( this );

	//PrepMethod List
	prepMethodListWidget = new KrePrepMethodListWidget( this, database );
	prepMethodActionsHandler = new KrePrepMethodActionsHandler( prepMethodListWidget, database );
	layout->addWidget( prepMethodListWidget );

	//Buttons
	QVBoxLayout* vboxl = new QVBoxLayout(); 
//TODO PORT QT5 	vboxl->setSpacing ( QDialog::spacingHint() );

	newPrepMethodButton = new QPushButton( this );
	newPrepMethodButton->setText( i18nc( "@action:button", "Create..." ) );
	newPrepMethodButton->setIcon( QIcon::fromTheme(QStringLiteral("list-add")) );
	vboxl->addWidget( newPrepMethodButton );

	removePrepMethodButton = new QPushButton( this );
	removePrepMethodButton->setText( i18nc( "@action:button", "Delete" ) );
	removePrepMethodButton->setIcon( QIcon::fromTheme(QStringLiteral("list-remove")) );
	vboxl->addWidget( removePrepMethodButton );
	vboxl->addStretch();

	layout->addLayout( vboxl );

	//Connect Signals & Slots

	connect ( newPrepMethodButton, SIGNAL( clicked() ),
		prepMethodActionsHandler, SLOT( createNew() ) );
	connect ( removePrepMethodButton, SIGNAL( clicked() ),
		prepMethodActionsHandler, SLOT( remove() ) );
}

PrepMethodsDialog::~PrepMethodsDialog()
{}

// (Re)loads the data from the database
void PrepMethodsDialog::reload( ReloadFlags flag )
{
	prepMethodListWidget->reload( flag );
}

KreGenericActionsHandler* PrepMethodsDialog::getActionsHandler() const
{
	return prepMethodActionsHandler;
}

void PrepMethodsDialog::addAction( QAction * action )
{
	prepMethodActionsHandler->addAction( action );
}

