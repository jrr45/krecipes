/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "selectunitdialog.h"

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kvbox.h>
#include <QVBoxLayout>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <KSharedConfig>

SelectUnitDialog::SelectUnitDialog( QWidget* parent, const UnitList &unitList, OptionFlag showEmpty )
		: QDialog( parent), m_showEmpty(showEmpty)
{
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
	setWindowTitle(i18nc( "@title:window", "Choose Unit" ) );
	setModal( true );
	KVBox *page = new KVBox(this );
//PORTING: Verify that widget was added to mainLayout: 	setMainWidget( page );
// Add mainLayout->addWidget(page); if necessary

	box = new QGroupBox( page );
	box->setTitle( i18nc( "@title:group", "Choose Unit" ) );
	box->setColumnLayout( 0, Qt::Vertical );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );

    unitChooseView = new QListWidget( box );

	KConfigGroup config( KSharedConfig::openConfig(), "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	unitChooseView->addColumn( i18nc( "@title:column", "Id" ) , show_id ? -1 : 0 );

	unitChooseView->addColumn( i18nc( "@title:column", "Unit" ) );
	unitChooseView->setSorting(1);
	unitChooseView->setGeometry( QRect( 5, 30, 180, 250 ) );
	unitChooseView->setAllColumnsShowFocus( true );
	boxLayout->addWidget( unitChooseView );

	resize( QSize( 200, 350 ) );

	loadUnits( unitList );
}


SelectUnitDialog::~SelectUnitDialog()
{}

int SelectUnitDialog::unitID( void )
{

    QListWidgetItem * it;
	if ( ( it = unitChooseView->selectedItem() ) ) {
		return ( it->text( 0 ).toInt() );
	}
	else
		return ( -1 );
}

void SelectUnitDialog::loadUnits( const UnitList &unitList )
{
	for ( UnitList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it ) {
		QString unitName = ( *unit_it ).name();
		if ( unitName.isEmpty() ) {
			if ( m_showEmpty == ShowEmptyUnit )
				unitName = ' '+i18nc("@item", "-No unit-");
			else
				continue;
		}

        ( void ) new QListWidgetItem( unitChooseView, QString::number( ( *unit_it ).id() ), unitName );
	}
}

