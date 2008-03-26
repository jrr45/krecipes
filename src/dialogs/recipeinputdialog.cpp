/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn (jkivlighn@gmail.com)               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeinputdialog.h"

#include <qstring.h>
#include <qlayout.h>


#include <qimage.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <qdatetimeedit.h>
#include <q3dragobject.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <q3widgetstack.h>
#include <qpainter.h>
#include <q3textbrowser.h>
//Added by qt3to4:
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QGridLayout>
#include <Q3ValueList>
#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QDropEvent>

#include <kapplication.h>
#include <kcompletionbox.h>
#include <sonnet/speller.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kled.h>
#include <kdialog.h>
#include <kio/netaccess.h>
#include <K3Spell>
#include <K3SpellConfig>
#include <kvbox.h>

#include "selectauthorsdialog.h"
#include "resizerecipedialog.h"
#include "ingredientparserdialog.h"
#include "editratingdialog.h"
#include "createunitdialog.h"
#include "datablocks/recipe.h"
#include "datablocks/categorytree.h"
#include "datablocks/unit.h"
#include "datablocks/weight.h"
#include "backends/recipedb.h"
#include "selectcategoriesdialog.h"
#include "createunitconversiondialog.h"
#include "editpropertiesdialog.h"
#include "widgets/fractioninput.h"
#include "widgets/kretextedit.h"
#include "widgets/inglistviewitem.h"
#include "widgets/kwidgetlistbox.h"
#include "widgets/ingredientinputwidget.h"
#include "image.h" //Initializes default photo

#include "profiling.h"

typedef enum ColorStatus { GreenStatus, RedStatus, YellowStatus };

ClickableLed::ClickableLed( QWidget *parent ) : KLed(parent)
{
}

void ClickableLed::mouseReleaseEvent( QMouseEvent* )
{
	emit clicked();
}

ImageDropLabel::ImageDropLabel( QWidget *parent, QPixmap &_sourcePhoto ) : QLabel( parent ),
		sourcePhoto( _sourcePhoto )
{
	setAcceptDrops( true );
}

void ImageDropLabel::dragEnterEvent( QDragEnterEvent* event )
{
   if ( event->mimeData()->hasImage() )
        event->acceptProposedAction();
}

