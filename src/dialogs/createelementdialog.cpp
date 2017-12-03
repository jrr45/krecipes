/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2008 Montel Laurent <montel@kde.org>                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createelementdialog.h"
#include <kvbox.h>
#include <klocale.h>
#include <QVBoxLayout>
#include <QLineEdit>
#include <Q3GroupBox>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>

CreateElementDialog::CreateElementDialog( QWidget *parent, const QString &text )
	 : QDialog( parent)
{
	 setWindowTitle( text );
	 setModal( true );
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

	 KVBox *page = new KVBox( this );
//PORTING: Verify that widget was added to mainLayout: 	 setMainWidget( page );
// Add mainLayout->addWidget(page); if necessary
	 Q3GroupBox *box = new Q3GroupBox( page );
	 box->setColumnLayout( 0, Qt::Vertical );
	 box->layout() ->setSpacing( 6 );
	 box->layout() ->setMargin( 11 );
	 QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
	 boxLayout->setAlignment( Qt::AlignTop );
	 box->setTitle( text );

	 elementEdit = new QLineEdit( box );
	 boxLayout->addWidget( elementEdit );

	 adjustSize();
	 resize( 450, size().height() );
	 setFixedHeight( size().height() );

	 elementEdit->setFocus();
	 connect( elementEdit, SIGNAL( textChanged(const QString& ) ), this, SLOT( slotTextChanged( const QString& ) ) );
	 okButton->setEnabled( false );
}


CreateElementDialog::~CreateElementDialog()
{}

QString CreateElementDialog::newElementName() const
{
	return ( elementEdit->text() );
}

void CreateElementDialog::slotTextChanged( const QString& text )
{
	okButton->setEnabled( !text.isEmpty() );
}
