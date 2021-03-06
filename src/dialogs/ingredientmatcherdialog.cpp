/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientmatcherdialog.h"

#include "datablocks/recipelist.h"
#include "widgets/ingredientlistview.h"
#include "datablocks/elementlist.h"
#include "backends/recipedb.h"
#include "widgets/krelistview.h"
#include "widgets/unitcombobox.h"
#include "widgets/fractioninput.h"
#include "widgets/amountunitinput.h"
#include "datablocks/mixednumber.h"
#include "actionshandlers/recipeactionshandler.h"

#include <QPainter>
#include <QGroupBox>
#include <QPointer>
#include <QHBoxLayout>
#include <QList>
#include <QLabel>
#include <QVBoxLayout>

#include <kapplication.h>
#include <kcursor.h>
#include <klocale.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>
#include <QDialog>
#include <kvbox.h>
#include <KHBox>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <KSharedConfig>

#include "profiling.h"

IngredientMatcherDialog::IngredientMatcherDialog( QWidget *parent, RecipeDB *db ) : QSplitter( parent )
{
	// Initialize internal variables
	database = db;

	//Design the dialog

	setOrientation( Qt::Vertical );
	setChildrenCollapsible( false );
	
	QWidget * upperBox = new QWidget( this );

	// Ingredient list
	QHBoxLayout *layout2 = new QHBoxLayout;
	layout2->setObjectName( "layout2" );

	allIngListView = new KreListView( this, QString(), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView(allIngListView,database);
    list_view->setSelectionMode( QListWidget::Multi );
	allIngListView->setListView(list_view);
	layout2->addWidget( allIngListView );
	allIngListView->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

	QVBoxLayout *layout1 = new QVBoxLayout;
	layout1->addStretch();
	layout1->setObjectName( "layout1" );

	addButton = new QPushButton;
	addButton->setObjectName( "addButton" );
	addButton->setIcon( QIcon::fromTheme(QStringLiteral("arrow-right")) );
	addButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( addButton );

	removeButton = new QPushButton;
	removeButton->setObjectName( "removeButton" );
	removeButton->setIcon( QIcon::fromTheme(QStringLiteral("arrow-left")) );
	removeButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( removeButton );
	layout1->addStretch();
	layout2->addLayout( layout1 );

	ingListView = new KreListView( this, QString(), true );
	ingListView->listView() ->addColumn( i18nc( "@title:column", "Ingredient (required?)" ) );
	ingListView->listView() ->addColumn( i18nc( "@title:column", "Amount Available" ) );
	layout2->addWidget( ingListView );
	upperBox->setLayout( layout2 );
	addWidget( upperBox );
	ingListView->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

	KVBox * lowerBox = new KVBox( this );

	// Box to select allowed number of missing ingredients
	missingBox = new KHBox( lowerBox );
	missingNumberLabel = new QLabel( missingBox );
	missingNumberLabel->setText( i18nc(
		"@label:spinbox Number of missing ingredients allowed when doing a search by ingredients",
		"Missing ingredients allowed:" ) );
	missingNumberSpinBox = new QSpinBox( missingBox );
	missingNumberSpinBox->setMinimum( -1 );
	missingNumberSpinBox->setSpecialValueText( i18nc(
		"@item Any amount of ingredients missing when doing a search by ingredients", "Any" ) );

	// Found recipe list
	recipeListView = new KreListView( lowerBox, i18nc( "@title", "Matching Recipes" ), false, 1, missingBox );
	recipeListView->listView() ->setAllColumnsShowFocus( true );

	recipeListView->listView() ->addColumn( i18nc( "@title:column Recipe Title", "Title" ) );

    KConfigGroup config( KSharedConfig::openConfig(), "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	recipeListView->listView() ->addColumn( "Id" , show_id ? -1 : 0 );

	recipeListView->listView()->addColumn( i18nc( "@title:column Missing ingredients in a search result",
		"Missing Ingredients" ) );

	recipeListView->listView() ->setSorting( -1 );
	recipeListView->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

	actionHandler = new RecipeActionsHandler( recipeListView->listView(), database );

	KHBox *buttonBox = new KHBox( lowerBox );

	okButton = new QPushButton( buttonBox );
	okButton->setIcon( QIcon::fromTheme(QStringLiteral("dialog-ok")) );
	okButton->setText( i18nc( "@action:button", "Find matching recipes" ) );

	//buttonBox->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton = new QPushButton( buttonBox );
	clearButton->setIcon( QIcon::fromTheme(QStringLiteral("edit-clear")) );
	clearButton->setText( i18nc( "@action:button Clear search criteria", "Clear" ) );

	addWidget( lowerBox );

	// Connect signals & slots
	connect ( okButton, SIGNAL( clicked() ), this, SLOT( findRecipes() ) );
	connect ( clearButton, SIGNAL( clicked() ), recipeListView->listView(), SLOT( clear() ) );
	connect ( clearButton, SIGNAL( clicked() ), this, SLOT( unselectIngredients() ) );
	connect( recipeListView->listView(), SIGNAL( selectionChanged() ), this, SLOT( haveSelectedItems() ) );
	connect ( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( addButton, SIGNAL( clicked() ), this, SLOT( addIngredient() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeIngredient() ) );
    connect( ingListView->listView(), SIGNAL( doubleClicked( QListWidgetItem*, const QPoint &, int ) ), SLOT( itemRenamed( QListWidgetItem*, const QPoint &, int ) ) );
}

IngredientMatcherDialog::~IngredientMatcherDialog()
{
}

void IngredientMatcherDialog::itemRenamed( QListWidgetItem* item, const QPoint &, int col )
{
	if ( col == 1 ) {
		QPointer<QDialog> amountEditDialog = new QDialog(this);
		amountEditDialog->setWindowTitle(i18nc("@title:window", "Enter amount"));
		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
		QWidget *mainWidget = new QWidget(this);
		QVBoxLayout *mainLayout = new QVBoxLayout;
		amountEditDialog->setLayout(mainLayout);
		mainLayout->addWidget(mainWidget);
		QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
		okButton->setDefault(true);
		okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
		amountEditDialog->connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		amountEditDialog->connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
		//PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
		mainLayout->addWidget(buttonBox);
		buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
		amountEditDialog->setModal( false );	
		QGroupBox *box = new QGroupBox( 1, Qt::Horizontal, i18nc("@title:group", "Amount"), amountEditDialog );
		AmountUnitInput *amountEdit = new AmountUnitInput( box, database );
		// Set the values from the item
		if ( !item->text(1).isEmpty() ) {
			MixedNumber number;
			MixedNumber::fromString( item->text(2), number );
			amountEdit->setAmount( number );
			Unit u;
			u.setId(item->text(3).toInt());
			amountEdit->setUnit( u );
		} else {
			amountEdit->setAmount( MixedNumber(-1) );
			Unit u;
			u.setId(-1);
			amountEdit->setUnit( u );
		}

		mainLayout->addWidget(box);
		amountEditDialog->adjustSize();
		amountEditDialog->resize( 400, amountEditDialog->size().height() );
		amountEditDialog->setFixedHeight( amountEditDialog->size().height() );

		if ( amountEditDialog->exec() == QDialog::Accepted ) {
			MixedNumber amount = amountEdit->amount();
			Unit unit = amountEdit->unit();

			if ( amount.toDouble() <= 1e-5 ) {
				amount = MixedNumber(-1);
				unit.setId(-1);

				item->setText(1,QString());
			} else {
				item->setText(1,amount.toString()+' '+unit.determineName(amount.toDouble(), /*useAbbrev=*/false));
			}

			item->setText(2,amount.toString());
			item->setText(3,QString::number(unit.id()));

			IngredientList::iterator ing_it = m_item_ing_map[item];
			(*ing_it).amount = amount.toDouble();
			(*ing_it).units = unit;
		}

		delete amountEditDialog;
	}
}

void IngredientMatcherDialog::addIngredient()
{
    QList<QListWidgetItem *> items = allIngListView->listView()->selectedItems();
	if ( !items.isEmpty() ) {
		for (int i = 0; i < items.size(); ++i) {
			bool dup = false;
            for ( QListWidgetItem *exists_it = ingListView->listView()->firstChild(); exists_it; exists_it = exists_it->nextSibling() ) {
				if ( exists_it->text( 0 ) == items[i]->text( 0 ) ) {
					dup = true;
					break;
				}
			}

			if ( !dup ) {
                QListWidgetItem * new_item = new QListWidgetItem( ingListView->listView(), items[i]->text( 0 ), QListWidgetItem::CheckBox );

				ingListView->listView() ->setSelected( new_item, true );
				ingListView->listView() ->ensureItemVisible( new_item );
				allIngListView->listView() ->setSelected( items[i], false );

				IngredientList::iterator it = m_ingredientList.append( Ingredient( items[i]->text( 0 ), 0, Unit(), -1, items[i]->text( 1 ).toInt() ) );
				m_item_ing_map.insert( new_item, it );
			}
		}
	}
}

void IngredientMatcherDialog::removeIngredient()
{
    QListWidgetItem * item = ingListView->listView() ->selectedItem();
	if ( item ) {
		for ( IngredientList::iterator it = m_ingredientList.begin(); it != m_ingredientList.end(); ++it ) {
			if ( *m_item_ing_map.find( item ) == it ) {
				m_ingredientList.remove( it );
				m_item_ing_map.remove( item );
				break;
			}
		}
		delete item;
	}
}

void IngredientMatcherDialog::unselectIngredients()
{
	ingListView->listView()->clear();
    for ( QListWidgetItem *it = allIngListView->listView()->firstChild(); it; it = it->nextSibling() )
		allIngListView->listView()->setSelected(it,false);
	m_ingredientList.clear();
	m_item_ing_map.clear();
}

void IngredientMatcherDialog::findRecipes( void )
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	START_TIMER("Ingredient Matcher: loading database data");

	RecipeList rlist;
	database->loadRecipes( &rlist, RecipeDB::Title | RecipeDB::NamesOnly | RecipeDB::Ingredients | RecipeDB::IngredientAmounts );

	END_TIMER();
	START_TIMER("Ingredient Matcher: analyzing data for matching recipes");

	// Clear the list
	recipeListView->listView() ->clear();

	// Now show the recipes with ingredients that are contained in the previous set
	// of ingredients
	RecipeList incompleteRecipes;
	QList <int> missingNumbers;
    QList <IngredientList> missingIngredients;

	RecipeList::Iterator it;
	for ( it = rlist.begin();it != rlist.end();++it ) {
		IngredientList il = ( *it ).ingList;
		if ( il.isEmpty() )
			continue;

		IngredientList missing;
		if ( m_ingredientList.containsSubSet( il, missing, true, database ) ) {
			new CustomRecipeListItem( recipeListView->listView(), *it );
		}
		else {
			incompleteRecipes.append( *it );
			missingIngredients.append( missing );
			missingNumbers.append( missing.count() );
		}
	}
	END_TIMER();

	//Check if the user wants to show missing ingredients

	if ( missingNumberSpinBox->value() == 0 ) {
		KApplication::restoreOverrideCursor();
		return ;
	} //"None"



	START_TIMER("Ingredient Matcher: searching for and displaying partial matches");

	IngredientList requiredIngredients;
    for ( QListWidgetItem *it = ingListView->listView()->firstChild(); it; it = it->nextSibling() ) {
        if ( ((QListWidgetItem*)it)->isOn() )
			requiredIngredients << *m_item_ing_map[it];
	}

	// Classify recipes with missing ingredients in different lists by amount
	QList<int>::Iterator nit;
    QList<IngredientList>::Iterator ilit;
	int missingNoAllowed = missingNumberSpinBox->value();

	if ( missingNoAllowed == -1 )  // "Any"
	{
		for ( nit = missingNumbers.begin();nit != missingNumbers.end();++nit )
			if ( ( *nit ) > missingNoAllowed )
				missingNoAllowed = ( *nit );
	}


	for ( int missingNo = 1; missingNo <= missingNoAllowed; missingNo++ ) {
		nit = missingNumbers.begin();
		ilit = missingIngredients.begin();

		bool titleShownYet = false;

		for ( it = incompleteRecipes.begin();it != incompleteRecipes.end();++it, ++nit, ++ilit ) {
			if ( !( *it ).ingList.containsAny( m_ingredientList ) )
				continue;

			if ( !( *it ).ingList.containsSubSet( requiredIngredients ) )
				continue;

			if ( ( *nit ) == missingNo ) {
				if ( !titleShownYet ) {
					new SectionItem( recipeListView->listView(), i18ncp( "@label:textbox", "You are missing 1 ingredient for:", "You are missing %1 ingredients for:", missingNo ) );
					titleShownYet = true;
				}
				new CustomRecipeListItem( recipeListView->listView(), *it, *ilit );
			}
		}
	}
	END_TIMER();

	KApplication::restoreOverrideCursor();
}

void IngredientMatcherDialog::reload( ReloadFlags flag )
{
	( ( StdIngredientListView* ) allIngListView->listView() ) ->reload(flag);
}

RecipeActionsHandler* IngredientMatcherDialog::getActionsHandler() const
{
	return actionHandler;
}

void IngredientMatcherDialog::addAction( QAction * action )
{
	actionHandler->addRecipeAction( action );
}

void IngredientMatcherDialog::haveSelectedItems()
{
	QList<int> list = actionHandler->recipeIDs();
	if ( list.isEmpty() )
		emit recipeSelected( false );
	else
		emit recipeSelected( true );
}

void SectionItem::paintCell ( QPainter * p, const QColorGroup & /*cg*/, int column, int width, int /*align*/ )
{
	// Draw the section's deco
	p->setPen( KGlobalSettings::activeTitleColor() );
	p->setBrush( KGlobalSettings::activeTitleColor() );
	p->drawRect( 0, 0, width, height() );

	// Draw the section's text
	if ( column == 0 ) {
		QFont titleFont = KGlobalSettings::windowTitleFont ();
		p->setFont( titleFont );

		p->setPen( KGlobalSettings::activeTextColor() );
		p->drawText( 0, 0, width, height(), Qt::AlignLeft | Qt::AlignVCenter, mText );
	}
}

