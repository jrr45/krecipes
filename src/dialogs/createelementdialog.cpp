/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createelementdialog.h"

#include <klocale.h>

CreateElementDialog::CreateElementDialog( QWidget *parent, const QString &text )
		: KDialogBase( parent, "createElementDialog", true, text,
		    KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok )
{
	QVBox *page = makeVBoxMainWidget();

	box = new QGroupBox( page );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
	boxLayout->setAlignment( Qt::AlignTop );
	box->setTitle( text );

	elementEdit = new KLineEdit( box );
	boxLayout->addWidget( elementEdit );

	adjustSize();
	setFixedSize( size() ); //we've got all the widgets put in, now let's keep it this size

	elementEdit->setFocus();
}


CreateElementDialog::~CreateElementDialog()
{}

QString CreateElementDialog::newElementName( void )
{
	return ( elementEdit->text() );
}

