/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "advancedsearchdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qcheckbox.h>
#include <q3frame.h>
#include <qcombobox.h>
#include <q3header.h>
#include <q3listview.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <q3datetimeedit.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3ValueList>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <kpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3scrollview.h>

#include <qradiobutton.h>
#include <q3buttongroup.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <k3listview.h>
#include <klocale.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kmenu.h>
#include <kvbox.h>

#include "backends/recipedb.h"
#include "backends/searchparameters.h"
#include "recipeactionshandler.h"
#include "widgets/recipelistview.h"
#include "widgets/kdateedit.h"
#include "widgets/ratingwidget.h"
#include "widgets/fractioninput.h"
#include "widgets/criteriacombobox.h"

#include "profiling.h"

AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent ),
		database( db )
{
	///
	///BEGIN OF AUTOMATICALLY GENERATED GUI CODE///
	///
	AdvancedSearchDialogLayout = new Q3HBoxLayout( this, 5, 3, "AdvancedSearchDialogLayout"); 
	
	layout7 = new Q3VBoxLayout( 0, 0, 3, "layout7"); 
	
	textLabel1_4 = new QLabel( this );
   textLabel1_4->setObjectName( "textLabel1_4" );
	layout7->addWidget( textLabel1_4 );
	
	scrollView1 = new Q3ScrollView( this, "scrollView1" );
	scrollView1->enableClipper(true);
	
	parametersFrame = new Q3Frame( scrollView1, "parametersFrame" );
	parametersFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, parametersFrame->sizePolicy().hasHeightForWidth() ) );
	parametersFrame->setFrameShape( Q3Frame::NoFrame );
	parametersFrame->setFrameShadow( Q3Frame::Plain );
	parametersFrame->setLineWidth( 0 );
	parametersFrameLayout = new Q3VBoxLayout( parametersFrame, 0, 0, "parametersFrameLayout"); 

	titleButton = new QPushButton( parametersFrame, "titleButton" );
	titleButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( titleButton );
	
	titleFrame = new Q3Frame( parametersFrame, "titleFrame" );
	titleFrame->setFrameShape( Q3Frame::StyledPanel );
	titleFrame->setFrameShadow( Q3Frame::Raised );
	titleFrameLayout = new Q3VBoxLayout( titleFrame, 5, 3, "titleFrameLayout"); 

	requireAllTitle = new QCheckBox( i18n("Require All Words"), titleFrame );
	titleFrameLayout->addWidget( requireAllTitle );

	KHBox *titleHBox = new KHBox( titleFrame );
	/*QLabel *titleInfoLabel = */new QLabel(i18n("Keywords:"),titleHBox);
	titleEdit = new QLineEdit( titleHBox, "titleEdit" );
	titleFrameLayout->addWidget( titleHBox );

	parametersFrameLayout->addWidget( titleFrame );
	titleFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( titleFrameSpacer );


	ingredientButton = new QPushButton( parametersFrame );
   ingredientButton->setObjectName( "ingredientButton" );
	ingredientButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( ingredientButton );
	
	ingredientFrame = new Q3Frame( parametersFrame, "ingredientFrame" );
	ingredientFrame->setFrameShape( Q3Frame::StyledPanel );
	ingredientFrame->setFrameShadow( Q3Frame::Raised );
	ingredientFrameLayout = new Q3GridLayout( ingredientFrame, 1, 1, 3, 3, "ingredientFrameLayout"); 

	QLabel *ingredientInfoLabel = new QLabel(i18n("Enter ingredients: (e.g. chicken pasta \"white wine\")"),ingredientFrame);
	ingredientInfoLabel->setTextFormat( Qt::RichText );
	ingredientFrameLayout->addMultiCellWidget( ingredientInfoLabel, 0, 0, 0, 1 );

	ingredientsAllEdit = new QLineEdit( ingredientFrame, "ingredientsAllEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAllEdit, 1, 1 );
	
	ingredientsAnyEdit = new QLineEdit( ingredientFrame, "ingredientsAnyEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAnyEdit, 2, 1 );
	
	textLabel1_2 = new QLabel( ingredientFrame );
   textLabel1_2->setObjectName( "textLabel1_2" );
	
	ingredientFrameLayout->addWidget( textLabel1_2, 2, 0 );
	
	textLabel1 = new QLabel( ingredientFrame );
   textLabel1->setObjectName( "textLabel1" );
	
	ingredientFrameLayout->addWidget( textLabel1, 1, 0 );
	
	ingredientsWithoutEdit = new QLineEdit( ingredientFrame, "ingredientsWithoutEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsWithoutEdit, 3, 1 );
	
	textLabel1_3 = new QLabel( ingredientFrame );
   textLabel1_3->setObjectName( "textLabel1_3" );
	
	ingredientFrameLayout->addWidget( textLabel1_3, 3, 0 );
	parametersFrameLayout->addWidget( ingredientFrame );
	spacer3_2_3_2_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2_2 );


	categoriesButton = new QPushButton( parametersFrame );
   categoriesButton->setObjectName( "categoriesButton" );
	categoriesButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( categoriesButton );
	
	categoryFrame = new Q3Frame( parametersFrame, "categoryFrame" );
	categoryFrame->setFrameShape( Q3Frame::StyledPanel );
	categoryFrame->setFrameShadow( Q3Frame::Raised );
	categoryFrameLayout = new Q3GridLayout( categoryFrame, 1, 1, 3, 3, "categoryFrameLayout");

	QLabel *categoryInfoLabel = new QLabel(i18n("Enter categories: (e.g. Desserts Pastas \"Main Dishes\")"),categoryFrame);
	categoryInfoLabel->setTextFormat( Qt::RichText );
	categoryFrameLayout->addMultiCellWidget( categoryInfoLabel, 0, 0, 0, 1 );
	
	categoriesAllEdit = new QLineEdit( categoryFrame, "categoriesAllEdit" );
	
	categoryFrameLayout->addWidget( categoriesAllEdit, 1, 1 );
	
	textLabel1_5 = new QLabel( categoryFrame );
   textLabel1_5->setObjectName( "textLabel1_5" );
	
	categoryFrameLayout->addWidget( textLabel1_5, 1, 0 );
	
	textLabel1_3_3 = new QLabel( categoryFrame );
   textLabel1_3_3->setObjectName( "textLabel1_3_3" );
	
	categoryFrameLayout->addWidget( textLabel1_3_3, 3, 0 );
	
	categoriesAnyEdit = new QLineEdit( categoryFrame, "categoriesAnyEdit" );
	
	categoryFrameLayout->addWidget( categoriesAnyEdit, 2, 1 );
	
	textLabel1_2_3 = new QLabel( categoryFrame );
   textLabel1_2_3->setObjectName( "textLabel1_2_3" );
	
	categoryFrameLayout->addWidget( textLabel1_2_3, 2, 0 );
	
	categoriesNotEdit = new QLineEdit( categoryFrame, "categoriesNotEdit" );
	
	categoryFrameLayout->addWidget( categoriesNotEdit, 3, 1 );
	parametersFrameLayout->addWidget( categoryFrame );
	spacer3_2_3_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2 );


	authorsButton = new QPushButton( parametersFrame );
   authorsButton->setObjectName( "authorsButton" );
	authorsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( authorsButton );
	
	authorsFrame = new Q3Frame( parametersFrame, "authorsFrame" );
	authorsFrame->setFrameShape( Q3Frame::StyledPanel );
	authorsFrame->setFrameShadow( Q3Frame::Raised );
	authorsFrameLayout = new Q3GridLayout( authorsFrame, 1, 1, 3, 3, "authorsFrameLayout"); 

	QLabel *authorsInfoLabel = new QLabel(i18n("Enter author name (e.g. Smith or \"Jane Doe\")"),authorsFrame);
	authorsInfoLabel->setTextFormat( Qt::RichText );
	authorsFrameLayout->addMultiCellWidget( authorsInfoLabel, 0, 0, 0, 1 );

	textLabel1_2_4 = new QLabel( authorsFrame );
   textLabel1_2_4->setObjectName( "textLabel1_2_4" );
	
	authorsFrameLayout->addWidget( textLabel1_2_4, 1, 0 );
	
	textLabel1_6 = new QLabel( authorsFrame );
   textLabel1_6->setObjectName( "textLabel1_6" );
	
	authorsFrameLayout->addWidget( textLabel1_6, 2, 0 );
	
	textLabel1_3_4 = new QLabel( authorsFrame );
   textLabel1_3_4->setObjectName( "textLabel1_3_4" );
	
	authorsFrameLayout->addWidget( textLabel1_3_4, 3, 0 );
	
	authorsAnyEdit = new QLineEdit( authorsFrame, "authorsAnyEdit" );
	
	authorsFrameLayout->addWidget( authorsAnyEdit, 1, 1 );
	
	authorsAllEdit = new QLineEdit( authorsFrame, "authorsAllEdit" );
	
	authorsFrameLayout->addWidget( authorsAllEdit, 2, 1 );
	
	authorsWithoutEdit = new QLineEdit( authorsFrame, "authorsWithoutEdit" );
	
	authorsFrameLayout->addWidget( authorsWithoutEdit, 3, 1 );
	parametersFrameLayout->addWidget( authorsFrame );
	spacer3_2_3 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3 );


	servingsButton = new QPushButton( parametersFrame );
   servingsButton->setObjectName( "servingsButton" );
	servingsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( servingsButton );
	
	servingsFrame = new Q3Frame( parametersFrame, "servingsFrame" );
	servingsFrame->setFrameShape( Q3Frame::StyledPanel );
	servingsFrame->setFrameShadow( Q3Frame::Raised );
	servingsFrameLayout = new Q3VBoxLayout( servingsFrame, 3, 3, "servingsFrameLayout"); 
	
	enableServingsCheckBox = new QCheckBox( servingsFrame );
   enableServingsCheckBox->setObjectName( "enableServingsCheckBox" );
	servingsFrameLayout->addWidget( enableServingsCheckBox );
	
	layout5 = new Q3HBoxLayout( 0, 0, 3, "layout5"); 
	
	servingsComboBox = new QComboBox( FALSE, servingsFrame, "servingsComboBox" );
	servingsComboBox->setEnabled( FALSE );
	servingsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, servingsComboBox->sizePolicy().hasHeightForWidth() ) );
	layout5->addWidget( servingsComboBox );
	
	servingsSpinBox = new QSpinBox( servingsFrame, "servingsSpinBox" );
	servingsSpinBox->setEnabled( FALSE );
	servingsSpinBox->setMinValue( 1 );
	servingsSpinBox->setMaxValue( 9999 );
	layout5->addWidget( servingsSpinBox );
	servingsFrameLayout->addLayout( layout5 );
	parametersFrameLayout->addWidget( servingsFrame );
	spacer3_2_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_2 );


	prepTimeButton = new QPushButton( parametersFrame );
   prepTimeButton->setObjectName( "prepTimeButton" );
	prepTimeButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( prepTimeButton );
	
	prepTimeFrame = new Q3Frame( parametersFrame, "prepTimeFrame" );
	prepTimeFrame->setFrameShape( Q3Frame::StyledPanel );
	prepTimeFrame->setFrameShadow( Q3Frame::Raised );
	prepTimeFrameLayout = new Q3VBoxLayout( prepTimeFrame, 3, 3, "prepTimeFrameLayout"); 
	
	enablePrepTimeCheckBox = new QCheckBox( prepTimeFrame );
   enablePrepTimeCheckBox->setObjectName( "enablePrepTimeCheckBox" );
	prepTimeFrameLayout->addWidget( enablePrepTimeCheckBox );
	
	layout6 = new Q3HBoxLayout( 0, 0, 3, "layout6"); 
	
	prepTimeComboBox = new QComboBox( FALSE, prepTimeFrame, "prepTimeComboBox" );
	prepTimeComboBox->setEnabled( FALSE );
	prepTimeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, prepTimeComboBox->sizePolicy().hasHeightForWidth() ) );
	layout6->addWidget( prepTimeComboBox );
	
	prepTimeEdit = new Q3TimeEdit( prepTimeFrame, "prepTimeEdit" );
	prepTimeEdit->setEnabled( FALSE );
	prepTimeEdit->setDisplay( int( Q3TimeEdit::Minutes | Q3TimeEdit::Hours ) );
	layout6->addWidget( prepTimeEdit );
	prepTimeFrameLayout->addLayout( layout6 );
	parametersFrameLayout->addWidget( prepTimeFrame );
	spacer15 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer15 );


	instructionsButton = new QPushButton( parametersFrame );
   instructionsButton->setObjectName( "instructionsButton" );
	instructionsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( instructionsButton );
	
	instructionsFrame = new Q3Frame( parametersFrame, "instructionsFrame" );
	instructionsFrame->setFrameShape( Q3Frame::StyledPanel );
	instructionsFrame->setFrameShadow( Q3Frame::Raised );
	instructionsFrameLayout = new Q3VBoxLayout( instructionsFrame, 5, 3, "instructionsFrameLayout"); 

	requireAllInstructions = new QCheckBox( i18n("Require All Words"), instructionsFrame );
	instructionsFrameLayout->addWidget( requireAllInstructions );

	KHBox *instructionsHBox = new KHBox(instructionsFrame);
	/*QLabel *instructionsInfoLabel = */new QLabel(i18n("Keywords:"),instructionsHBox);

	instructionsEdit = new QLineEdit( instructionsHBox, "instructionsEdit" );
	instructionsFrameLayout->addWidget( instructionsHBox );

	parametersFrameLayout->addWidget( instructionsFrame );
	instructionsFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( instructionsFrameSpacer );


	metaDataButton = new QPushButton( parametersFrame );
   metaDataButton->setObjectName( "metaDataButton" );
	metaDataButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( metaDataButton );
	
	metaDataFrame = new Q3Frame( parametersFrame, "metaDataFrame" );
	metaDataFrame->setFrameShape( Q3Frame::StyledPanel );
	metaDataFrame->setFrameShadow( Q3Frame::Raised );
	metaDataFrameLayout = new Q3VBoxLayout( metaDataFrame, 5, 3, "metaDataFrameLayout");

	QLabel *createdLabel = new QLabel( i18n("Created:"), metaDataFrame );
	metaDataFrameLayout->addWidget( createdLabel );

	KHBox *createdHBox = new KHBox(metaDataFrame);
	createdStartDateEdit = new KDateEdit(createdHBox,"createdStartEdit");
	createdStartDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	(void)new QLabel(" - ",createdHBox);
	createdEndDateEdit = new KDateEdit(createdHBox,"createdEndEdit");
	createdEndDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	metaDataFrameLayout->addWidget( createdHBox );

	QLabel *modifiedLabel = new QLabel( i18n("Modified:"), metaDataFrame );
	metaDataFrameLayout->addWidget( modifiedLabel );

	KHBox *modifiedHBox = new KHBox(metaDataFrame);
	modifiedStartDateEdit = new KDateEdit(modifiedHBox,"modifiedStartEdit");
	modifiedStartDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	(void)new QLabel(" - ",modifiedHBox);
	modifiedEndDateEdit = new KDateEdit(modifiedHBox,"modifiedEndEdit");
	modifiedEndDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	metaDataFrameLayout->addWidget( modifiedHBox );

	QLabel *accessedLabel = new QLabel( i18n("Last Accessed:"), metaDataFrame );
	metaDataFrameLayout->addWidget( accessedLabel );

	KHBox *accessedHBox = new KHBox(metaDataFrame);
	accessedStartDateEdit = new KDateEdit(accessedHBox,"accessedStartEdit");
	accessedStartDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	(void)new QLabel(" - ",accessedHBox);
	accessedEndDateEdit = new KDateEdit(accessedHBox,"accessedEndEdit");
	accessedEndDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	metaDataFrameLayout->addWidget( accessedHBox );

	parametersFrameLayout->addWidget( metaDataFrame );
	metaDataFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( metaDataFrameSpacer );

	//=============RATINGS FRAME===========//
	ratingsButton = new QPushButton( parametersFrame );
   ratingsButton->setObjectName( "ratingsButton" );
	ratingsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( ratingsButton );

	ratingButtonGroup = new Q3ButtonGroup( parametersFrame, "ratingButtonGroup" );
	ratingButtonGroup->setLineWidth( 0 );
	ratingButtonGroup->setColumnLayout(0, Qt::Vertical );
	ratingButtonGroup->layout()->setSpacing( 5 );
	ratingButtonGroup->layout()->setMargin( 3 );
	ratingButtonGroupLayout = new Q3VBoxLayout( ratingButtonGroup->layout() );
	ratingButtonGroupLayout->setAlignment( Qt::AlignTop );
	
	ratingAvgRadioButton = new QRadioButton( ratingButtonGroup, "ratingAvgRadioButton" );
	ratingAvgRadioButton->setChecked( TRUE );
	ratingButtonGroupLayout->addWidget( ratingAvgRadioButton );
	
	ratingAvgFrame = new Q3Frame( ratingButtonGroup, "ratingAvgFrame" );
	ratingAvgFrame->setLineWidth( 0 );
	ratingAvgFrameLayout = new Q3HBoxLayout( ratingAvgFrame, 2, 2, "ratingAvgFrameLayout"); 
	
	avgStarsEdit = new FractionInput( ratingAvgFrame );
	avgStarsEdit->setAllowRange(true);
	ratingAvgFrameLayout->addWidget( avgStarsEdit );
	
	avgStarsLabel = new QLabel( ratingAvgFrame );
   avgStarsLabel->setObjectName( "avgStarsLabel" );
	ratingAvgFrameLayout->addWidget( avgStarsLabel );
	ratingButtonGroupLayout->addWidget( ratingAvgFrame );
	
	criterionRadioButton = new QRadioButton( ratingButtonGroup, "criterionRadioButton" );
	ratingButtonGroupLayout->addWidget( criterionRadioButton );
	
	criterionFrame = new Q3Frame( ratingButtonGroup, "criterionFrame" );
	criterionFrame->setEnabled( FALSE );
	criterionFrame->setLineWidth( 0 );
	criterionFrameLayout = new Q3VBoxLayout( criterionFrame, 2, 2, "criterionFrameLayout"); 
	
	layout12 = new Q3HBoxLayout( 0, 0, 6, "layout12"); 
	
	criteriaComboBox = new CriteriaComboBox( FALSE, criterionFrame, database );
	criteriaComboBox->setEditable( false );
	criteriaComboBox->reload();
	layout12->addWidget( criteriaComboBox );
	
	starsWidget = new FractionInput( criterionFrame );
	starsWidget->setAllowRange(true);
	starsWidget->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
	layout12->addWidget( starsWidget );
	

	addCriteriaButton = new QPushButton( criterionFrame );
   addCriteriaButton->setObjectName( "addCriteriaButton" );
	addCriteriaButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, addCriteriaButton->sizePolicy().hasHeightForWidth() ) );
	addCriteriaButton->setMaximumSize( QSize( 30, 30 ) );
	layout12->addWidget( addCriteriaButton );
