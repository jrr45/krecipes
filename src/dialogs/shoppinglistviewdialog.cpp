/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "shoppinglistviewdialog.h"
#include "datablocks/ingredientlist.h"
#include "datablocks/mixednumber.h"

#include <QPushButton>

#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kvbox.h>
#include <KStandardGuiItem>
#include <q3tl.h>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <KGuiItem>
#include <QVBoxLayout>
#include <KSharedConfig>

ShoppingListViewDialog::ShoppingListViewDialog( QWidget *parent, const IngredientList &ingredientList )
		: QDialog( parent )
{
	this->setObjectName( "shoppingviewdialog" );
	this->setWindowTitle( i18nc( "@title:window", "Shopping List" ) );
	this->setModal( true );
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	this->setLayout(mainLayout);
	mainLayout->addWidget(mainWidget);
	QPushButton *user1Button = new QPushButton;
	buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
	this->connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	this->connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	//PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
	mainLayout->addWidget(buttonBox);
	KGuiItem::assign(user1Button, KStandardGuiItem::print( ));
	buttonBox->button(QDialogButtonBox::Close)->setDefault(true);
	
	// Design dialog
	KVBox *page = new KVBox ( this );
//PORTING: Verify that widget was added to mainLayout: 	setMainWidget( page );
// Add mainLayout->addWidget(page); if necessary

	shoppingListView = new KHTMLPart( page );

	resize( QSize(350, 450) );

	connect(user1Button, SIGNAL( clicked() ), this, SLOT( print() ) );
	connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked() ), this, SLOT( accept() ) );

	//---------- Sort the list --------
	IngredientList list_copy = ingredientList;
	qHeapSort( list_copy );

	//---------- Load  the list --------
	display( list_copy );
}


ShoppingListViewDialog::~ShoppingListViewDialog()
{}

void ShoppingListViewDialog::display( const IngredientList &ingredientList )
{
	QString recipeHTML;

	// Create HTML Code

	// Headers
	recipeHTML = QString( "<html><head><title>%1</title></head><body>" ).arg( i18n( "Shopping List" ) );
	recipeHTML += "<center><div STYLE=\"width: 95%\">";
	recipeHTML += QString( "<center><h1>%1</h1></center>" ).arg( i18n( "Shopping List" ) );

	// Ingredient List

	recipeHTML += "<div STYLE=\"border:medium solid blue; width:95%\"><table cellspacing=0px width=100%><tbody>";
	bool counter = true;

	KConfigGroup config = KSharedConfig::openConfig()->group( "Formatting" );

	bool useAbbreviations = config.readEntry("AbbreviateUnits", false );

	for ( IngredientList::const_iterator ing_it = ingredientList.begin(); ing_it != ingredientList.end(); ++ing_it ) {
		QString color = ( counter ) ? "#CBCEFF" : "#BFC2F0";
		counter = !counter;

		QString amount_str = MixedNumber( ( *ing_it ).amount ).toString( MixedNumber::AutoFormat );

		QString unit = ( *ing_it ).units.determineName( ( *ing_it ).amount + ( *ing_it ).amount_offset, useAbbreviations );

		recipeHTML += QString( "<tr bgcolor=\"%1\"><td>- %2:</td><td>%3 %4</td></tr>" ).arg( color ).arg( ( *ing_it ).name ).arg( amount_str ).arg( unit );
	}
	recipeHTML += "</tbody></table></div>";
	// Close
	recipeHTML += "</div></center></body></html>";


	// Display
    shoppingListView->begin( QUrl::fromLocalFile( "file:/tmp/" ) ); // Initialize to /tmp, where photos and logos are stored
	shoppingListView->write( recipeHTML );
	shoppingListView->end();


}

void ShoppingListViewDialog::print()
{
	shoppingListView->view() ->print();
}

