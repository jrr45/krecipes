/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "conversiondialog.h"

#include <QPushButton>
#include <QLabel>

#include <KVBox>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kvbox.h>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "backends/recipedb.h"
#include "widgets/unitcombobox.h"
#include "widgets/ingredientcombobox.h"
#include "widgets/prepmethodcombobox.h"
#include "widgets/fractioninput.h"

ConversionDialog::ConversionDialog( QWidget* parent, RecipeDB *db, const char* name )
		: QDialog( parent ),
		m_database(db)
{
	 setObjectName( name );
	 setWindowTitle(i18nc( "@title:window", "Measurement Converter" ));
	 QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help|QDialogButtonBox::Close);
	 QWidget *mainWidget = new QWidget(this);
	 QVBoxLayout *mainLayout = new QVBoxLayout;
	 setLayout(mainLayout);
	 mainLayout->addWidget(mainWidget);
	 QPushButton *user1Button = new QPushButton;
	 buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
	 connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	 connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	 //PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
	 mainLayout->addWidget(buttonBox);
	 buttonBox->button(QDialogButtonBox::Close)->setDefault(true);
	 setHelp("measure-converter");
	 user1Button->setText(i18nc("@action:button"));

	 setModal( false );

	 KVBox *page = new KVBox( this );
//PORTING: Verify that widget was added to mainLayout: 	 setMainWidget( page );
// Add mainLayout->addWidget(page); if necessary

	KHBox *vbox = new KVBox(page);

	KHBox *fromTopBox = new KHBox(vbox);
	convertLabel = new QLabel( fromTopBox );

	amountEdit = new FractionInput( fromTopBox );

	fromUnitBox = new UnitComboBox( fromTopBox, db );
	fromUnitBox->reload();
	fromTopBox->setStretchFactor( fromUnitBox, 2 );
	fromTopBox->setSpacing(3);

	KHBox *fromBottomBox = new KHBox(vbox);

	ingredientBox = new IngredientComboBox( false, fromBottomBox, db, i18nc( "@item:inlistbox", "--Ingredient (optional)--" ) );
	ingredientBox->reload();

	prepMethodBox = new PrepMethodComboBox( false, fromBottomBox, db, i18nc( "@item:inlistbox", "-No Preparation-" ) );
	prepMethodBox->reload();
	fromBottomBox->setSpacing(3);

	KHBox *toBox = new KHBox(vbox);

	toLabel = new QLabel( toBox );

	toUnitBox = new UnitComboBox( toBox, db );
	toUnitBox->reload();
	toBox->setStretchFactor( toUnitBox, 2 );
	toBox->setSpacing(8);

	KHBox *resultBox = new KHBox(vbox);
	resultLabel = new QLabel( resultBox );
	resultLabel->setObjectName( "resultLabel" );
	resultText = new QLabel( resultBox );
	resultText->setObjectName( "resultText" );
	resultBox->setStretchFactor( resultText, 2 );

	languageChange();

	resize( QSize(300, 200).expandedTo(minimumSizeHint()) );

	// signals and slots connections
	connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked() ), this, SLOT( accept() ) );
	connect(user1Button, SIGNAL( clicked() ), this, SLOT( convert() ) );
}

ConversionDialog::~ConversionDialog()
{
}

void ConversionDialog::languageChange()
{
	convertLabel->setText( i18nc( "@label", "Convert" ) );
	toLabel->setText( i18nc( "@label Convert to", "To" ) );
	resultLabel->setText( i18nc( "@label Result of an unit conversion",
		"<b>Result:</b>" ) );
	resultText->setText( QString() );
}

void ConversionDialog::show()
{
	reset();
	QDialog::show();
}

void ConversionDialog::reset()
{
	resultText->setText( QString() );
	ingredientBox->setCurrentItem( 0 );
	prepMethodBox->setCurrentItem( 0 );
	toUnitBox->setCurrentItem( 0 );
	fromUnitBox->setCurrentItem( 0 );
	amountEdit->clear();
}

void ConversionDialog::convert()
{
	Ingredient result, ing;
	Unit unit = m_database->unitName(toUnitBox->id(toUnitBox->currentIndex()));

	ing.amount = amountEdit->value().toDouble();
	ing.ingredientID = ingredientBox->id(ingredientBox->currentIndex());
	ing.units = m_database->unitName(fromUnitBox->id(fromUnitBox->currentIndex()));

	int prepID = prepMethodBox->id(prepMethodBox->currentIndex());
	if ( prepID != -1 )
		ing.prepMethodList.append(Element(QString(),prepID));

	switch ( m_database->convertIngredientUnits( ing, unit, result ) ) {
	case RecipeDB::Success:
		resultLabel->setText( i18nc( "@label:textbox", "<b>Result:</b>" ) );
		resultText->setText(QString::number(result.amount)+' '+result.units.determineName(result.amount, /*useAbbrev=*/false));
		break;
	case RecipeDB::MismatchedPrepMethodUsingApprox:
		resultLabel->setText( i18nc( "@label:textbox",  "<b>Approximated result:</b>" ) );
		resultText->setText(QString::number(result.amount)+' '+result.units.determineName(result.amount, /*useAbbrev=*/false));
		break;
	case RecipeDB::MissingUnitConversion:
		resultLabel->setText( i18nc( "@label:textbox", "<b>Error:</b>" ) );
		resultText->setText( i18nc( "@info", "Missing unit conversion" ) );
		break;
	case RecipeDB::MissingIngredientWeight:
		resultLabel->setText( i18nc( "@label:textbox", "<b>Error:</b>" ) );
		resultText->setText( i18nc( "@info", "No ingredient weight available" ) );
		break;
	case RecipeDB::MismatchedPrepMethod:
		resultLabel->setText( i18nc( "@label:textbox", "<b>Error:</b>" ) );
		resultText->setText( i18nc( "@info", "No ingredient weight available for this method of preparation" ) );
		break;
	case RecipeDB::MissingIngredient:
		resultLabel->setText( i18nc( "@label:textbox", "<b>Error:</b>" ) );
		resultText->setText( i18nc( "@info", "Ingredient required for conversion" ) );
		break;
	case RecipeDB::InvalidTypes:
		resultLabel->setText( i18nc( "@label:textbox", "<b>Error:</b>" ) );
		resultText->setText( i18nc( "@info", "Impossible unit conversion based on unit types" ) );
		break;
	}
}

#include "conversiondialog.moc"