void ImageDropLabel::dropEvent( QDropEvent* event )
{
	QImage image;

	if ( event->mimeData()->hasImage() ) {
		if ( ( image.width() > width() || image.height() > height() ) || ( image.width() < width() && image.height() < height() ) ) {
			QPixmap pm_scaled;
			pm_scaled.fromImage( image.scaled( QSize( width(), height()), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
			setPixmap( pm_scaled );

			sourcePhoto = pm_scaled; // to save scaled later on
		}
		else {
			setPixmap( QPixmap::fromImage( image ) );
			sourcePhoto = QPixmap::fromImage( image );
		}

		emit changed();
	}
}


RecipeInputDialog::RecipeInputDialog( QWidget* parent, RecipeDB *db ) : KVBox( parent )
{

	// Adjust internal parameters
	loadedRecipe = new Recipe();
	loadedRecipe->recipeID = -1; // No loaded recipe initially
	loadedRecipe->title = QString::null;
	loadedRecipe->instructions = QString::null;
	database = db;

	KIconLoader *il = KIconLoader::global();

	// Tabs
	tabWidget = new QTabWidget( this );
   setObjectName( "tabWidget" );
	tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );


	//------- Recipe Tab -----------------
	// Recipe Photo

	recipeTab = new Q3GroupBox( tabWidget );
	recipeTab->setFrameStyle( QFrame::NoFrame );
	recipeTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );


	// Design the Dialog
	QGridLayout* recipeLayout = new QGridLayout( recipeTab );
   recipeLayout->cellRect( 1, 1 );
   recipeLayout->setMargin( 0 );
   recipeLayout->setSpacing( 0 );

	// Border
	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	recipeLayout->addItem( spacer_left, 1, 0 );
	QSpacerItem* spacer_right = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	recipeLayout->addItem( spacer_right, 1, 8 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum , QSizePolicy::Fixed );
	recipeLayout->addItem( spacer_top, 0, 1 );
	QSpacerItem* spacer_bottom = new QSpacerItem( 10, 10, QSizePolicy::Minimum , QSizePolicy::MinimumExpanding );
	recipeLayout->addItem( spacer_bottom, 8, 1 );


	QPixmap image1( defaultPhoto );

	photoLabel = new ImageDropLabel( recipeTab, sourcePhoto );
	photoLabel->setPixmap( image1 );
	photoLabel->setFixedSize( QSize( 221, 166 ) );
	photoLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	photoLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	recipeLayout->addWidget( photoLabel, 3, 1, 5, 1, 0 );

	KVBox *photoButtonsBox = new KVBox( recipeTab );

	changePhotoButton = new QPushButton( photoButtonsBox );
	changePhotoButton->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored ) );
	changePhotoButton->setText( "..." );
	changePhotoButton->setToolTip( i18n( "Select photo" ) );

	QPushButton *clearPhotoButton = new QPushButton( photoButtonsBox );
	clearPhotoButton->setIcon( QIcon( il->loadIcon( "clear_left", KIconLoader::NoGroup, 16 ) ) );
	clearPhotoButton->setIconSize(  il->loadIcon( "clear_left", KIconLoader::NoGroup, 16 ).size() );
	clearPhotoButton->setToolTip( i18n( "Clear photo" ) );

	recipeLayout->addWidget( photoButtonsBox, 3, 2, 5, 1, 0 );


	//Title->photo spacer
	QSpacerItem* title_photo = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	recipeLayout->addItem( title_photo, 2, 3 );


	// Title
	KVBox *titleBox = new KVBox( recipeTab );
	titleBox->setSpacing( 5 );
	titleLabel = new QLabel( i18n( "Recipe Name" ), titleBox );
	titleEdit = new KLineEdit( titleBox );
	titleEdit->setMinimumSize( QSize( 360, 30 ) );
	titleEdit->setMaximumSize( QSize( 10000, 30 ) );
	titleEdit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
	recipeLayout->addWidget( titleBox, 1, 1, 1, 7, 0 );


	// Photo ->author spacer
	QSpacerItem* title_spacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	recipeLayout->addItem( title_spacer, 2, 1 );

	// Author(s) & Categories
	KVBox *authorBox = new KVBox( recipeTab ); // contains label and authorInput (input widgets)
	authorBox->setSpacing( 5 );
	recipeLayout->addWidget( authorBox, 3, 4 );
	authorLabel = new QLabel( i18n( "Authors" ), authorBox );
	KHBox *authorInput = new KHBox( authorBox ); // Contains input + button


	authorShow = new KLineEdit( authorInput );
	authorShow->setReadOnly( true );
	authorShow->setMinimumSize( QSize( 100, 20 ) );
	authorShow->setMaximumSize( QSize( 10000, 20 ) );
	authorShow->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );


	addAuthorButton = new QPushButton( authorInput );
	addAuthorButton->setText( "+" );
	addAuthorButton->setFixedSize( QSize( 20, 20 ) );
	addAuthorButton->setFlat( true );


	QSpacerItem* author_category = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	recipeLayout->addItem( author_category, 3, 5 );

	KVBox *categoryBox = new KVBox( recipeTab ); // Contains the label and categoryInput (input widgets)
	categoryBox->setSpacing( 5 );
	categoryLabel = new QLabel( i18n( "Categories" ), categoryBox );
	KHBox *categoryInput = new KHBox( categoryBox ); // Contains the input widgets

	categoryShow = new KLineEdit( categoryInput );
	categoryShow->setReadOnly( true );
	categoryShow->setMinimumSize( QSize( 100, 20 ) );
	categoryShow->setMaximumSize( QSize( 10000, 20 ) );
	categoryShow->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
	recipeLayout->addWidget( categoryBox, 4, 4 );

	addCategoryButton = new QPushButton( categoryInput );
	addCategoryButton->setText( "+" );
	addCategoryButton->setFixedSize( QSize( 20, 20 ) );
	addCategoryButton->setFlat( true );

	//Category ->Servings spacer
	QSpacerItem* category_yield = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	recipeLayout->addItem( category_yield, 5, 4 );

	KHBox *serv_prep_box = new KHBox( recipeTab );
	serv_prep_box->setSpacing( 5 );

	// Backup options
	Q3GroupBox *yieldGBox = new Q3GroupBox( serv_prep_box, "yieldGBox" );
	yieldGBox->setTitle( i18n( "Yield" ) );
	yieldGBox->setColumns( 2 );

	yieldLabel = new QLabel( i18n( "Amount" ), yieldGBox );
	/*QLabel *yieldTypeLabel = */new QLabel( i18n( "Type" ), yieldGBox );
	yieldNumInput = new FractionInput( yieldGBox );
	yieldNumInput->setAllowRange(true);
	yieldTypeEdit = new KLineEdit( yieldGBox );

	KVBox *prepTimeBox = new KVBox( serv_prep_box );
	prepTimeBox->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	prepTimeBox->setSpacing( 5 );

	( void ) new QLabel( i18n( "Preparation Time" ), prepTimeBox );
	prepTimeEdit = new QDateTimeEdit( prepTimeBox );
	prepTimeEdit->setMinimumTime( QTime( 0, 0 ) );
   prepTimeEdit->setDisplayFormat( "hh:mm");

	recipeLayout->addWidget( serv_prep_box, 6, 4 );

	//------- END OF Recipe Tab ---------------

	//------- Ingredients Tab -----------------

	ingredientGBox = new Q3GroupBox( recipeTab );
	ingredientGBox->setFrameStyle( QFrame::NoFrame );
	ingredientGBox->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	QGridLayout* ingredientsLayout = new QGridLayout( ingredientGBox );

	// Border
	QSpacerItem* spacerBoxLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	ingredientsLayout->addItem( spacerBoxLeft, 1, 0 );
	QSpacerItem* spacerBoxTop = new QSpacerItem( 10, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerBoxTop, 0, 1 );

	//Input Widgets
	ingInput = new IngredientInputWidget( database, ingredientGBox );
	ingredientsLayout->addWidget( ingInput, 1, 1, 1, 5, 0 );

	// Spacers to list and buttons
	QSpacerItem* spacerToList = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerToList, 2, 1 );
	QSpacerItem* spacerToButtons = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	ingredientsLayout->addItem( spacerToButtons, 3, 4 );

	// Add, Up,down,... buttons

	addButton = new KPushButton( ingredientGBox );
	addButton->setFixedSize( QSize( 31, 31 ) );
	addButton->setFlat( true );
	addButton->setIcon(KIcon( "add_ingredient" ) );
	addButton->setIconSize( QSize( 16, 16 ) );
	addButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( addButton, 3, 5 );

	// Spacer to the rest of buttons
	QSpacerItem* spacerToOtherButtons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerToOtherButtons, 4, 5 );

	upButton = new KPushButton( ingredientGBox );
	upButton->setFixedSize( QSize( 31, 31 ) );
	upButton->setFlat( true );
	upButton->setIcon( KIcon( "go-up" ) );
	upButton->setIconSize( QSize( 16, 16 ) );
	upButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( upButton, 5, 5 );

	downButton = new KPushButton( ingredientGBox );
	downButton->setFixedSize( QSize( 31, 31 ) );
	downButton->setFlat( true );
	downButton->setIcon( KIcon( "go-down" ) );
	downButton->setIconSize(  QSize( 16, 16 ) );
	downButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( downButton, 6, 5 );

	removeButton = new KPushButton( ingredientGBox );
	removeButton->setFixedSize( QSize( 31, 31 ) );
	removeButton->setFlat( true );
	removeButton->setIcon( KIcon( "list-remove" ) );
	removeButton->setIconSize( QSize( 16, 16 )  );
	removeButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( removeButton, 7, 5 );

	ingParserButton = new KPushButton( ingredientGBox );
	ingParserButton->setFixedSize( QSize( 31, 31 ) );
	ingParserButton->setFlat( true );
	ingParserButton->setIcon( KIcon( "edit-paste" ) );
	ingParserButton->setIconSize( QSize( 16, 16 ) );
	ingParserButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( ingParserButton, 8, 5 );

		addButton->setToolTip( i18n( "Add ingredient" ) );
		upButton->setToolTip( i18n( "Move ingredient up" ) );
		downButton->setToolTip( i18n( "Move ingredient down" ) );
		removeButton->setToolTip( i18n( "Remove ingredient" ) );
		ingParserButton->setToolTip( i18n( "Paste Ingredients" ) );

	// Ingredient List
	ingredientList = new K3ListView( ingredientGBox);
	ingredientList->addColumn( i18n( "Ingredient" ) );
	ingredientList->addColumn( i18n( "Amount" ) );
	ingredientList->setColumnAlignment( 1, Qt::AlignHCenter );
	ingredientList->addColumn( i18n( "Units" ) );
	ingredientList->addColumn( i18n( "Preparation Method" ) );
	ingredientList->setSorting( -1 ); // Do not sort
	ingredientList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding ) );
	ingredientList->setItemsRenameable( true );
	ingredientList->setRenameable( 0, false ); //name
	ingredientList->setRenameable( 1, true ); //amount
	ingredientList->setRenameable( 2, true ); //units
	ingredientList->setRenameable( 3, true ); //prep method
	ingredientList->setDefaultRenameAction( Q3ListView::Reject );
	ingredientsLayout->addWidget( ingredientList, 3, 1, 7, 3, 0 );

	QHBoxLayout *propertyStatusLayout = new QHBoxLayout( ingredientGBox );
   propertyStatusLayout->setMargin( 0 );
   propertyStatusLayout->setSpacing( 5 );

	QLabel *propertyLabel = new QLabel( i18n("Property Status:"), ingredientGBox );
	propertyStatusLabel = new QLabel( ingredientGBox );
	propertyStatusLed = new ClickableLed( ingredientGBox );
	propertyStatusLed->setFixedSize( QSize(16,16) );
	propertyStatusButton = new QPushButton( i18n("Details..."), ingredientGBox );
	//QPushButton *propertyUpdateButton = new QPushButton( i18n("Update"), ingredientGBox );
	propertyStatusLayout->addWidget( propertyLabel );
	propertyStatusLayout->addWidget( propertyStatusLabel );
	propertyStatusLayout->addWidget( propertyStatusLed );
	propertyStatusLayout->addWidget( propertyStatusButton );
	//propertyStatusLayout->addWidget( propertyUpdateButton );
	QSpacerItem* propertySpacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	propertyStatusLayout->addItem( propertySpacerRight );

	KGuiItem updateGuiItem;
	updateGuiItem.setText( i18n("Update") );
	updateGuiItem.setIcon( KIcon( "view-refresh" ) );

	propertyStatusDialog = new KDialog( this );
	propertyStatusDialog->setObjectName( "propertyStatusDialog" );
	propertyStatusDialog->setModal( false );
	propertyStatusDialog->showButtonSeparator( false );
	propertyStatusDialog->setCaption( i18n("Property details") );
	propertyStatusDialog->setButtons( KDialog::Close | KDialog::User1 | KDialog::Help );
	propertyStatusDialog->setDefaultButton( KDialog::Close );
	propertyStatusDialog->setButtonGuiItem( KDialog::User1, updateGuiItem );
	propertyStatusDialog->setHelp("property-status");

	statusTextView = new Q3TextBrowser(0);
	statusTextView->setReadOnly(true);
	propertyStatusDialog->setMainWidget( statusTextView );
	propertyStatusDialog->resize( 400, 300 );

	ingredientsLayout->addLayout( propertyStatusLayout, 10, 1, 1, 4, 0 );

	// ------- Recipe Instructions Tab -----------

	instructionsTab = new Q3GroupBox( recipeTab );
	instructionsTab->setFrameStyle( QFrame::NoFrame );
	instructionsTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QVBoxLayout *instructionsLayout = new QVBoxLayout( instructionsTab );

	instructionsEdit = new KreTextEdit( instructionsTab );
	instructionsEdit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	instructionsEdit->setTabChangesFocus ( true );
	instructionsLayout->addWidget( instructionsEdit );

	spellCheckButton = new QToolButton( instructionsTab );
	spellCheckButton->setIcon( KIcon( "tools-check-spelling" ) );
	spellCheckButton->setToolTip( i18n( "Check spelling" ) );
	instructionsLayout->addWidget( spellCheckButton );

	// ------- END OF Recipe Instructions Tab -----------


	// ------- Recipe Ratings Tab -----------

	KVBox *ratingsTab = new KVBox(recipeTab);
	ratingListDisplayWidget = new KWidgetListbox(ratingsTab);
	QPushButton *addRatingButton = new QPushButton(i18n("Add Rating..."),ratingsTab);

	connect( addRatingButton, SIGNAL(clicked()), this, SLOT(slotAddRating()) );

	// ------- END OF Recipe Ratings Tab -----------


	tabWidget->insertTab( -1, recipeTab, i18n( "Recipe" ) );
	tabWidget->insertTab( -1, ingredientGBox, i18n( "Ingredients" ) );
	tabWidget->insertTab( -1, instructionsTab, i18n( "Instructions" ) );
	tabWidget->insertTab( -1, ratingsTab, i18n( "Ratings" ) );


	// Functions Box
	KHBox* functionsLayout = new KHBox( this );

	functionsBox = new Q3GroupBox( 1, Qt::Vertical, functionsLayout );
	functionsBox->setFrameStyle( QFrame::NoFrame );

	saveButton = new QToolButton( functionsBox );
	saveButton->setIcon( KIcon( "document-save" ) );
	saveButton->setEnabled( false );
	showButton = new QToolButton( functionsBox );
	showButton->setIcon( KIcon( "zoom-original" ) );
	closeButton = new QToolButton( functionsBox );
	closeButton->setIcon( KIcon( "window-close" ) );
	resizeButton = new QToolButton( functionsBox );
	resizeButton->setIcon( KIcon( "arrow-up-double" ) ); //TODO: give me an icon :)

	saveButton->setText( i18n( "Save recipe" ) );
	saveButton->setToolTip( i18n( "Save recipe" ) );
	saveButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	showButton->setText( i18n( "Show recipe" ) );
	showButton->setToolTip( i18n( "Show recipe" ) );
	showButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	closeButton->setText( i18n( "Close" ) );
	closeButton->setToolTip( i18n( "Close" ) );
	closeButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	resizeButton->setText( i18n( "Resize recipe" ) );
	resizeButton->setToolTip( i18n( "Resize recipe" ) );
	resizeButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

	functionsLayout->layout() ->addItem( new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	// Dialog design
	tabWidget->resize( size().expandedTo( minimumSizeHint() ) );
	//KDE4 port
	//clearWState( WState_Polished );

	// Initialize internal data
	unsavedChanges = false; // Indicates if there's something not saved yet.
	enableChangedSignal(); // Enables the signal "changed()"

	// Connect signals & Slots
	connect( changePhotoButton, SIGNAL( clicked() ), this, SLOT( changePhoto() ) );
	connect( clearPhotoButton, SIGNAL( clicked() ), SLOT( clearPhoto() ) );
	connect( upButton, SIGNAL( clicked() ), this, SLOT( moveIngredientUp() ) );
	connect( downButton, SIGNAL( clicked() ), this, SLOT( moveIngredientDown() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeIngredient() ) );
	connect( addButton, SIGNAL( clicked() ), ingInput, SLOT( addIngredient() ) );
	connect( ingParserButton, SIGNAL( clicked() ), this, SLOT( slotIngredientParser() ) );
	connect( photoLabel, SIGNAL( changed() ), this, SIGNAL( changed() ) );
	connect( this, SIGNAL( changed() ), this, SLOT( recipeChanged() ) );
	connect( yieldNumInput, SIGNAL( textChanged( const QString & ) ), this, SLOT( recipeChanged() ) );
	connect( yieldTypeEdit, SIGNAL( textChanged( const QString & ) ), this, SLOT( recipeChanged() ) );
	connect( prepTimeEdit, SIGNAL( valueChanged( const QTime & ) ), SLOT( recipeChanged() ) );
	connect( titleEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( recipeChanged( const QString& ) ) );
	connect( instructionsEdit, SIGNAL( textChanged() ), this, SLOT( recipeChanged() ) );
	connect( addCategoryButton, SIGNAL( clicked() ), this, SLOT( addCategory() ) );
	connect( addAuthorButton, SIGNAL( clicked() ), this, SLOT( addAuthor() ) );
	connect( titleEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( prepTitleChanged( const QString& ) ) );
	connect( ingredientList, SIGNAL( itemRenamed( Q3ListViewItem*, const QString &, int ) ), SLOT( syncListView( Q3ListViewItem*, const QString &, int ) ) );

	connect ( ingInput, SIGNAL( ingredientEntered(const Ingredient&) ), this, SLOT( addIngredient(const Ingredient&) ) );
	connect ( ingInput, SIGNAL( headerEntered(const Element&) ), this, SLOT( addIngredientHeader(const Element&) ) );

	connect( propertyStatusLed, SIGNAL(clicked()), SLOT(updatePropertyStatus()) );
	connect( propertyStatusDialog, SIGNAL(user1Clicked()), SLOT(updatePropertyStatus()) );
	connect( propertyStatusButton, SIGNAL(clicked()), propertyStatusDialog, SLOT(show()) );
	connect( statusTextView, SIGNAL(linkClicked(const QString&)), this, SLOT( statusLinkClicked(const QString &) ) );

	// Function buttons
	connect ( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
	connect ( closeButton, SIGNAL( clicked() ), this, SLOT( closeOptions() ) );
	connect ( showButton, SIGNAL( clicked() ), this, SLOT( showRecipe() ) );
	connect ( resizeButton, SIGNAL( clicked() ), this, SLOT( resizeRecipe() ) );
	connect ( spellCheckButton, SIGNAL( clicked() ), this, SLOT( spellCheck() ) );
	connect ( this, SIGNAL( enableSaveOption( bool ) ), this, SLOT( enableSaveButton( bool ) ) );

	connect ( database, SIGNAL( recipeRemoved(int) ), this, SLOT( recipeRemoved(int) ) );

	//FIXME: We've got some sort of build issue... we get undefined references to CreateUnitDialog without this dummy code here
	CreateUnitDialog d( this, "" );
}


RecipeInputDialog::~RecipeInputDialog()
{
	delete loadedRecipe;
}

void RecipeInputDialog::recipeRemoved( int id )
{
	if ( loadedRecipe->recipeID == id ) {
		loadedRecipe->recipeID = -1;
		recipeChanged();
	}
}

void RecipeInputDialog::prepTitleChanged( const QString &title )
{
	//we don't want the menu to grow due to a long title
	//### KStringHandler::rsqueeze does this but I can't remember when it was added (compatibility issue...)
	QString short_title = title.left( 20 );
	if ( title.length() > 20 )
		short_title.append( "..." );

	emit titleChanged( short_title );
}

int RecipeInputDialog::loadedRecipeID() const
{
	return loadedRecipe->recipeID;
}

void RecipeInputDialog::loadRecipe( int recipeID )
{
	emit enableSaveOption( false );
	unsavedChanges = false;

	//Disable changed() signals
	enableChangedSignal( false );

	//Empty current recipe
	loadedRecipe->empty();

	//Set back to the first page
	tabWidget->setCurrentIndex( 0 );

	// Load specified Recipe ID
	database->loadRecipe( loadedRecipe, RecipeDB::All ^ RecipeDB::Meta ^ RecipeDB::Properties, recipeID );

	reload();

	propertyStatusDialog->hide();
	updatePropertyStatus();

	//Enable changed() signals
	enableChangedSignal();

}

void RecipeInputDialog::reload( void )
{
	yieldNumInput->setValue( 1, 0 );
	yieldTypeEdit->setText("");
	ingredientList->clear();
	ratingListDisplayWidget->clear();
	ingInput->clear();

	//Load Values in Interface
	titleEdit->setText( loadedRecipe->title );
	instructionsEdit->clearCompletionItems();
	instructionsEdit->setText( loadedRecipe->instructions );
	yieldNumInput->setValue( loadedRecipe->yield.amount, loadedRecipe->yield.amount_offset );
	yieldTypeEdit->setText( loadedRecipe->yield.type );
	prepTimeEdit->setTime( loadedRecipe->prepTime );

	//show ingredient list
	IngredientList list_copy = loadedRecipe->ingList;
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
		Q3ListViewItem * lastElement = ingredientList->lastItem();
		Q3ListViewItem *ing_header = 0;

		QString group = group_list[ 0 ].group;
		if ( !group.isEmpty() ) {
			if ( lastElement && lastElement->parent() )
				lastElement = lastElement->parent();

			ing_header = new IngGrpListViewItem( ingredientList, lastElement, group_list[ 0 ].group, group_list[ 0 ].groupID );
			ing_header->setOpen( true );
			lastElement = ing_header;
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			//Insert ingredient after last one
			if ( ing_header ) {
				lastElement = new IngListViewItem ( ing_header, lastElement, *ing_it );
			}
			else {
				if ( lastElement && lastElement->parent() )
					lastElement = lastElement->parent();
				lastElement = new IngListViewItem ( ingredientList, lastElement, *ing_it );
			}

			for ( Q3ValueList<IngredientData>::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ++sub_it ) {
				new IngSubListViewItem ( lastElement, *sub_it );
				lastElement->setOpen(true);
			}

			//update completion
			instructionsEdit->addCompletionItem( ( *ing_it ).name );
		}
	}
	//
	//show photo
	if ( !loadedRecipe->photo.isNull() ) {

		//     		//get the photo
		sourcePhoto = loadedRecipe->photo;

		if ( ( sourcePhoto.width() > photoLabel->width() || sourcePhoto.height() > photoLabel->height() ) || ( sourcePhoto.width() < photoLabel->width() && sourcePhoto.height() < photoLabel->height() ) ) {
			QImage pm = sourcePhoto.toImage();
			QPixmap pm_scaled;
			pm_scaled.fromImage( pm.scaled( QSize( photoLabel->width(), photoLabel->height() ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
			photoLabel->setPixmap( pm_scaled );

			sourcePhoto = pm_scaled; // to save scaled later on
		}
		else {
			photoLabel->setPixmap( sourcePhoto );
		}
	}
	else {
		QPixmap photo = QPixmap( defaultPhoto );
		photoLabel->setPixmap( photo );
		sourcePhoto = QPixmap();
	}


	// Show categories
	showCategories();

	// Show authors
	showAuthors();

	// Show ratings
	for ( RatingList::iterator rating_it = loadedRecipe->ratingList.begin(); rating_it != loadedRecipe->ratingList.end(); ++rating_it ) {
		RatingDisplayWidget *item = new RatingDisplayWidget( this );
		item->rating_it = rating_it;
		addRating(*rating_it,item);
		ratingListDisplayWidget->insertItem(item);
	}
	ratingListDisplayWidget->ensureCellVisible(0,0);

	// Update yield type auto completion
	KCompletion *completion = yieldTypeEdit->completionObject();
	completion->clear();
	ElementList yieldList;
	database->loadYieldTypes( &yieldList );
	for ( ElementList::const_iterator it = yieldList.begin(); it != yieldList.end(); ++it ) {
		completion->addItem( (*it).name );
	}
}

void RecipeInputDialog::changePhoto( void )
{
	// standard filedialog
	KUrl url = KFileDialog::getOpenUrl( KUrl() , QString( "*.png *.jpg *.jpeg *.xpm *.gif|%1 (*.png *.jpg *.jpeg *.xpm *.gif)" ).arg( i18n( "Images" ) ), this );
        if ( url.isEmpty() )
            return;
	QString filename;
	if (!url.isLocalFile()) {
		if (!KIO::NetAccess::download(url,filename,this)) {
			KMessageBox::error(this, KIO::NetAccess::lastErrorString() );
			return;
		}
	} else {
		filename = url.path();
	}

	QPixmap pixmap ( filename );
	if ( !( pixmap.isNull() ) ) {
		// If photo is bigger than the label, or smaller in width, than photoLabel, scale it
		sourcePhoto = pixmap;
		if ( ( sourcePhoto.width() > photoLabel->width() || sourcePhoto.height() > photoLabel->height() ) || ( sourcePhoto.width() < photoLabel->width() && sourcePhoto.height() < photoLabel->height() ) ) {
			QImage pm = sourcePhoto.toImage();
			QPixmap pm_scaled;
			pm_scaled.fromImage( pm.scaled( QSize( photoLabel->width(), photoLabel->height() ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
			photoLabel->setPixmap( pm_scaled );

			sourcePhoto = pm_scaled; // to save scaled later on
			photoLabel->setPixmap( pm_scaled );
		}
		else {
			photoLabel->setPixmap( sourcePhoto );
		}
		emit changed();
	}

	if (!url.isLocalFile()) {
		KIO::NetAccess::removeTempFile( filename );
	}
}

void RecipeInputDialog::clearPhoto( void )
{
	sourcePhoto = QPixmap();
	photoLabel->setPixmap( QPixmap( defaultPhoto ) );

	emit changed();
}

void RecipeInputDialog::moveIngredientUp( void )
{
	Q3ListViewItem * it = ingredientList->selectedItem();
	if ( !it || it->rtti() == INGSUBLISTVIEWITEM_RTTI )
		return ;

	Q3ListViewItem *iabove = it->itemAbove();
	while ( iabove && iabove->rtti() == INGSUBLISTVIEWITEM_RTTI )
		iabove = iabove->itemAbove();

	if ( iabove ) {
		if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
			if ( iabove->parent() )
				iabove = iabove->parent();

			int it_index = ingItemIndex( ingredientList, it );
			int iabove_index = ingItemIndex( ingredientList, iabove );

			iabove->moveItem( it ); //Move the Item

			loadedRecipe->ingList.move( iabove_index, ( iabove->rtti() == INGGRPLISTVIEWITEM_RTTI ) ? iabove->childCount() : 1, it_index + it->childCount() - 1 );
		}
		else {
			int it_index = ingItemIndex( ingredientList, it );
			int iabove_index = ingItemIndex( ingredientList, iabove );
			IngredientList::iterator ing = loadedRecipe->ingList.at( it_index );

			if ( iabove->parent() != it->parent() ) {
				if ( iabove->rtti() == INGGRPLISTVIEWITEM_RTTI && it->parent() ) { //move the item out of the group
					it->parent() ->takeItem( it );
					ingredientList->insertItem( it );
					it->moveItem( ( iabove->itemAbove() ->parent() ) ? iabove->itemAbove() ->parent() : iabove->itemAbove() ); //Move the Item
				}
				else { //move the item into the group
					ingredientList->takeItem( it );
					iabove->parent() ->insertItem( it );
					it->moveItem( iabove ); //Move the Item
				}

				ingredientList->setCurrentItem( it ); //Keep selected
			}
			else {
				iabove->moveItem( it ); //Move the Item
				loadedRecipe->ingList.move( it_index, iabove_index );
			}

			if ( it->parent() )
				( *ing ).groupID = ( ( IngGrpListViewItem* ) it->parent() ) ->id();
			else
				( *ing ).groupID = -1;
		}

		emit changed();
	}
}

void RecipeInputDialog::moveIngredientDown( void )
{
	Q3ListViewItem * it = ingredientList->selectedItem();
	if ( !it || it->rtti() == INGSUBLISTVIEWITEM_RTTI )
		return ;

	Q3ListViewItem *ibelow = it->itemBelow();
	while ( ibelow && ibelow->rtti() == INGSUBLISTVIEWITEM_RTTI )
		ibelow = ibelow->itemBelow();

	if ( ibelow ) {
		if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
			Q3ListViewItem * next_sibling = it->nextSibling();

			if ( next_sibling ) {
				int it_index = ingItemIndex( ingredientList, it );
				int ibelow_index = ingItemIndex( ingredientList, next_sibling );

				it->moveItem( next_sibling ); //Move the Item

				int skip = 0;
				if ( next_sibling->childCount() > 0 )
					skip = next_sibling->childCount() - 1;

				loadedRecipe->ingList.move( it_index, it->childCount(), ibelow_index + skip );
			}
		}
		else {
			int it_index = ingItemIndex( ingredientList, it );
			int ibelow_index = ingItemIndex( ingredientList, ibelow );
			IngredientList::iterator ing = loadedRecipe->ingList.at( it_index );

			if ( ibelow->rtti() == INGGRPLISTVIEWITEM_RTTI || ( ibelow->parent() != it->parent() ) ) {
				if ( ibelow->rtti() == INGGRPLISTVIEWITEM_RTTI && !it->parent() ) { //move the item into the group
					if ( !it->parent() )
						ingredientList->takeItem( it );
					else
						it->parent() ->takeItem( it );

					ibelow->insertItem( it );
				}
				else { //move the item out of the group
					Q3ListViewItem *parent = it->parent(); //store this because we can't get it after we do it->takeItem()
					parent->takeItem( it );
					ingredientList->insertItem( it );
					it->moveItem( parent ); //Move the Item
				}

				ingredientList->setCurrentItem( it ); //Keep selected
			}
			else {
				it->moveItem( ibelow ); //Move the Item
				loadedRecipe->ingList.move( it_index, ibelow_index );
			}

			if ( it->parent() )
				( *ing ).groupID = ( ( IngGrpListViewItem* ) it->parent() ) ->id();
			else
				( *ing ).groupID = -1;
		}

		emit changed();
	}
	else if ( it->parent() ) {
		it->parent() ->takeItem( it );
		ingredientList->insertItem( it );
		it->moveItem( ( ingredientList->lastItem() ->parent() ) ? ingredientList->lastItem() ->parent() : ingredientList->lastItem() ); //Move the Item
		ingredientList->setCurrentItem( it ); //Keep selected

		int it_index = ingItemIndex( ingredientList, it );
		IngredientList::iterator ing = loadedRecipe->ingList.at( it_index );
		( *ing ).groupID = -1;

		emit changed();
	}
}

void RecipeInputDialog::removeIngredient( void )
{
	Q3ListViewItem * it = ingredientList->selectedItem();
	if ( it && (it->rtti() == INGLISTVIEWITEM_RTTI || it->rtti() == INGSUBLISTVIEWITEM_RTTI) ) {
		Q3ListViewItem *iselect = it->itemBelow();
		while ( iselect && iselect->rtti() == INGSUBLISTVIEWITEM_RTTI )
			iselect = iselect->itemBelow();

		if ( !iselect ) {
			iselect = it->itemAbove();
			while ( iselect && iselect->rtti() == INGSUBLISTVIEWITEM_RTTI )
				iselect = iselect->itemAbove();
		}

		IngListViewItem *ing_item = (IngListViewItem*)it; //we can cast IngSubListViewItem to this too, it's a subclass

		IngredientData &ing = loadedRecipe->ingList.findSubstitute( ing_item->ingredient() );

		//Remove it from the instruction's completion
		instructionsEdit->removeCompletionItem( ing.name );

		loadedRecipe->ingList.removeSubstitute( ing );

		int ingID = ing_item->ingredient().ingredientID;
		QMap<int,QString>::iterator map_it;
		if ( (map_it = propertyStatusMapRed.find(ingID)) != propertyStatusMapRed.end() )
			propertyStatusMapRed.erase( map_it );
		else if ( (map_it = propertyStatusMapYellow.find(ingID)) != propertyStatusMapYellow.end() )
			propertyStatusMapYellow.erase( map_it );
		showStatusIndicator();

		//Now remove the ingredient
		it->setSelected( false );
		delete it;
		if ( iselect )
			ingredientList->setSelected( iselect, true ); // be careful iselect->setSelected doesn't work this way.

		emit changed();
	}
	else if ( it && it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
		IngGrpListViewItem * header = ( IngGrpListViewItem* ) it;

		for ( IngListViewItem * sub_item = (IngListViewItem*)header->firstChild(); sub_item; sub_item = (IngListViewItem*)sub_item->nextSibling() ) {
			IngredientData &ing = loadedRecipe->ingList.findSubstitute( sub_item->ingredient() );

			//Remove it from the instruction's completion
			instructionsEdit->removeCompletionItem( ing.name );

			loadedRecipe->ingList.removeSubstitute( ing );

			int ingID = sub_item->ingredient().ingredientID;
			QMap<int,QString>::iterator map_it;
			if ( (map_it = propertyStatusMapRed.find(ingID)) != propertyStatusMapRed.end() )
				propertyStatusMapRed.erase( map_it );
			else if ( (map_it = propertyStatusMapYellow.find(ingID)) != propertyStatusMapYellow.end() )
				propertyStatusMapYellow.erase( map_it );
			showStatusIndicator();
		}

		delete header;

		emit changed();
	}

}

int RecipeInputDialog::createNewYieldIfNecessary( const QString &yield )
{
	if ( yield.trimmed().isEmpty() )  //no yield
		return -1;
	else
	{
		int id = database->findExistingYieldTypeByName( yield );
		if ( id == -1 ) //creating new
		{
			database->createNewYieldType( yield );
			id = database->lastInsertID();
		}

		return id;
	}
}

void RecipeInputDialog::syncListView( Q3ListViewItem* it, const QString &new_text, int col )
{
	if ( it->rtti() != INGLISTVIEWITEM_RTTI && it->rtti() != INGSUBLISTVIEWITEM_RTTI )
		return ;

	IngListViewItem *ing_item = ( IngListViewItem* ) it;

	IngredientData &new_ing = loadedRecipe->ingList.findSubstitute( ing_item->ingredient() );

	switch ( col ) {
	case 1:  //amount
		{
			bool ok;

			Ingredient new_ing_amount;
			new_ing_amount.setAmount(new_text,&ok);

			if ( ok )
			{
				if ( new_ing.amount != new_ing_amount.amount ||
				     new_ing.amount_offset != new_ing_amount.amount_offset ) {
					new_ing.amount = new_ing_amount.amount;
					new_ing.amount_offset = new_ing_amount.amount_offset;
					if ( !new_text.isEmpty() )
						ing_item->setAmount( new_ing_amount.amount, new_ing_amount.amount_offset );

					new_ing.amount = new_ing_amount.amount;
					new_ing.amount_offset = new_ing_amount.amount_offset;
					emit changed();
				}
			}
			else
			{
				if ( !new_text.isEmpty() )
 					ing_item->setAmount( new_ing.amount, new_ing.amount_offset );
			}

			break;
		}
	case 2:  //unit
		{
			Unit old_unit = new_ing.units;

			if ( new_text.length() > int(database->maxUnitNameLength()) )
			{
				KMessageBox::error( this, i18n( "Unit name cannot be longer than %1 characters." , database->maxUnitNameLength() ) );
				ing_item->setUnit( old_unit );
				break;
			}

			QString approp_unit = new_ing.amount > 1 ? new_ing.units.plural : new_ing.units.name;
			if ( approp_unit != new_text.trimmed() )
			{
				Unit new_unit;
				int new_id = IngredientInputWidget::createNewUnitIfNecessary( new_text.trimmed(), new_ing.amount > 1, ing_item->ingredient().ingredientID, new_unit, database );

				if ( new_id != -1 ) {
					new_ing.units = new_unit;
					new_ing.units.id = new_id;

					ing_item->setUnit( new_ing.units );

					updatePropertyStatus();
					emit changed();
				}
				else {
					ing_item->setUnit( old_unit );
				}
			}
			break;
		}
	case 3:  //prep method
		{
			QString old_text = new_ing.prepMethodList.join(",");

			QStringList prepMethodList;
         if (new_text.isEmpty())
            prepMethodList = QStringList();
         else
            prepMethodList = new_text.split( ",", QString::SkipEmptyParts);

			for ( QStringList::const_iterator it = prepMethodList.begin(); it != prepMethodList.end(); ++it ) {
				if ( (*it).trimmed().length() > int(database->maxPrepMethodNameLength()) )
				{
					KMessageBox::error( this, i18n( "Preparation method cannot be longer than %1 characters." , database->maxPrepMethodNameLength() ) );
					ing_item->setPrepMethod( old_text );
					break;
				}
			}

			if ( old_text != new_text.trimmed() )
			{
				new_ing.prepMethodList = ElementList::split(",",new_text.trimmed());
				QList<int> new_ids = IngredientInputWidget::createNewPrepIfNecessary( new_ing.prepMethodList, database );

				QList<int>::const_iterator id_it = new_ids.begin();
				for ( ElementList::iterator it = new_ing.prepMethodList.begin(); it != new_ing.prepMethodList.end(); ++it, ++id_it ) {
					(*it).id = *id_it;
				}

				updatePropertyStatus();
				emit changed();
			}
			break;
		}
	}
}

void RecipeInputDialog::recipeChanged( void )
{
	if ( changedSignalEnabled ) {
		// Enable Save Button
		emit enableSaveOption( true );
		emit createButton( this, titleEdit->text() );
		unsavedChanges = true;

	}

}

void RecipeInputDialog::recipeChanged( const QString & /*t*/ )
{
	recipeChanged(); // jumps to the real slot function
}

void RecipeInputDialog::enableChangedSignal( bool en )
{
	changedSignalEnabled = en;
}

bool RecipeInputDialog::save ( void )
{
	//check bounds first
	if ( titleEdit->text().length() > int(database->maxRecipeTitleLength()) ) {
		KMessageBox::error( this, i18n( "Recipe title cannot be longer than %1 characters." , database->maxRecipeTitleLength() ), i18n( "Unable to save recipe" ) );
		return false;
	}

	emit enableSaveOption( false );
	saveRecipe();
	unsavedChanges = false;

	return true;
}

void RecipeInputDialog::saveRecipe( void )
{
	// Nothing except for the ingredient list (loadedRecipe->ingList)
	// was stored before for performance. (recipeID is already there)

	loadedRecipe->photo = sourcePhoto;
	loadedRecipe->instructions = instructionsEdit->text();
	loadedRecipe->title = titleEdit->text();
	yieldNumInput->value(loadedRecipe->yield.amount,loadedRecipe->yield.amount_offset);
	loadedRecipe->yield.type_id = createNewYieldIfNecessary(yieldTypeEdit->text());
	loadedRecipe->prepTime = prepTimeEdit->time();

	// Now save()
	kDebug() << "Saving..." ;
	database->saveRecipe( loadedRecipe );


}

void RecipeInputDialog::newRecipe( void )
{
	loadedRecipe->empty();
	QPixmap image( defaultPhoto );
	photoLabel->setPixmap( image );
	sourcePhoto = QPixmap();

	instructionsEdit->setText( i18n( "Write the recipe instructions here" ) );
	instructionsEdit->clearCompletionItems();
	titleEdit->setText( i18n( "Write the recipe title here" ) );
	ingredientList->clear();
	authorShow->clear();
	categoryShow->clear();
	yieldNumInput->setValue( 1, 0 );
	yieldTypeEdit->setText("");
	prepTimeEdit->setTime( QTime( 0, 0 ) );

	instructionsEdit->selectAll();

	ratingListDisplayWidget->clear();

	//Set back to the first page
	tabWidget->setCurrentIndex( 0 );

	ingInput->clear();

	//Set focus to the title
	titleEdit->setFocus();
	titleEdit->selectAll();

	//clear status info
	propertyStatusMapRed.clear();
	propertyStatusMapYellow.clear();
	showStatusIndicator();
}

bool RecipeInputDialog::everythingSaved()
{
	return ( !( unsavedChanges ) );
}

void RecipeInputDialog::addCategory( void )
{
	SelectCategoriesDialog *editCategoriesDialog = new SelectCategoriesDialog( this, loadedRecipe->categoryList, database );

	if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
		loadedRecipe->categoryList.clear();
		editCategoriesDialog->getSelectedCategories( &( loadedRecipe->categoryList ) ); // get the category list chosen
		emit( recipeChanged() ); //Indicate that the recipe changed

	}

	delete editCategoriesDialog;

	// show category list
	showCategories();


}

void RecipeInputDialog::showCategories( void )
{
	QString categories;
	for ( ElementList::const_iterator cat_it = loadedRecipe->categoryList.begin(); cat_it != loadedRecipe->categoryList.end(); ++cat_it ) {
		if ( !categories.isEmpty() )
			categories += ",";
		categories += ( *cat_it ).name;
	}
	categoryShow->setText( categories );
}

void RecipeInputDialog::addAuthor( void )
{
	SelectAuthorsDialog * editAuthorsDialog = new SelectAuthorsDialog( this, loadedRecipe->authorList, database );


	if ( editAuthorsDialog->exec() == QDialog::Accepted ) { // user presses Ok
		loadedRecipe->authorList.clear();
		editAuthorsDialog->getSelectedAuthors( &( loadedRecipe->authorList ) ); // get the category list chosen
		emit( recipeChanged() ); //Indicate that the recipe changed
	}

	delete editAuthorsDialog;

	// show authors list
	showAuthors();
}

void RecipeInputDialog::showAuthors( void )
{
	QString authors;
	for ( ElementList::const_iterator author_it = loadedRecipe->authorList.begin(); author_it != loadedRecipe->authorList.end(); ++author_it ) {
		if ( !authors.isEmpty() )
			authors += ",";
		authors += ( *author_it ).name;
	}
	authorShow->setText( authors );
}

void RecipeInputDialog::enableSaveButton( bool enabled )
{
	saveButton->setEnabled( enabled );
}

void RecipeInputDialog::closeOptions( void )
{

	// First check if there's anything unsaved in the recipe
	if ( unsavedChanges ) {

		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This recipe contains unsaved changes.\n" "Would you like to save it before closing?" ), i18n( "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return ;
		}

	}

	emit enableSaveOption( false );
	unsavedChanges = false;

	// Now close really
	emit closeRecipe();


}

void RecipeInputDialog::showRecipe( void )
{
	// First check if there's anything unsaved in the recipe

	if ( loadedRecipe->recipeID == -1 ) {
		switch ( KMessageBox::questionYesNo( this, i18n( "You need to save the recipe before displaying it. Would you like to save it now?" ), i18n( "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			return ;
		}
	}
	else if ( unsavedChanges ) {

		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This recipe has changes that will not be displayed unless the recipe is saved. Would you like to save it now?" ), i18n( "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return ;
		}

	}

	// Now open it really
	emit showRecipe( loadedRecipe->recipeID );
}

void RecipeInputDialog::spellCheck( void )
{
	QString text = instructionsEdit->text();
	K3SpellConfig default_cfg( this );
	K3Spell::modalCheck( text, &default_cfg );
	KMessageBox::information( this, i18n( "Spell check complete." ) );

	if ( text != instructionsEdit->text() )  //check if there were changes
		instructionsEdit->setText( text );
}

void RecipeInputDialog::resizeRecipe( void )
{
	yieldNumInput->value( loadedRecipe->yield.amount, loadedRecipe->yield.amount_offset );
	ResizeRecipeDialog dlg( this, loadedRecipe );

	if ( dlg.exec() == QDialog::Accepted )
		reload();
}

int RecipeInputDialog::ingItemIndex( Q3ListView *listview, const Q3ListViewItem *item ) const
{
	if ( !item )
		return -1;

	if ( item == listview->firstChild() )
		return 0;
	else {
		Q3ListViewItemIterator it( listview->firstChild() );
		int j = 0;
		for ( ; it.current() && it.current() != item; ++it ) {
			if ( it.current() ->rtti() == INGLISTVIEWITEM_RTTI ) {
				if ( !it.current()->parent() || it.current()->parent()->rtti() == INGGRPLISTVIEWITEM_RTTI )
					j++;
			}
		}

		if ( !it.current() )
			return -1;

		return j;
	}
}

void RecipeInputDialog::slotIngredientParser()
{
	UnitList units;
	database->loadUnits(&units);
	IngredientParserDialog dlg(units,this);
	if ( dlg.exec() == QDialog::Accepted ) {
		IngredientList ings = dlg.ingredients();
		QStringList usedGroups;
		bool haveHeader = ingredientList->lastItem() && ingredientList->lastItem()->rtti() == INGGRPLISTVIEWITEM_RTTI;
		for ( IngredientList::iterator it = ings.begin(); it != ings.end(); ++it ) {
			if ( !(*it).group.isEmpty() && usedGroups.indexOf((*it).group) == -1 ) {
				int id = IngredientInputWidget::createNewGroupIfNecessary((*it).group,database);
				addIngredientHeader( Element((*it).group, id) );
				haveHeader = true;
				usedGroups << (*it).group;
			}
			(*it).ingredientID = IngredientInputWidget::createNewIngredientIfNecessary((*it).name,database);
			(*it).units.id = IngredientInputWidget::createNewUnitIfNecessary((*it).units.name,false,(*it).ingredientID,(*it).units,database);

			QList<int> prepIDs = IngredientInputWidget::createNewPrepIfNecessary((*it).prepMethodList,database);
			QList<int>::const_iterator prep_id_it = prepIDs.begin();
			for ( ElementList::iterator prep_it = (*it).prepMethodList.begin(); prep_it != (*it).prepMethodList.end(); ++prep_it, ++prep_id_it ) {
				(*prep_it).id = *prep_id_it;
			}

			addIngredient( *it, !haveHeader );

			if ( usedGroups.count() > 0 && (*it).group.isEmpty() ) {
				Q3ListViewItem *last_item = ingredientList->lastItem();
				if ( last_item->parent() ) {
					last_item->parent()->takeItem( last_item );
					ingredientList->insertItem( last_item );
					last_item->moveItem( ingredientList->lastItem()->parent() );
				}
			}
		}
	}
}

void RecipeInputDialog::slotAddRating()
{
	ElementList criteriaList;
	database->loadRatingCriterion(&criteriaList);

	EditRatingDialog ratingDlg(criteriaList,this);
	if ( ratingDlg.exec() == QDialog::Accepted ) {
		Rating r = ratingDlg.rating();

		for ( RatingCriteriaList::iterator rc_it = r.ratingCriteriaList.begin(); rc_it != r.ratingCriteriaList.end(); ++rc_it ) {
			int criteria_id = database->findExistingRatingByName((*rc_it).name);
			if ( criteria_id == -1 ) {
				database->createNewRating((*rc_it).name);
				criteria_id = database->lastInsertID();
			}
			(*rc_it).id = criteria_id;
		}

		RatingDisplayWidget *item = new RatingDisplayWidget( this );
		item->rating_it = loadedRecipe->ratingList.append(r);
		addRating(r,item);
		ratingListDisplayWidget->insertItem(item,0);
		emit( recipeChanged() ); //Indicate that the recipe changed
	}
}

void RecipeInputDialog::addRating( const Rating &rating, RatingDisplayWidget *item )
{
	int average = qRound(rating.average());
	if ( average >= 0 )
		item->iconLabel->setPixmap( UserIcon(QString("rating%1").arg(average) ) );
	else //no rating criteria, therefore no average (we don't want to automatically assume a zero average)
		item->iconLabel->clear();

	item->raterName->setText(rating.rater);
	item->comment->setText(rating.comment);

	item->criteriaListView->clear();
	for ( RatingCriteriaList::const_iterator rc_it = rating.ratingCriteriaList.begin(); rc_it != rating.ratingCriteriaList.end(); ++rc_it ) {
		Q3ListViewItem * it = new Q3ListViewItem(item->criteriaListView,(*rc_it).name);

		int stars = int((*rc_it).stars * 2); //multiply by two to make it easier to work with half-stars

		QPixmap star = UserIcon(QString::fromLatin1("star_on"));
		int pixmapWidth = 18*(stars/2)+((stars%2==1)?9:0);
		QPixmap generatedPixmap(pixmapWidth,18);

		if ( !generatedPixmap.isNull() ) { //there aren't zero stars
			generatedPixmap.fill();
			QPainter painter( &generatedPixmap );
			painter.drawTiledPixmap(0,0,pixmapWidth,18,star);
			it->setPixmap(1,generatedPixmap);
		}
	}

	item->buttonEdit->disconnect();
	item->buttonRemove->disconnect();
	connect(item->buttonEdit, SIGNAL(clicked()),
		this, SLOT(slotEditRating()));
	connect(item->buttonRemove, SIGNAL(clicked()),
		this, SLOT(slotRemoveRating()));
}

void RecipeInputDialog::slotEditRating()
{
	RatingDisplayWidget *sender = (RatingDisplayWidget*)(QObject::sender()->parent());

	ElementList criteriaList;
	database->loadRatingCriterion(&criteriaList);

	EditRatingDialog ratingDlg(criteriaList,*sender->rating_it,this);
	if ( ratingDlg.exec() == QDialog::Accepted ) {
		Rating r = ratingDlg.rating();

		for ( RatingCriteriaList::iterator rc_it = r.ratingCriteriaList.begin(); rc_it != r.ratingCriteriaList.end(); ++rc_it ) {
			int criteria_id = database->findExistingRatingByName((*rc_it).name);
			if ( criteria_id == -1 ) {
				database->createNewRating((*rc_it).name);
				criteria_id = database->lastInsertID();
			}
			(*rc_it).id = criteria_id;
		}

		(*sender->rating_it) = r;
		addRating(r,sender);
		emit recipeChanged(); //Indicate that the recipe changed
	}
}

void RecipeInputDialog::slotRemoveRating()
{
	RatingDisplayWidget *sender = (RatingDisplayWidget*)(QObject::sender()->parent());
	loadedRecipe->ratingList.remove(sender->rating_it);

	//FIXME: sender is removed but never deleted (sender->deleteLater() doesn't work)
	ratingListDisplayWidget->removeItem(sender);

	emit recipeChanged(); //Indicate that the recipe changed
}

void RecipeInputDialog::addIngredient( const Ingredient &ing, bool noHeader )
{
	Ingredient ingCopy = ing;

	//Append to the ListView
	Q3ListViewItem* lastElement = ingredientList->lastItem();
	while ( lastElement && lastElement->rtti() == INGSUBLISTVIEWITEM_RTTI )
		lastElement = lastElement->itemAbove();

	if ( noHeader && lastElement )
		lastElement = (lastElement->parent())?lastElement->parent():lastElement;

	if ( !noHeader && lastElement &&
		( lastElement->rtti() == INGGRPLISTVIEWITEM_RTTI || ( lastElement->parent() && lastElement->parent() ->rtti() == INGGRPLISTVIEWITEM_RTTI ) ) )
	{
		IngGrpListViewItem * header = ( lastElement->parent() ) ? ( IngGrpListViewItem* ) lastElement->parent() : ( IngGrpListViewItem* ) lastElement;

		ingCopy.groupID = header->id();

		lastElement = new IngListViewItem( header, lastElement, ingCopy );
		for ( Q3ValueList<IngredientData>::const_iterator it = ingCopy.substitutes.begin(); it != ingCopy.substitutes.end(); ++it ) {
			new IngSubListViewItem( lastElement, *it );
		}
		lastElement->setOpen(true);
	}
	else {
		lastElement = new IngListViewItem( ingredientList, lastElement, ingCopy );
		for ( Q3ValueList<IngredientData>::const_iterator it = ing.substitutes.begin(); it != ing.substitutes.end(); ++it ) {
			new IngSubListViewItem( lastElement, *it );
		}
		lastElement->setOpen(true);
	}

	//append to recipe
	loadedRecipe->ingList.append( ingCopy );

	//update the completion in the instructions edit
	instructionsEdit->addCompletionItem( ingCopy.name );

	updatePropertyStatus( ingCopy, true );

	emit changed();
}

void RecipeInputDialog::addIngredientHeader( const Element &header )
{
	Q3ListViewItem *last_item = ingredientList->lastItem();
	if ( last_item && last_item->parent() )
		last_item = last_item->parent();

	IngGrpListViewItem *ing_header = new IngGrpListViewItem( ingredientList, last_item, header.name, header.id );
	ing_header->setOpen( true );
}

void RecipeInputDialog::updatePropertyStatus()
{
	propertyStatusMapRed.clear();
	propertyStatusMapYellow.clear();

	for ( IngredientList::const_iterator ing_it = loadedRecipe->ingList.begin(); ing_it != loadedRecipe->ingList.end(); ++ing_it ) {
		updatePropertyStatus( *ing_it, false );
	}

	showStatusIndicator();
}

void RecipeInputDialog::updatePropertyStatus( const Ingredient &ing, bool updateIndicator )
{
	IngredientPropertyList ingPropertyList;
	database->loadProperties( &ingPropertyList, ing.ingredientID );

	if ( ingPropertyList.count() == 0 ) {
		propertyStatusMapRed.insert(ing.ingredientID,QString(i18n("<b>%1:</b> No nutrient information available.  <a href=\"ingredient#%3\">Provide nutrient information.</a>")).arg(ing.name).arg(ing.ingredientID));
	}

	QMap<int,bool> ratioCache; //unit->conversion possible
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = ingPropertyList.begin(); prop_it != ingPropertyList.end(); ++prop_it ) {
		Ingredient result;

		QMap<int,bool>::const_iterator cache_it = ratioCache.find((*prop_it).perUnit.id);
		if ( cache_it == ratioCache.end() ) {
			RecipeDB::ConversionStatus status = database->convertIngredientUnits( ing, (*prop_it).perUnit, result );
			ratioCache.insert((*prop_it).perUnit.id,status==RecipeDB::Success||status==RecipeDB::MismatchedPrepMethod);

			switch ( status ) {
			case RecipeDB::Success: break;
			case RecipeDB::MissingUnitConversion: {
				if ( ing.units.type != Unit::Other && ing.units.type == (*prop_it).perUnit.type ) {
					propertyStatusMapRed.insert(ing.ingredientID,i18n("<b>%3:</b> Unit conversion missing for conversion from '%1' to '%2'"
						,(ing.units.name.isEmpty()?i18n("-No unit-"):ing.units.name)
						,((*prop_it).perUnit.name)
						,ing.name));
				} else {
					WeightList weights = database->ingredientWeightUnits( ing.ingredientID );
					Q3ValueList< QPair<int,int> > usedIds;
					QStringList missingConversions;
					for ( WeightList::const_iterator weight_it = weights.begin(); weight_it != weights.end(); ++weight_it ) {
						//skip entries that only differ in how it's prepared
						QPair<int,int> usedPair((*weight_it).perAmountUnitID,(*weight_it).weightUnitID);
						if ( usedIds.find(usedPair) != usedIds.end() )
							continue;

						QString toUnit = database->unitName((*weight_it).perAmountUnitID).name;
						if ( toUnit.isEmpty() ) toUnit = i18n("-No unit-");

						QString fromUnit = database->unitName((*weight_it).weightUnitID).name;
						if ( fromUnit.isEmpty() ) fromUnit = i18n("-No unit-");

						QString ingUnit = ing.units.name;
						if ( ingUnit.isEmpty() ) ingUnit = i18n("-No unit-");

						QString propUnit = (*prop_it).perUnit.name;
						if ( propUnit.isEmpty() ) propUnit = i18n("-No unit-");

						missingConversions << conversionPath( ingUnit, toUnit, fromUnit, propUnit);
					}
					propertyStatusMapRed.insert(ing.ingredientID,i18n("<b>%1:</b> Either <a href=\"ingredient#%3\">enter an appropriate ingredient weight entry</a>, or provide conversion information to perform one of the following conversions: %2",
					  ing.name
					  ,("<ul><li>"+missingConversions.join("</li><li>")+"</li></ul>")
					  ,ing.ingredientID)
					);
				}
				break;
			}
			case RecipeDB::MissingIngredientWeight:
				propertyStatusMapRed.insert(ing.ingredientID,QString(i18n("<b>%1:</b> No ingredient weight entries. <a href=\"ingredient#%3\">Provide ingredient weight.</a>")).arg(ing.name).arg(ing.ingredientID));
				break;
			case RecipeDB::MismatchedPrepMethod:
				if ( ing.prepMethodList.count() == 0 )
					propertyStatusMapRed.insert(ing.ingredientID,QString(i18n("<b>%1:</b> There is no ingredient weight entry for when no preparation method is specified. <a href=\"ingredient#%3\">Provide ingredient weight.</a>")).arg(ing.name).arg(ing.ingredientID));
				else
					propertyStatusMapRed.insert(ing.ingredientID,QString(i18n("<b>%1:</b> There is no ingredient weight entry for when prepared in any of the following manners: %2<a href=\"ingredient#%3\">Provide ingredient weight.</a>")).arg(ing.name).arg("<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>").arg(ing.ingredientID));
				break;
			case RecipeDB::MismatchedPrepMethodUsingApprox:
				propertyStatusMapYellow.insert(ing.ingredientID,QString(i18n("<b>%1:</b> There is no ingredient weight entry for when prepared in any of the following manners (defaulting to a weight entry without a preparation method specified): %2<a href=\"ingredient#%3\">Provide ingredient weight.</a>")).arg(ing.name).arg("<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>").arg(ing.ingredientID));
				break;
			default: kDebug()<<"Code error: Unhandled conversion status code "<<status; break;
			}
		}
	}

	if ( updateIndicator )
		showStatusIndicator();
}

void RecipeInputDialog::showStatusIndicator()
{
	if ( propertyStatusMapRed.count() == 0 ) {
		if ( propertyStatusMapYellow.count() == 0 ) {
			propertyStatusLed->setColor( Qt::green );
			propertyStatusLabel->setText( i18n("Complete") );
			propertyStatusButton->setEnabled(false);
		}
		else {
			propertyStatusLed->setColor( Qt::yellow );
			propertyStatusLabel->setText( i18n("Complete, but approximations made") );
			propertyStatusButton->setEnabled(true);
		}
	}
	else {
		propertyStatusLed->setColor( Qt::red );
		propertyStatusLabel->setText( i18n("Incomplete") );
		propertyStatusButton->setEnabled(true);
	}

	if ( propertyStatusMapRed.count() == 0 && propertyStatusMapYellow.count() == 0 )
		propertyStatusDialog->hide();
	else
		statusTextView->setText(statusMessage());
}

QString RecipeInputDialog::statusMessage() const
{
	QString statusMessage;

	if ( propertyStatusMapRed.count() > 0 ) {
		statusMessage.append( i18n("The nutrient information for this recipe is incomplete because the following information is missing:") );
		statusMessage.append("<ul>");
		for ( QMap<int,QString>::const_iterator it = propertyStatusMapRed.begin(); it != propertyStatusMapRed.end(); ++it ) {
			statusMessage.append("<li>");
			statusMessage.append(it.value());
			statusMessage.append("</li>");
		}
		statusMessage.append("</ul>");
	}

	if ( propertyStatusMapYellow.count() > 0 ) {
		statusMessage.append( i18n("The following approximations will be made when determining nutrient information:") );
		statusMessage.append("<ul>");
		for ( QMap<int,QString>::const_iterator it = propertyStatusMapYellow.begin(); it != propertyStatusMapYellow.end(); ++it ) {
			statusMessage.append("<li>");
			statusMessage.append(it.value());
			statusMessage.append("</li>");
		}
		statusMessage.append("</ul>");
	}

	return statusMessage;
}

QString RecipeInputDialog::conversionPath( const QString &ingUnit, const QString &toUnit, const QString &fromUnit, const QString &propUnit ) const
{
	QString path = "'"+ingUnit+"'";

	QString lastUnit = ingUnit;
	if ( lastUnit != toUnit ) {
		path.append(" =&gt; '"+toUnit+"'");
		lastUnit = toUnit;
	}
	if ( lastUnit != fromUnit ) {
		path.append(" =&gt; '"+fromUnit+"'");
		lastUnit = fromUnit;
	}
	if ( lastUnit != propUnit ) {
		path.append(" =&gt; '"+propUnit+"'");
		lastUnit = propUnit;
	}
	return path;
}

void RecipeInputDialog::statusLinkClicked( const QString &link )
{
	if (link.startsWith("ingredient#")) {
		int ingID = link.mid(link.indexOf("#")+1).toInt();
		QString ingName = database->ingredientName(ingID);
		EditPropertiesDialog d(ingID,ingName,database,this);
		d.exec();
	} else if (link.startsWith("unit#")) {
		QString unitIDs = link.mid(link.indexOf("#")+1);
		QStringList idList = unitIDs.split(",", QString::SkipEmptyParts );
		int unitFrom = idList[0].toInt();
		ElementList toUnits;
		int lastUnit = -1;
		for (int i = 1; i < idList.count(); ++i ) {
			int id = idList[i].toInt();
			if ( id != lastUnit ) {
				toUnits << Element(database->unitName(id).name,id);
				lastUnit = id;
			}
		}
		CreateUnitConversionDialog dlg(database->unitName(unitFrom).name,toUnits,this);
		if ( dlg.exec() == QDialog::Accepted ) {
			UnitRatio ratio;
			ratio.uID1 = dlg.toUnitID();
			ratio.uID2 = unitFrom;
			ratio.ratio = dlg.ratio();
			if (ratio.ratio >= 0 ) {
				database->saveUnitRatio(&ratio);
			}
		}
	}
	updatePropertyStatus();
}

#include "recipeinputdialog.moc"
