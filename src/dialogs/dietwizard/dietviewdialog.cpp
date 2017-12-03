/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dietviewdialog.h"

#include <KHTMLView>
#include <KLocale>
#include <KStandardDirs>
#include <KStandardGuiItem>
#include <KVBox>

#include <QDir>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

DietViewDialog::DietViewDialog( QWidget *parent, const RecipeList &recipeList, int dayNumber, int mealNumber, const QList <int> &dishNumbers )
		: QDialog( parent )
{
	setWindowTitle( i18nc( "@title:window", "View Diet" ) );
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	setLayout(mainLayout);
	mainLayout->addWidget(mainWidget);
	QPushButton *user1Button = new QPushButton;
	buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
	QPushButton *user2Button = new QPushButton;
	buttonBox->addButton(user2Button, QDialogButtonBox::ActionRole);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	//PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
	mainLayout->addWidget(buttonBox);
	setDefaultButton(QDialog::User2);
	setModal( false );
	KVBox *page = new KVBox( this );
	mainLayout->addWidget(page);
	user1Button->setText(KStandardGuiItem::print(.text( ));
	setButtonIcon( QDialog::User1, KIcon( "document-print" ) );

	user2Button->setText(i18nc( "@action:button"));
	setButtonIcon( QDialog::User2, KIcon( "view-pim-tasks" ) );

	// The html part
	dietView = new KHTMLPart( page );
	mainLayout->addWidget(dietView);

	resize( QSize(350, 450) );

	setSizeGripEnabled( true );

	connect(user2Button, SIGNAL(clicked() ), this, SLOT( slotOk() ) );
	connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked() ), this, SLOT( close() ) );
	connect(user1Button, SIGNAL( clicked() ), this, SLOT( print() ) );

	// Show the diet
	showDiet( recipeList, dayNumber, mealNumber, dishNumbers );
}

DietViewDialog::~DietViewDialog()
{}

void DietViewDialog::showDiet( const RecipeList &recipeList, int dayNumber, int mealNumber, const QList <int> &dishNumbers )
{


	// Header
	QString htmlCode = QString( "<html><head><title>%1</title>" ).arg( i18n( "Diet" ) );

	// CSS
	htmlCode += "<STYLE type=\"text/css\">\n";
	htmlCode += "#calendar{border: thin solid black}";
	htmlCode += ".dayheader{ background-color: #D6D6D6; color: black; border:none;}";
	htmlCode += ".day{ background-color: #E5E5E5; color: black; border:medium solid #D6D6D6;}";
	htmlCode += ".meal{ background-color: #CDD4FF; color: black; border:thin solid #B4BEFF; text-align:center;}";
	htmlCode += ".dish{font-size: smaller; overflow: hidden; height:2.5em;}";
	htmlCode += "</STYLE>";


	htmlCode += "</head><body>"; //  /Header

	// Calendar border
	htmlCode += QString( "<div id=\"calendar\">" );

	// Title
	htmlCode += QString( "<center><div STYLE=\"width: 100%\">" );
	htmlCode += QString( "<h1>%1</h1></div></center>" ).arg( i18n( "Diet" ) );

	// Diet table
	htmlCode += QString( "<center><div STYLE=\"width: 98%\">" );
	htmlCode += QString( "<table><tbody>" );


	QList <int>::ConstIterator it = dishNumbers.begin();
	RecipeList::ConstIterator rit;
	rit = recipeList.begin();

	for ( int row = 0, day = 0; row <= ( ( dayNumber - 1 ) / 7 ); row++ )  // New row (week)
	{
		htmlCode += QString( "<tr>" );

		for ( int col = 0; ( col < 7 ) && ( day < dayNumber ); col++, day++ )  // New column (day)
		{
			htmlCode += QString( "<td><div class=\"day\">" );
			htmlCode += QString( "<div class=\"dayheader\"><center>" );
			htmlCode += i18n( "Day %1", day + 1 );
			htmlCode += QString( "</center></div>" );
			for ( int meal = 0;meal < mealNumber;meal++ )  // Meals in each cell
			{
				int dishNumber = *it;
				htmlCode += QString( "<div class=\"meal\">" );
				for ( int dish = 0; dish < dishNumber;dish++ )  // Dishes in each Meal
				{
					htmlCode += QString( "<div class=\"dish\">" );
					htmlCode += ( *rit ).title;
					htmlCode += "<br>";
					htmlCode += QString( "</div>" );
					rit++;
				}
				it++;
				htmlCode += QString( "</div>" );
			}
			it = dishNumbers.begin(); // meals have same dish number everyday
			htmlCode += QString( "</div></td>" );
		}

		htmlCode += QString( "</tr>" );
	}

	htmlCode += QString( "</tbody></table>" );
	htmlCode += QString( "</div></center>" );
	htmlCode += QString( "</div></body></html>" );

	resize( QSize( 600, 400 ) );

	// Display it
    dietView->begin( QUrl::fromLocalFile( QDir::tempPath() ) ); // Initialize to tmp dir, where photos and logos can be stored
	dietView->write( htmlCode );
	dietView->end();
}

void DietViewDialog::print( void )
{
	dietView->view()->print();
}

void DietViewDialog::slotOk( void )
{
	emit signalOk();
	close();
}

#include "dietviewdialog.moc"
