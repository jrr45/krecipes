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

#include "resizerecipedialog.h"

#include "ui_resizerecipedialog.h"

#include <cmath>

#include <klocale.h>
#include <kmessagebox.h>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "datablocks/recipe.h"
#include "widgets/fractioninput.h"


ResizeRecipeDialog::ResizeRecipeDialog( QWidget *parent, Recipe *recipe )
		: QDialog( parent ),
		m_recipe( recipe )
{
	setWindowTitle(i18nc("@title:window", "Resize Recipe" ));
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
	ui = new Ui::ResizeRecipeDialog;
	QWidget * mainWidget = new QWidget( this );
	ui->setupUi( mainWidget );
//PORTING: Verify that widget was added to mainLayout: 	setMainWidget( mainWidget );
// Add mainLayout->addWidget(mainWidget); if necessary

	//Ignore the range info, it doesn't work in this context
	ui->m_newYieldInput->setValue( m_recipe->yield.amount(), 0 );
	ui->m_currentYieldInput->setText( m_recipe->yield.toString() );

	if ( recipe->yield.amountOffset() > 0 ) {
		ui->m_yieldButton->setEnabled(false);
		ui->m_buttonGroup->setSelected( ui->m_buttonGroup->id(ui->m_factorButton) );
		activateCurrentOption( ui->m_buttonGroup->id(ui->m_factorButton) );
	} else {
		//We are selecting the factorbutton first to workaround
		//a possible bug in KButtonGroup.
		ui->m_buttonGroup->setSelected( ui->m_buttonGroup->id(ui->m_factorButton) );
		ui->m_buttonGroup->setSelected( ui->m_buttonGroup->id(ui->m_yieldButton) );
		activateCurrentOption( ui->m_buttonGroup->id(ui->m_yieldButton) );
	}

	// signals and slots connections
	connect( ui->m_buttonGroup, SIGNAL( changed( int ) ),
		this, SLOT( activateCurrentOption( int ) ) );
}

void ResizeRecipeDialog::activateCurrentOption( int button_id )
{
	if ( button_id == ui->m_buttonGroup->id(ui->m_yieldButton) ) {
		ui->m_factorFrame->setEnabled( false );
		ui->m_yieldFrame->setEnabled( true );
	} else if ( button_id == ui->m_buttonGroup->id(ui->m_factorButton) ) {
		ui->m_factorFrame->setEnabled( true );
		ui->m_yieldFrame->setEnabled( false );
	}
}

void ResizeRecipeDialog::accept()
{
	if ( m_recipe->yield.amount() == 0 ) {
		KMessageBox::error( this, i18nc( "@info", "Unable to scale a recipe with zero yield" ) );
	} else if ( ui->m_buttonGroup->selected() == ui->m_buttonGroup->id(ui->m_yieldButton) ) {
		if ( ui->m_newYieldInput->isInputValid() ) {
			double new_yield = ui->m_newYieldInput->value().toDouble();
			MixedNumber number;
			MixedNumber::fromString( ui->m_currentYieldInput->text(), number );
			double current_yield = number.toDouble();

			resizeRecipe( new_yield / current_yield );
		} else {
			KMessageBox::error( this, i18nc( "@info", "Invalid input" ) );
			ui->m_newYieldInput->selectAll();
			return;
		}
	} else {
		if ( ui->m_factorInput->isInputValid() && ui->m_factorInput->value() > 0 ) {
			resizeRecipe( ui->m_factorInput->value().toDouble() );
		} else {
			KMessageBox::error( this, i18nc( "@info", "Invalid input" ) );
			ui->m_factorInput->selectAll();
			return ;
		}
	}

	QDialog::accept();
}

void ResizeRecipeDialog::resizeRecipe( double factor )
{
	MixedNumber number;
	MixedNumber::fromString( ui->m_currentYieldInput->text(), number );
	m_recipe->yield.setAmount( number.toDouble() * factor );

	for ( IngredientList::iterator ing_it = m_recipe->ingList.begin(); ing_it != m_recipe->ingList.end(); ++ing_it ) {
		( *ing_it ).amount = ( *ing_it ).amount * factor;
		( *ing_it ).amount_offset = ( *ing_it ).amount_offset * factor;
	}
}

#include "resizerecipedialog.moc"
