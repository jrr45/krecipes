/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createunitdialog.h"

#include <QFormLayout>
#include <QGroupBox>

#include <klocale.h>
#include <QLineEdit>
#include <kcombobox.h>
#include <KVBox>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

CreateUnitDialog::CreateUnitDialog( QWidget *parent, const QString &name, const QString &plural,
	const QString &name_abbrev, const QString &plural_abbrev, Unit::Type type, bool newUnit )
		: QDialog( parent )
{
	setWindowTitle(newUnit?i18nc( "@title:window", "New Unit" ):i18nc( "@title:window", "Unit" ));
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

	QGroupBox * box = new QGroupBox;
	QFormLayout * layout = new QFormLayout;
	box->setLayout( layout );
//PORTING: Verify that widget was added to mainLayout: 	setMainWidget( box );
// Add mainLayout->addWidget(box); if necessary

	box->setTitle( (newUnit)?i18nc( "@title:group", "New Unit" ):i18nc("@title:group", "Unit") );
	
	nameEdit = new QLineEdit( name, box );
	layout->addRow( i18nc("@label:textbox Single unit name",
		"Singular:" ), nameEdit );
	
	nameAbbrevEdit = new QLineEdit( name_abbrev, box );
	layout->addRow( i18nc( "@label:textbox Single unit abbreviation",
		"Singular Abbreviation:" ), nameAbbrevEdit );

	pluralEdit = new QLineEdit( plural, box );
	layout->addRow( i18nc("@label:textbox Plural unit name",
		"Plural:" ), pluralEdit );

	pluralAbbrevEdit = new QLineEdit( plural_abbrev, box );
	layout->addRow( i18nc( "@label:textbox Plural unit abbreviation",
		"Plural Abbreviation:" ), pluralAbbrevEdit );

	typeComboBox = new KComboBox( false, box );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type other", "Other") );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type mass", "Mass") );
	typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type volume", "Volume") );

	typeComboBox->setCurrentIndex( type );

	layout->addRow( i18nc("@label:textbox Unit Type", "Type:" ), typeComboBox );

	adjustSize();
	resize( 400, size().height() );
	setFixedHeight( size().height() );

	connect( nameAbbrevEdit, SIGNAL(textChanged(const QString&)), SLOT(nameAbbrevTextChanged(const QString &)) );

	if ( name.isEmpty() )
		nameEdit->setFocus();
	else if ( plural.isEmpty() )
		pluralEdit->setFocus();
}


CreateUnitDialog::~CreateUnitDialog()
{}

Unit CreateUnitDialog::newUnit( void )
{
	QString name = nameEdit->text();
	QString plural = pluralEdit->text();

	if ( name.isEmpty() )
		name = plural;
	if ( plural.isEmpty() )
		plural = name;

	Unit new_unit = Unit( name, plural );
	new_unit.setNameAbbrev(nameAbbrevEdit->text());
	new_unit.setPluralAbbrev(pluralAbbrevEdit->text());

	new_unit.setType((Unit::Type)typeComboBox->currentIndex());

	return new_unit;
}

void CreateUnitDialog::nameAbbrevTextChanged(const QString &newText)
{
	//appending
	if ( newText.left( newText.length()-1 ) == pluralAbbrevEdit->text() ) {
		pluralAbbrevEdit->setText( newText );
	}

	//truncating
	if ( newText.left( newText.length()-1 ) == pluralAbbrevEdit->text().left( newText.length()-1 ) ) {
		pluralAbbrevEdit->setText( newText );
	}
}

