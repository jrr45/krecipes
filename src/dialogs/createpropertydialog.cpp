/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createpropertydialog.h"

#include <klocale.h>
#include <QGroupBox>
#include <QFormLayout>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

CreatePropertyDialog::CreatePropertyDialog( QWidget *parent, UnitList* list )
		: QDialog( parent )
{
	setWindowTitle( i18nc( "@title:window", "New Property" ) );
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	setLayout(mainLayout);
	mainLayout->addWidget(mainWidget);
	QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
	okButton->setDefault(true);
	okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	//PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
	mainLayout->addWidget(buttonBox);
	buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
	setModal( true );
	// Initialize Internal Variables
	unitList = list; // Store the pointer to the unitList;

	// Initialize widgets
	QGroupBox * box = new QGroupBox;
//PORTING: Verify that widget was added to mainLayout: 	setMainWidget( box );
// Add mainLayout->addWidget(box); if necessary
	QFormLayout * layout = new QFormLayout;
	box->setLayout( layout );
	box->setTitle( i18nc( "@title:group", "New Property" ) );

	propertyNameEdit = new QLineEdit( box );
	propertyNameEdit->setMinimumWidth( 150 );
	layout->addRow( i18nc( "@label:textbox", "Property name:" ), propertyNameEdit );

	propertyUnits = new QLineEdit( box );
	propertyUnits->setMinimumWidth( 150 );
	layout->addRow( i18nc( "@label:textbox", "Units:" ), propertyUnits );

	adjustSize();
	resize( 400, size().height() );
	setFixedHeight( size().height() );

	propertyNameEdit->setFocus();
}


CreatePropertyDialog::~CreatePropertyDialog()
{}


QString CreatePropertyDialog::newPropertyName( void )
{
	return ( propertyNameEdit->text() );
}

QString CreatePropertyDialog::newUnitsName( void )
{
	return ( propertyUnits->text() );
}