#if 0
	removeCriteriaButton = new QPushButton( criterionFrame, "removeCriteriaButton" );
	removeCriteriaButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, removeCriteriaButton->sizePolicy().hasHeightForWidth() ) );
	removeCriteriaButton->setMaximumSize( QSize( 30, 30 ) );
	layout12->addWidget( removeCriteriaButton );
#endif
	criterionFrameLayout->addLayout( layout12 );
	
	criteriaListView = new K3ListView( criterionFrame );
	criteriaListView->setObjectName( "criteriaListView" );
	criteriaListView->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum );
	criteriaListView->addColumn( i18n( "Criterion" ) );
	criteriaListView->addColumn( i18n( "Stars" ) );
	criterionFrameLayout->addWidget( criteriaListView );
	ratingButtonGroupLayout->addWidget( criterionFrame );

	parametersFrameLayout->addWidget( ratingButtonGroup );
	ratingsFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
	parametersFrameLayout->addItem( ratingsFrameSpacer );


	scrollView1->addChild( parametersFrame );
	layout7->addWidget( scrollView1 );
	
	layout9 = new Q3HBoxLayout( 0, 0, 3, "layout9"); 
	
	clearButton = new KPushButton( this );
	clearButton->setObjectName( "clearButton" );
	clearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, clearButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( clearButton );
	spacer3 = new QSpacerItem( 110, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout9->addItem( spacer3 );
	
	findButton = new KPushButton( this );
	findButton->setObjectName( "findButton" );
	findButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, findButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( findButton );
	layout7->addLayout( layout9 );
	AdvancedSearchDialogLayout->addLayout( layout7 );
	
	resultsListView = new K3ListView( this); 
	resultsListView->setObjectName( "resultsListView" );
	resultsListView->setSelectionMode( Q3ListView::Extended );
	resultsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 1, resultsListView->sizePolicy().hasHeightForWidth() ) );
	AdvancedSearchDialogLayout->addWidget( resultsListView );
	languageChange();
	//KDE4 port
	//QWidget::setWindowState( WState_Polished );
	///
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	///

	resultsListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	scrollView1->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );

	AdvancedSearchDialogLayout->setStretchFactor( resultsListView, 2 );

	scrollView1->setHScrollBarMode( Q3ScrollView::AlwaysOff );
	scrollView1->setResizePolicy( Q3ScrollView::AutoOneFit );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	
	resultsListView->addColumn( i18n( "Title" ) );
	resultsListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	actionHandler = new RecipeActionsHandler( resultsListView, database, RecipeActionsHandler::Open | RecipeActionsHandler::Edit | RecipeActionsHandler::Export | RecipeActionsHandler::CopyToClipboard | RecipeActionsHandler::Remove );

	connect( titleEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsAllEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsAnyEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsWithoutEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( authorsAnyEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( authorsAllEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( authorsWithoutEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( categoriesNotEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( categoriesAnyEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( categoriesAllEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsWithoutEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( instructionsEdit, SIGNAL( returnPressed() ), SLOT( search() ) );

	connect( findButton, SIGNAL( clicked() ), SLOT( search() ) );
	connect( clearButton, SIGNAL( clicked() ), SLOT( clear() ) );

	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsSpinBox, SLOT( setEnabled( bool ) ) );
	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsComboBox, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepTimeEdit, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepTimeComboBox, SLOT( setEnabled( bool ) ) );

	connect( titleButton, SIGNAL( toggled( bool ) ), titleFrame, SLOT( setShown( bool ) ) );
	connect( ingredientButton, SIGNAL( toggled( bool ) ), ingredientFrame, SLOT( setShown( bool ) ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), authorsFrame, SLOT( setShown( bool ) ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), categoryFrame, SLOT( setShown( bool ) ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), servingsFrame, SLOT( setShown( bool ) ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), prepTimeFrame, SLOT( setShown( bool ) ) );
	connect( instructionsButton, SIGNAL( toggled( bool ) ), instructionsFrame, SLOT( setShown( bool ) ) );
	connect( metaDataButton, SIGNAL( toggled( bool ) ), metaDataFrame, SLOT( setShown( bool ) ) );
	connect( ratingsButton, SIGNAL( toggled( bool ) ), ratingButtonGroup, SLOT( setShown( bool ) ) );

	connect( titleButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( ingredientButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( instructionsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( metaDataButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( ratingsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );

	connect( ratingButtonGroup, SIGNAL( clicked( int ) ), this, SLOT( activateRatingOption( int ) ) );

	connect( addCriteriaButton, SIGNAL( clicked() ), this, SLOT( slotAddRatingCriteria() ) );

	titleFrame->setShown(false);
	ingredientFrame->setShown(false);
	authorsFrame->setShown(false);
	categoryFrame->setShown(false);
	servingsFrame->setShown(false);
	prepTimeFrame->setShown(false);
	instructionsFrame->setShown(false);
	metaDataFrame->setShown(false);
	ratingButtonGroup->setShown(false);

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( actionHandler, SIGNAL( recipesSelected( const Q3ValueList<int> &, int ) ), SIGNAL( recipesSelected( const Q3ValueList<int> &, int ) ) );

	connect( database, SIGNAL( recipeRemoved( int ) ), SLOT( removeRecipe( int ) ) );

	clear();

	KMenu *kpop = new KMenu( criteriaListView );
	kpop->insertItem( i18n( "&Delete" ), this, SLOT( slotRemoveRatingCriteria() ), Qt::Key_Delete );
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{}

void AdvancedSearchDialog::languageChange()
{
	titleButton->setText( QString("%1 >>").arg(i18n("Title")) );
	textLabel1_4->setText( i18n( "Search using the following criteria:" ) );
	ingredientButton->setText( QString("%1 >>").arg(i18n("Ingredients")) );
	textLabel1_2->setText( i18n( "Uses any of:" ) );
	textLabel1->setText( i18n( "Uses all:" ) );
	textLabel1_3->setText( i18n( "Without:" ) );
	categoriesButton->setText( i18n( "Categories >>" ) );
	textLabel1_5->setText( i18n( "In all:" ) );
	textLabel1_3_3->setText( i18n( "Not in:" ) );
	textLabel1_2_3->setText( i18n( "In any of:" ) );
	authorsButton->setText( QString("%1 >>").arg(i18n("Authors")) );
	textLabel1_2_4->setText( i18n( "By any of:" ) );
	textLabel1_6->setText( i18n( "By all:" ) );
	textLabel1_3_4->setText( i18n( "Not by:" ) );
	servingsButton->setText( QString("%1 >>").arg(i18n("Yield")) );
	enableServingsCheckBox->setText( i18n( "Enabled" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Yields at least:" ) );
	servingsComboBox->insertItem( i18n( "Yields at most:" ) );
	servingsComboBox->insertItem( i18n( "Yields about:" ) );
	prepTimeButton->setText( QString("%1 >>").arg(i18n("Preparation Time")) );
	enablePrepTimeCheckBox->setText( i18n( "Enabled" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( i18n( "Ready in at most:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in about:" ) );
	instructionsButton->setText( QString("%1 >>").arg(i18n("Instructions")) );
	metaDataButton->setText( QString("%1 >>").arg(i18n("Meta Data")) );
	clearButton->setText( i18n( "C&lear" ) );
	clearButton->setAccel( QKeySequence( i18n( "Alt+L" ) ) );
	findButton->setText( i18n( "&Search" ) );
	ratingAvgRadioButton->setText( i18n( "By average:" ) );
	avgStarsLabel->setText( i18n( "stars" ) );
	criterionRadioButton->setText( i18n( "By criteria:" ) );
	addCriteriaButton->setText( i18n( "+" ) );
	//removeCriteriaButton->setText( i18n( "-" ) );
	criteriaListView->header()->setLabel( 0, i18n( "Criteria" ) );
	criteriaListView->header()->setLabel( 1, i18n( "Stars" ) );
	ratingsButton->setText( QString("%1 >>").arg(i18n("Ratings")) );
}

void AdvancedSearchDialog::removeRecipe( int id )
{
	Q3ListViewItemIterator iterator( resultsListView );
	while ( iterator.current() ) {
		if ( iterator.current()->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
			if ( recipe_it->recipeID() == id )
				delete recipe_it;
		}
		++iterator;
	}
}

void AdvancedSearchDialog::clear()
{
	resultsListView->clear();
	authorsAllEdit->clear();
	authorsWithoutEdit->clear();
	authorsAnyEdit->clear();
	categoriesAllEdit->clear();
	categoriesNotEdit->clear();
	categoriesAnyEdit->clear();
	ingredientsAllEdit->clear();
	ingredientsWithoutEdit->clear();
	ingredientsAnyEdit->clear();
	titleEdit->clear();
	instructionsEdit->clear();

	createdStartDateEdit->setDate( QDate() );
	createdEndDateEdit->setDate( QDate() );
	modifiedStartDateEdit->setDate( QDate() );
	modifiedEndDateEdit->setDate( QDate() );
	accessedStartDateEdit->setDate( QDate() );
	accessedEndDateEdit->setDate( QDate() );

	servingsSpinBox->setValue( 1 );
	prepTimeEdit->setTime( QTime(0,0) );

	enablePrepTimeCheckBox->setChecked(false);
	enableServingsCheckBox->setChecked(false);

	requireAllTitle->setChecked(false);
	requireAllInstructions->setChecked(false);

	ratingAvgRadioButton->setChecked(true);
	activateRatingOption(0);
	avgStarsEdit->clear();
	criteriaListView->clear();
	starsWidget->clear();
}

void AdvancedSearchDialog::activateRatingOption( int button_id )
{
	switch ( button_id ) {
	case 0:
		criterionFrame->setEnabled( false );
		ratingAvgFrame->setEnabled( true );
		break;
	case 1:
		criterionFrame->setEnabled( true );
		ratingAvgFrame->setEnabled( false );
		break;
	default:
		break;
	}
}

void AdvancedSearchDialog::buttonSwitched()
{
	const QObject *sent = sender();

	if ( sent->inherits("QPushButton") ) {
		QPushButton *pushed = (QPushButton*) sent;

		//KDE4 port
		QString suffix = ( pushed->isChecked() ) ? " <<" : " >>";
		pushed->setText( pushed->text().left( pushed->text().length() - 3 ) + suffix );
	}
}

void AdvancedSearchDialog::search()
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	//we need to load more than just the title because we'll be doing further refining of the search
	int load_items = RecipeDB::Title | RecipeDB::NamesOnly | RecipeDB::Noatime;
	if ( !authorsAllEdit->text().isEmpty() || !authorsWithoutEdit->text().isEmpty() )
		load_items |= RecipeDB::Authors;
	if ( !ingredientsAllEdit->text().isEmpty() || !ingredientsWithoutEdit->text().isEmpty() )
		load_items |= RecipeDB::Ingredients;
	if ( !categoriesAllEdit->text().isEmpty() || !categoriesNotEdit->text().isEmpty() )
		load_items |= RecipeDB::Categories;
	if ( (ratingAvgRadioButton->isChecked() && !avgStarsEdit->isEmpty()) || (criterionRadioButton->isChecked() && criteriaListView->firstChild()) )
		load_items |= RecipeDB::Ratings;

	RecipeSearchParameters parameters;

	parameters.titleKeywords = split(titleEdit->text(),true);
	parameters.requireAllTitleWords = requireAllTitle->isChecked();

	parameters.instructionsKeywords = split(instructionsEdit->text(),true);
	parameters.requireAllInstructionsWords = requireAllInstructions->isChecked();

	parameters.ingsOr = split(ingredientsAnyEdit->text(),true);
	parameters.catsOr = split(categoriesAnyEdit->text(),true);
	parameters.authorsOr = split(authorsAnyEdit->text(),true);

	if ( enablePrepTimeCheckBox->isChecked() )
		parameters.prep_time = prepTimeEdit->time();
	parameters.prep_param = prepTimeComboBox->currentItem();

	if ( enableServingsCheckBox->isChecked() )
		parameters.servings = servingsSpinBox->value();
	parameters.servings_param = servingsComboBox->currentItem();

	parameters.createdDateBegin = QDateTime(createdStartDateEdit->date());
	parameters.createdDateEnd = QDateTime(createdEndDateEdit->date());
	if ( parameters.createdDateEnd.date().isValid() )
		parameters.createdDateEnd = parameters.createdDateEnd.addDays(1); //we want to include the given day in the search

	parameters.modifiedDateBegin = QDateTime(modifiedStartDateEdit->date());
	parameters.modifiedDateEnd = QDateTime(modifiedEndDateEdit->date());
	if ( parameters.modifiedDateEnd.date().isValid() )
		parameters.modifiedDateEnd = parameters.modifiedDateEnd.addDays(1); //we want to include the given day in the search

	parameters.accessedDateBegin = QDateTime(accessedStartDateEdit->date());
	parameters.accessedDateEnd = QDateTime(accessedEndDateEdit->date());
	if ( parameters.accessedDateEnd.date().isValid() )
		parameters.accessedDateEnd = parameters.accessedDateEnd.addDays(1); //we want to include the given day in the search

	START_TIMER("Doing database SQL search");
	RecipeList allRecipes;
	database->search( &allRecipes, load_items, parameters );
	END_TIMER();

	/*
	 * Ideally, would be done by the above SQL query, but I have no idea how to accomplish this.
	 */

	START_TIMER("Further narrowing the search (no SQL)");
	QStringList items = split(authorsAllEdit->text());
	for ( QStringList::const_iterator author_it = items.begin(); author_it != items.end(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( QRegExp(*author_it,false, true) ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(authorsWithoutEdit->text());
	for ( QStringList::const_iterator author_it = items.begin(); author_it != items.end(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( QRegExp(*author_it,false,true) ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by categories
	items = split(categoriesAllEdit->text());
	for ( QStringList::const_iterator cat_it = items.begin(); cat_it != items.end(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( QRegExp(*cat_it,false,true) ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(categoriesNotEdit->text());
	for ( QStringList::const_iterator cat_it = items.begin(); cat_it != items.end(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( QRegExp(*cat_it,false,true) ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by ingredients
	items = split(ingredientsAllEdit->text());
	for ( QStringList::const_iterator ing_it = items.begin(); ing_it != items.end(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( QRegExp(*ing_it,false,true) ).ingredientID == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(ingredientsWithoutEdit->text());
	for ( QStringList::const_iterator ing_it = items.begin(); ing_it != items.end(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( QRegExp(*ing_it,false,true) ).ingredientID != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	if ( ratingAvgRadioButton->isChecked() && !avgStarsEdit->isEmpty() ) {
		for ( RecipeList::iterator recipe_it = allRecipes.begin(); recipe_it != allRecipes.end(); ++recipe_it ) {
			double sum = 0;
			int count = 0;
	
			for ( RatingList::iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
				sum += (*rating_it).average();
				++count;
			}
	
			if ( count != 0 ) {
				double average = sum/count;

				double stars;
				double stars_offset;
				avgStarsEdit->value(stars,stars_offset);
                                if ( stars_offset < 1e-10 ) { //if an exact amount is given, search for an amount within 0.5 of what is given
                                        //we can get negatives here, but it really doesn't matter
                                        stars = stars-0.5;
                                        stars_offset = 1.0;
                                }


				kDebug()<<"average for "<<(*recipe_it).title<<" "<<average<<endl;
				if ( average < stars || average > stars + stars_offset ) {
					recipe_it = allRecipes.remove( recipe_it );
					recipe_it--;
				}
			}
			else {
				recipe_it = allRecipes.remove( recipe_it );
				recipe_it--;
			}
		}
	}

	//TODO: Clean this up and/or do it more efficiently
	if ( criterionRadioButton->isChecked() && criteriaListView->firstChild() ) {
		for ( RecipeList::iterator recipe_it = allRecipes.begin(); recipe_it != allRecipes.end(); ++recipe_it ) {
			QMap< int, double > idSumMap;
			QMap< int, int > idCountMap;

			for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
				for ( RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin(); rc_it != (*rating_it).ratingCriteriaList.end(); ++rc_it ) {
					QMap< int, double >::iterator sum_it = idSumMap.find((*rc_it).id);
					if ( sum_it == idSumMap.end() )
						sum_it = idSumMap.insert((*rc_it).id,0);
					(*sum_it) += (*rc_it).stars;

					QMap< int, int >::iterator count_it = idCountMap.find((*rc_it).id);
					if ( count_it == idCountMap.end() )
						count_it = idCountMap.insert((*rc_it).id,0);
					(*count_it)++;
				}
			}

			for ( Q3ListViewItem *item = criteriaListView->firstChild(); item; item = item->nextSibling() ) {
				Ingredient i; i.setAmount( item->text(1) );
				double stars = i.amount;
				double stars_offset = i.amount_offset;

				if ( stars_offset < 1e-10 ) { //if an exact amount is given, search for an amount within 0.5 of what is given
					//we can get negatives here, but it really doesn't matter
					stars = stars-0.5;
					stars_offset = 1.0;
				}

				int id = item->text(2).toInt();

				QMap< int, double >::iterator sum_it = idSumMap.find(id);
				if ( sum_it != idSumMap.end() ) {
					QMap< int, int >::iterator count_it = idCountMap.find(id);
					double average = (*sum_it)/(*count_it);

					if ( average < stars || average > stars + stars_offset ) {
						recipe_it = allRecipes.remove( recipe_it );
						recipe_it--;
						break;
					}
				}
				else {
					recipe_it = allRecipes.remove( recipe_it );
					recipe_it--;
					break;
				}
			}
		}
	}
	END_TIMER();


	//now display the recipes left
	resultsListView->clear();
	for ( RecipeList::const_iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
		( void ) new RecipeListItem( resultsListView, *it );
	}

	if ( !resultsListView->firstChild() ) {
		( void ) new Q3ListViewItem( resultsListView, "--- "+i18n("No matching recipes found")+" ---");
	}

	KApplication::restoreOverrideCursor();
}

QStringList AdvancedSearchDialog::split( const QString &text, bool sql_wildcards ) const
{
	QStringList result;

	// To keep quoted words together, first split on quotes,
	// and then split again on the even numbered items
	
	QStringList temp = QStringList::split('"',text,true);
	for ( uint i = 0; i < temp.count(); ++i ) {
		if ( i & 1 ) //odd
			result += temp[i].trimmed();
		else         //even
			result += QStringList::split(' ',temp[i]);
	}

	if ( sql_wildcards ) {
		for ( QStringList::iterator it = result.begin(); it != result.end(); ++it ) {
			(*it).replace("%","\\%");
			(*it).replace("_","\\_");

			(*it).replace("*","%");
			(*it).replace("?","_");
		}
	}

	return result;
}

void AdvancedSearchDialog::slotAddRatingCriteria()
{
	Q3ListViewItem * it = new Q3ListViewItem(criteriaListView,criteriaComboBox->currentText());

	MixedNumber stars;
	double stars_offset;
	starsWidget->value(stars,stars_offset);
	QString stars_str = stars.toString();
	if ( stars_offset > 0 )
		stars_str += "-"+MixedNumber( stars + stars_offset ).toString();
	else if ( stars.toDouble() <= 1e-10 )
		stars_str = "";

	it->setText(1,stars_str);
	it->setText(2,QString::number(criteriaComboBox->criteriaID(criteriaComboBox->currentItem())));
}

void AdvancedSearchDialog::slotRemoveRatingCriteria()
{
	delete criteriaListView->selectedItem();
}

#include "advancedsearchdialog.moc"
