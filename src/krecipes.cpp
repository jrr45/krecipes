/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "krecipes.h"
#include "krecipesview.h"

#include "dialogs/preferences/krecipespreferences.h"
#include "dialogs/recipeviewdialog.h"
#include "dialogs/recipeinput/recipeinputdialog.h"
#include "dialogs/selectrecipedialog.h"
#include "dialogs/ingredientsdialog.h"
#include "dialogs/propertiesdialog.h"
#include "dialogs/shoppinglistdialog.h"
#include "dialogs/categorieseditordialog.h"
#include "dialogs/authorsdialog.h"
#include "dialogs/unitsdialog.h"
#include "dialogs/ingredientmatcherdialog.h"
#include "dialogs/dbimportdialog.h"
#include "dialogs/pagesetupdialog.h"
#include "dialogs/recipeimportdialog.h"
#include "dialogs/similarcategoriesdialog.h"
#include "dialogs/conversiondialog.h"

#include "importers/kreimporter.h"
#include "importers/mmfimporter.h"
#include "importers/mx2importer.h"
#include "importers/mxpimporter.h"
#include "importers/nycgenericimporter.h"
#include "importers/recipemlimporter.h"
#include "importers/rezkonvimporter.h"
#include "importers/kredbimporter.h"

#include "datablocks/recipe.h"
#include "backends/recipedb.h"
#include "backends/progressinterface.h"

#include <QPointer>

#include <QLabel>
#include <QFrame>
#include <QDragEnterEvent>

#include <KTextEdit>
#include <kprogressdialog.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kshortcutsdialog.h>
#include <QAction>
#include <kfiledialog.h>
#include <kconfig.h>

#include <kcursor.h>

#include <ktoolbar.h>
#include <kedittoolbar.h>
#include <kstdaccel.h>
#include <ktoggleaction.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
//Settings headers
#include <kdeversion.h>
#include <KShortcutsDialog>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <KSharedConfig>


Krecipes::Krecipes(): KXmlGuiWindow( 0 )
{
	m_view = new KrecipesView( this );
	setObjectName( "Krecipes" );
	// accept dnd
	setAcceptDrops( true );

	// tell the KMainWindow that this is indeed the main widget
	setCentralWidget( m_view );

	// then, setup our actions
	setupActions();

	// and a status bar
	statusBar() ->show();
	statusLabel = new QLabel;
	statusBar()->addPermanentWidget( statusLabel );

	// apply the saved mainwindow settings, if any, and ask the mainwindow
	// to automatically save settings if changed: window size, toolbar
	// position, icon size, etc.
	setAutoSaveSettings();


	// allow the view to change the statusbar and caption
	connect( m_view, SIGNAL( signalChangeStatusbar( const QString& ) ),
		this, SLOT( changeStatusbar( const QString& ) ) );
	connect( m_view, SIGNAL( signalChangeCaption( const QString& ) ),
		this, SLOT( changeCaption( const QString& ) ) );

	connect( m_view, SIGNAL( panelShown( KrePanel, bool ) ), SLOT( updateActions( KrePanel, bool ) ) );

	connect( m_view, SIGNAL( recipeSelected(bool) ), SLOT( recipeSelected(bool) ) );


	// Enable/Disable the Save Button (Initialize disabled, and connect signal)

	connect( m_view, SIGNAL( enableSaveOption( bool ) ), this, SLOT( enableSaveOption( bool ) ) );

	enableSaveOption( false ); // Disables saving initially
	recipeSelected( false ); //nothing is selected initially

	parsing_file_dlg = new QDialog( this );
	parsing_file_dlg->setObjectName( "parsing_file_dlg" );
	parsing_file_dlg->setModal( true );
	//parsing_file_dlg->setWindowFlags ( Qt::WX11BypassWM );
	QLabel *parsing_file_dlg_label = new QLabel( i18n( "Gathering recipe data from file.\nPlease wait..." ), parsing_file_dlg );
	parsing_file_dlg_label->setFrameStyle( QFrame::Box | QFrame::Raised );
	//( new QVBoxLayout( parsing_file_dlg ) ) ->addWidget( parsing_file_dlg_label );
//PORTING: Verify that widget was added to mainLayout:         parsing_file_dlg->setMainWidget( parsing_file_dlg_label );
// Add mainLayout->addWidget(parsing_file_dlg_label); if necessary
	parsing_file_dlg->adjustSize();
	//parsing_file_dlg->setFixedSize(parsing_file_dlg->size());
	convertDialog = new ConversionDialog(this,m_view->database);
}

Krecipes::~Krecipes()
{}

void Krecipes::updateActions( KrePanel panel, bool show )
{
	switch ( panel ) {
	case RecipeView: {
			exportAction->setEnabled( show );
			printAction->setEnabled( show );
			reloadAction->setEnabled( show );
			copyToClipboardAction->setEnabled( show );
			showRecipeAction->setEnabled( false );

			//can't edit when there are multiple recipes loaded
			if ( show && m_view->viewPanel->recipesLoaded() == 1 ) {
				editAction->setEnabled( true );
			}
			else
				editAction->setEnabled( false );

			break;
		}
	case RecipeEdit: {
			printAction->setEnabled( show );
			exportAction->setEnabled( false );
			showRecipeAction->setEnabled( show );
			break;
		}
	case SelectP: {
			printAction->setEnabled( show );
			exportAction->setEnabled( show );
			if (show)
				m_view->selectPanel->haveSelectedItems();
			else
				recipeSelected( false );
			break;
		}
	case MatcherP: {
			printAction->setEnabled( show );
			exportAction->setEnabled( show );
			if (show) 
				m_view->ingredientMatcherPanel->haveSelectedItems();
			else
				recipeSelected( false );
			break;
		}
	default:
		break;
	}
}

void Krecipes::recipeSelected( bool selected )
{
	copyToClipboardAction->setEnabled( selected );
	showRecipeAction->setEnabled( selected );
	editAction->setEnabled( selected );
}

void Krecipes::setupActions()
{

	printAction =  KStandardAction::print( this, SLOT( filePrint() ), actionCollection() );

	reloadAction = new QAction( this );
	reloadAction->setText( i18nc("@action:inmenu Reload Recipe", "Reloa&d" ) );
	reloadAction->setIcon( QIcon::fromTheme(QStringLiteral("view-refresh")) );
	reloadAction->setShortcut( Qt::Key_F5 );
	actionCollection()->addAction( "reload_action" , reloadAction );
	connect( reloadAction, SIGNAL(triggered(bool)), m_view, SLOT( reloadDisplay() ) );

	editAction = new QAction( this );
	editAction->setText( i18n( "&Edit Recipe" ) );
	editAction->setIcon( QIcon::fromTheme(QStringLiteral("document-edit")) );
	editAction->setShortcut( Qt::CTRL + Qt::Key_E );
	actionCollection()->addAction( "edit_action" , editAction );
	connect( editAction, SIGNAL(triggered(bool)), m_view, SLOT( editRecipe() ) );

	converterAction = new QAction( this );
	converterAction->setText( i18n( "&Measurement Converter" ) );
	converterAction->setShortcut( Qt::CTRL + Qt::Key_M );
	actionCollection()->addAction( "converter_action" , converterAction );
	connect( converterAction, SIGNAL(triggered(bool)), this , SLOT( conversionToolSlot() ) );

	KConfigGroup grp(KSharedConfig::openConfig(),"Advanced");

	if ( grp.readEntry("UnhideMergeTools",false) ) { //FIXME: Please review it, not used yet

		mergeCategoriesAction = new QAction( this );
		mergeCategoriesAction->setIcon( QIcon::fromTheme(QStringLiteral("folder-yellow")) );
		mergeCategoriesAction->setText( i18n( "&Merge Similar Categories..." ) );
		//mergeCategoriesAction->setShortcut( Qt::CTRL + Qt::Key_M );
		actionCollection()->addAction( "merge_categories_action", mergeCategoriesAction );
		connect( mergeCategoriesAction, SIGNAL(triggered(bool)), this, SLOT( mergeSimilarCategories() ) );

		mergeIngredientsAction = new QAction( this );
		mergeIngredientsAction->setIcon( QIcon::fromTheme(QStringLiteral("ingredients")) );
		mergeIngredientsAction->setText( i18n( "&Merge Similar Ingredients..." ) );
		//ºmergeIngredientsAction->setShortcut( Qt::CTRL + Qt::Key_M );
		actionCollection()->addAction( "merge_ingredients_action", mergeIngredientsAction );
		connect( mergeIngredientsAction, SIGNAL(triggered(bool)), this, SLOT( mergeSimilarIngredients() ) );

	}

	QAction *action = KStandardAction::openNew( this, SLOT( fileNew() ), actionCollection() );
	action->setText( i18n( "&New Recipe" ) );
	action->setToolTip( i18n( "&New Recipe" ) );

	saveAction = KStandardAction::save( this, SLOT( fileSave() ), actionCollection() );

	KStandardAction::quit( kapp, SLOT( quit() ), actionCollection() );

	m_statusbarAction = KStandardAction::showStatusbar( this, SLOT( optionsShowStatusbar() ), actionCollection() );

	KStandardAction::keyBindings( this, SLOT( optionsConfigureKeys() ), actionCollection() );
	KStandardAction::configureToolbars( this, SLOT( optionsConfigureToolbars() ), actionCollection() );
	KStandardAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );

	importAction = new QAction( this );
	importAction->setText( i18n( "Import from File..." ) );
	importAction->setIcon( QIcon::fromTheme(QStringLiteral("document-import")) );
	importAction->setShortcut( Qt::CTRL + Qt::Key_I );
	actionCollection()->addAction( "import_action" , importAction );
	connect( importAction, SIGNAL(triggered(bool)), this, SLOT( import() ) );

	importDBAction= new QAction( this );
	importDBAction->setText( i18n( "Import from Database..." ) );
	importDBAction->setIcon( QIcon::fromTheme(QStringLiteral("document-import")) );
	actionCollection()->addAction( "import_db_action" , importDBAction );
	connect( importDBAction, SIGNAL(triggered(bool)), this, SLOT( kreDBImport() ) );

	exportAction = new QAction( this );
	exportAction->setText( i18n( "Export..." ) );
	exportAction->setShortcut( Qt::CTRL + Qt::Key_O );
	exportAction->setIcon( QIcon::fromTheme(QStringLiteral("document-export")) );
	actionCollection()->addAction( "export_action" , exportAction );
	connect( exportAction, SIGNAL(triggered(bool)), this, SLOT( fileExport() ) );

	copyToClipboardAction = new QAction( this );
	copyToClipboardAction->setText( i18n( "&Copy to Clipboard" ) );
	copyToClipboardAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-copy")) );
	copyToClipboardAction->setShortcut( Qt::CTRL + Qt::Key_C );
	actionCollection()->addAction( "copy_to_clipboard_action" , copyToClipboardAction );
	connect( copyToClipboardAction, SIGNAL(triggered(bool)), this, SLOT( fileToClipboard()  ) );


        pageSetupAction = new QAction( this );
        pageSetupAction->setText( i18n( "Page Setup..." ) );
	pageSetupAction->setIcon( QIcon::fromTheme(QStringLiteral("configure")) );
        actionCollection()->addAction( "page_setup_action" , pageSetupAction );
        connect( pageSetupAction, SIGNAL(triggered(bool)), this, SLOT( pageSetupSlot() ) );

        printSetupAction = new QAction( this );
        printSetupAction->setText( i18n( "Print Setup..." ) );
	printSetupAction->setIcon( QIcon::fromTheme(QStringLiteral("configure")) );
        actionCollection()->addAction( "print_setup_action" , printSetupAction );
        connect( printSetupAction, SIGNAL(triggered(bool)), this, SLOT( printSetupSlot() ) );

        backupAction = new QAction( this );
        backupAction->setText( i18n( "Backup..." ) );
	backupAction->setIcon( QIcon::fromTheme(QStringLiteral("document-save-all")) );
        actionCollection()->addAction( "backup_action" , backupAction );
        connect( backupAction, SIGNAL(triggered(bool)), this, SLOT( backupSlot() ) );

        restoreAction = new QAction( this );
        restoreAction->setText( i18n( "Restore..." ) );
	restoreAction->setIcon( QIcon::fromTheme(QStringLiteral("document-revert")) );
        actionCollection()->addAction( "restore_action" , restoreAction );
        connect( restoreAction, SIGNAL(triggered(bool)), this, SLOT( restoreSlot() ) );

	addToShoppingListAction = new QAction( this );
	addToShoppingListAction->setText( i18n( "Add to Shopping List" ) ); 
	addToShoppingListAction->setIcon( QIcon::fromTheme(QStringLiteral("view-pim-tasks")) );
	addToShoppingListAction->setShortcut( Qt::CTRL + Qt::Key_S );
	actionCollection()->addAction( "add_to_shopping_list_action", addToShoppingListAction );
	connect( addToShoppingListAction, SIGNAL(triggered(bool)), m_view, SLOT( addToShoppingList() ) );

	removeFromCategoryAction = new QAction( this );
	removeFromCategoryAction->setText( i18n( "Remove From Category" ) );
	removeFromCategoryAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-delete-shred")) );
	removeFromCategoryAction->setShortcut( Qt::ALT + Qt::Key_Delete );
	actionCollection()->addAction( "remove_from_category_action", removeFromCategoryAction );
	connect( removeFromCategoryAction, SIGNAL(triggered(bool)), m_view, SLOT( removeFromCategory() ) );

	deleteAction = new QAction( this );
	deleteAction->setText( i18n( "Delete" ) );
	deleteAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-delete")) );
	deleteAction->setShortcut( Qt::Key_Delete );
	actionCollection()->addAction( "delete_action", deleteAction );
	connect( deleteAction, SIGNAL(triggered(bool)), m_view, SLOT( deleteCurrentElements() ) );

	showRecipeAction = new QAction( this );
	showRecipeAction->setText( i18n( "Show Recipe(s)" ) );
	showRecipeAction->setIcon( QIcon::fromTheme(QStringLiteral("system-search")) );
	showRecipeAction->setShortcut( Qt::CTRL + Qt::Key_L );
	actionCollection()->addAction( "show_recipe_action", showRecipeAction );
	connect( showRecipeAction, SIGNAL(triggered(bool)), m_view, SLOT( showCurrentRecipes() ) );

	categorizeAction = new QAction( this );
	categorizeAction->setText( i18n( "Categorize" ) );
	categorizeAction->setIcon( QIcon::fromTheme(QStringLiteral("folder-yellow")) );
	categorizeAction->setShortcut( Qt::CTRL + Qt::Key_T );
	actionCollection()->addAction( "categorize_action", categorizeAction );
	connect( categorizeAction, SIGNAL(triggered(bool)), m_view, SLOT( categorizeCurrentRecipe() ) );

	expandAllAction = new QAction( this );
	expandAllAction->setText( i18n( "Expand All" ) );
	expandAllAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
	actionCollection()->addAction( "expand_all_action", expandAllAction );
	connect( expandAllAction, SIGNAL(triggered(bool)), m_view, SLOT( expandAll() ) );

	collapseAllAction = new QAction( this );
	collapseAllAction->setText( i18n ( "Collapse All") );
	collapseAllAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
	actionCollection()->addAction( "collapse_all_action", collapseAllAction );
	connect( collapseAllAction, SIGNAL(triggered(bool)), m_view, SLOT( collapseAll() ) );
	
	//Note that createAction is intended to create anything except a new recipe.
	createAction = new QAction( this );
	createAction->setText( i18nc("@action:button Create new data item", "Create New" ) );
	createAction->setIcon( QIcon::fromTheme(QStringLiteral("document-new")) );
	createAction->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_N );
	actionCollection()->addAction( "create_new_action", createAction );
	connect( createAction, SIGNAL(triggered(bool)), m_view, SLOT( createNewElement() ) );

	renameAction = new QAction( this );
	renameAction->setText( i18n( "Rename" ) );
	renameAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-rename")) );
	renameAction->setShortcut( Qt::CTRL + Qt::Key_R );
	actionCollection()->addAction( "rename_action", renameAction );
	connect( renameAction, SIGNAL(triggered(bool)), m_view, SLOT( renameCurrentElement() ) );

	//Not using an standard action because I don't want it in toolbar nor in menus.
	cutAction = new QAction( this );
	cutAction->setText( i18n( "Cut" ) );
	cutAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-cut")) );
	cutAction->setShortcut( Qt::CTRL + Qt::Key_X );
	actionCollection()->addAction( "cut_action", cutAction );
	connect( cutAction, SIGNAL(triggered(bool)), m_view, SLOT( cut() ) );

	//Not using an standard action because I don't want it in toolbar nor in menus.
	pasteAction = new QAction( this );
	pasteAction->setText( i18n( "Paste" ) );
	pasteAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-paste")) );
	pasteAction->setShortcut( Qt::CTRL + Qt::Key_V );
	actionCollection()->addAction( "paste_action", pasteAction );
	connect( pasteAction, SIGNAL(triggered(bool)), m_view, SLOT( paste() ) );

	pasteAsSubcategoryAction = new QAction( this );
	pasteAsSubcategoryAction->setText( i18n( "Paste as Subcategory" ) );
	pasteAsSubcategoryAction->setIcon( QIcon::fromTheme(QStringLiteral("edit-paste")) );
	pasteAsSubcategoryAction->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_V );
	actionCollection()->addAction( "paste_as_subcategory_action", pasteAsSubcategoryAction );
	connect( pasteAsSubcategoryAction, SIGNAL(triggered(bool)), m_view, SLOT( pasteAsSubcategory() ) );
	
	// Add actions to the context menus in the "select recipe" dialog
	m_view->addSelectRecipeAction( showRecipeAction );
	m_view->addSelectRecipeAction( editAction );
	m_view->addSelectRecipeAction( exportAction );
	m_view->addSelectRecipeAction( addToShoppingListAction );
	m_view->addSelectRecipeAction( copyToClipboardAction );
	m_view->setCategorizeAction( categorizeAction );
	m_view->setRemoveFromCategoryAction( removeFromCategoryAction );
	m_view->addSelectRecipeAction( deleteAction );
	m_view->addCategoryAction( expandAllAction );
	m_view->addCategoryAction( collapseAllAction );
	m_view->addCategoryAction( exportAction );
	// Add actions to the context menus in the "advanced search" dialog
	m_view->addFindRecipeAction( showRecipeAction );
	m_view->addFindRecipeAction( editAction );
	m_view->addFindRecipeAction( exportAction );
	m_view->addFindRecipeAction( copyToClipboardAction );
	m_view->addFindRecipeAction( deleteAction );
	// Add actions to the context menus in the "ingredient matcher" dialog
	m_view->addIngredientMatcherAction( showRecipeAction ); 
	m_view->addIngredientMatcherAction( editAction ); 
	m_view->addIngredientMatcherAction( exportAction );
	// Add actions to the dialogs in the "data" submenu
	m_view->addElementAction( createAction );
	m_view->addElementAction( renameAction );
	m_view->addCategoriesPanelAction( cutAction );
	m_view->setCategoryPasteAction( pasteAction );
	m_view->setPasteAsSubcategoryAction( pasteAsSubcategoryAction );
	m_view->addElementAction( deleteAction );

	updateActions( SelectP, true );
	updateActions( RecipeView, false );
	exportAction->setEnabled( true );
	printAction->setEnabled( true );

	createGUI();
}

void Krecipes::saveProperties( KConfigGroup& group )
{
          Q_UNUSED(group);
	// the 'config' object points to the session managed
	// config file.  anything you write here will be available
	// later when this app is restored

	//if (!m_view->currentUrl().isNull())
	//group.writeEntry("lastURL", m_view->currentUrl());
}

void Krecipes::readProperties( const KConfigGroup& group )
{
          Q_UNUSED(group);
	// the 'config' object points to the session managed
	// config file.  this function is automatically called whenever
	// the app is being restored.  read in here whatever you wrote
	// in 'saveProperties'

	//QString url = group.readEntry("lastURL");

	//if (!url.isNull())
	//m_view->openURL(KUrl(url));
}

void Krecipes::dragEnterEvent( QDragEnterEvent *event )
{
	// accept uri drops only
	event->setAccepted( Q3UriDrag::canDecode( event ) );
}


void Krecipes::fileNew()
{

	// Create a new recipe.
	m_view->createNewRecipe();
}

void Krecipes::fileOpen()
{
	// this slot is called whenever the File->Open menu is selected,
	// the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
	// button is clicked
	/*
	    // this brings up the generic open dialog
	    KUrl url = KUrlRequesterDlg::getURL(QString(), this, i18n("Open Location") );
	*/
	// standard filedialog
	/*KUrl url = QFileDialog::getOpenFileUrl(this, i18n("Open Location"), QString(), QString());
	if (!url.isEmpty())
		m_view->openURL(url);*/
}

void Krecipes::fileSave()
{
	// this slot is called whenever the File->Save menu is selected,
	// the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
	// button is clicked
	m_view->save();
}

void Krecipes::fileExport()
{
	// this slot is called whenever the File->Export menu is selected,
	m_view->exportRecipe();
}

void Krecipes::fileToClipboard()
{
	m_view->exportToClipboard();
}

void Krecipes::filePrint()
{
	m_view->printRequested();
}

void Krecipes::import()
{
	QPointer<KFileDialog> file_dialog = new KFileDialog( KUrl(), QString(
		"*.kre *.kreml|Krecipes (*.kre, *.kreml)\n"
		"*.mx2|MasterCook (*.mx2)\n"
		"*.mxp *.txt|%1 (*.mxp, *.txt)\n"
		"*.mmf *.txt|Meal-Master (*.mmf, *.txt)\n"
		"*.txt|%2 (*.txt)\n"
		"*.xml *.recipeml|RecipeML (*.xml, *.recipeml)\n"
		"*.rk *.txt|Rezkonv (*.rk, *.txt)"
		).arg( i18n("MasterCook Export format") )
		.arg( i18n("\"Now You're Cooking\" Generic Export format") ),
		this,
		0
	);
	file_dialog->setObjectName( "file_dialog" );
	file_dialog->setWindowTitle( i18n( "Import from file" ) );
	file_dialog->setMode( KFile::Files );

	if ( file_dialog->exec() == KFileDialog::Accepted ) {
		QStringList warnings_list;

		QString selected_filter = file_dialog->currentFilter();

		BaseImporter *importer;
		if ( selected_filter == "*.mxp *.txt" )
			importer = new MXPImporter();
		else if ( selected_filter == "*.mmf *.txt" )
			importer = new MMFImporter();
		else if ( selected_filter == "*.txt" )
			importer = new NYCGenericImporter();
		else if ( selected_filter == "*.mx2" )
			importer = new MX2Importer();
		else if ( selected_filter == "*.kre *.kreml" )
			importer = new KreImporter();
		else if ( selected_filter == "*.xml *.recipeml" )
			importer = new RecipeMLImporter();
		else if ( selected_filter == "*.rk *.txt" )
			importer = new RezkonvImporter();
		else {
			KMessageBox::sorry( this,
				i18n( "Filter \"%1\" not recognized.\n"
				"Please select one of the provided filters."  , selected_filter ),
				i18n( "Unrecognized Filter" )
			);
			import(); //let's try again :)
			return ;
		}

		parsing_file_dlg->show();
		KApplication::setOverrideCursor( Qt::WaitCursor );
		importer->parseFiles( file_dialog->selectedFiles() );
		parsing_file_dlg->hide();
		KApplication::restoreOverrideCursor();

		KConfigGroup grp(KSharedConfig::openConfig(),"Import");
		bool direct = grp.readEntry( "DirectImport", false );
		if ( !direct ) {
			QPointer<RecipeImportDialog> import_dialog = new RecipeImportDialog( importer->recipeList() );

			if ( import_dialog->exec() != QDialog::Accepted ) {
				delete importer;
				return;
			}
			else
				importer->setRecipeList( import_dialog->getSelectedRecipes() );

			delete import_dialog;
		}

		importer->import(m_view->database);
		//m_view->database->import( importer ); //TODO TESTS: Do it this way

		if ( !importer->getMessages().isEmpty() ) {
			KTextEdit * warningEdit = new KTextEdit( this );
			warningEdit->setText( i18n("<p>NOTE: We recommend that all recipes generating warnings be checked to ensure that they were properly imported, and no loss of recipe data has occurred.</p>" ) + importer->getMessages() );
			warningEdit->setReadOnly( true );

			//FIXME: This dialog should allow cancel the import.
			QPointer<QDialog> showWarningsDlg = new QDialog( this );
			showWarningsDlg->setWindowTitle( i18n("Import Warnings") );
			QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
			QWidget *mainWidget = new QWidget(this);
			QVBoxLayout *mainLayout = new QVBoxLayout;
			showWarningsDlg->setLayout(mainLayout);
			mainLayout->addWidget(mainWidget);
			QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
			okButton->setDefault(true);
			okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
			showWarningsDlg->connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
			showWarningsDlg->connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
			//PORTING SCRIPT: WARNING mainLayout->addWidget(buttonBox) must be last item in layout. Please move it.
			mainLayout->addWidget(buttonBox);

//PORTING: Verify that widget was added to mainLayout: 			showWarningsDlg->setMainWidget( warningEdit ); //QDialog will delete warningEdit for us
// Add mainLayout->addWidget(warningEdit); if necessary
			showWarningsDlg->resize( QSize( 550, 250 ) );
			showWarningsDlg->exec();
			delete showWarningsDlg;
		}

		delete importer;
	}

	delete file_dialog;
}

void Krecipes::kreDBImport()
{
	QPointer<DBImportDialog> importOptions = new DBImportDialog;
	if ( importOptions->exec() == QDialog::Accepted ) {
		//Get all params, even if we don't use them
		QString host, user, pass, table;
		int port;
		importOptions->serverParams( host, user, pass, port, table );

		KreDBImporter importer( importOptions->dbType(), host, user, pass, port ); //last 4 params may or may not be even used (depends on dbType)

		parsing_file_dlg->show();
		KApplication::setOverrideCursor( Qt::WaitCursor );
		QStringList tables;
		if ( importOptions->dbType() == "SQLite" )
			tables << importOptions->dbFile();
		else //MySQL or PostgreSQL
			tables << table;
		importer.parseFiles( tables );
		parsing_file_dlg->hide();
		KApplication::restoreOverrideCursor();

		KConfigGroup grp(KSharedConfig::openConfig(),"Import");
		bool direct = grp.readEntry( "DirectImport", false );
		if ( !direct ) {
			QPointer<RecipeImportDialog> import_dialog = new RecipeImportDialog( importer.recipeList() );

			if ( import_dialog->exec() != QDialog::Accepted ) {
				delete import_dialog;
				return;
			}
			else
				importer.setRecipeList( import_dialog->getSelectedRecipes() );

			delete import_dialog;

		}

		QString error = importer.getErrorMsg();
		if ( !error.isEmpty() ) {
			KMessageBox::sorry( this, error );
		}
		else
			importer.import(m_view->database);
	}
	delete importOptions;
}

void Krecipes::pageSetupSlot()
{
	Recipe recipe;
	m_view->selectPanel->getCurrentRecipe( &recipe );

	KConfigGroup grp(KSharedConfig::openConfig(),"Page Setup");
	QString layout = grp.readEntry( "Layout", QStandardPaths::locate(QStandardPaths::DataLocation, "layouts/None.klo" ) );
	QString printLayout = grp.readEntry( "PrintLayout", QStandardPaths::locate(QStandardPaths::DataLocation, "layouts/None.klo" ) );

	if ( layout == printLayout ) {
		KMessageBox::information( this, i18n("The recipe print and view layouts use the same file for their style, meaning changing one view's look changes them both.  If this is not the behavior you desire, load one style and save it under a different name."),
		QString(), "sharedLayoutWarning" );
	}

	QPointer<PageSetupDialog> page_setup = new PageSetupDialog( this, recipe );
	page_setup->exec();
	delete page_setup;
}

void Krecipes::printSetupSlot()
{
	Recipe recipe;
	m_view->selectPanel->getCurrentRecipe( &recipe );

        KConfigGroup grp(KSharedConfig::openConfig(),"Page Setup");
	QString layout = grp.readEntry( "Layout", QStandardPaths::locate(QStandardPaths::DataLocation, "layouts/None.klo" ) );
	QString printLayout = grp.readEntry( "PrintLayout", QStandardPaths::locate(QStandardPaths::DataLocation, "layouts/None.klo" ) );

	if ( layout == printLayout ) {
		KMessageBox::information( this, i18n("The recipe print and view layouts use the same file for their style, meaning changing one view's look changes them both.  If this is not the behavior you desire, load one style and save it under a different name."),
		QString(), "sharedLayoutWarning" );
	}

	QPointer<PageSetupDialog> pageSetup = new PageSetupDialog( this, recipe, "Print" );
	pageSetup->setWindowTitle( i18n("Print Setup") );
	pageSetup->exec();
	delete pageSetup;
}

void Krecipes::conversionToolSlot()
{
	convertDialog->show();
}

void Krecipes::backupSlot()
{
	QString fileName = QFileDialog::getSaveFileName(0, QString(), KUrl(, QString());
		QString("*.krecbk|%1 (*.krecbk)").arg(i18n("Krecipes Backup File")),
		this, i18n("Save Backup As..."));

	int overwrite = KMessageBox::Yes;
	if ( QFile::exists(fileName) ) {
		overwrite = KMessageBox::warningYesNo( this, i18n( "File \"%1\" exists.  Are you sure you want to overwrite it?" , fileName ) );
	}

	if ( !fileName.isEmpty() && overwrite == KMessageBox::Yes ) {
		ProgressInterface pi(this);
		pi.listenOn(m_view->database);

		QString errMsg;
		if ( !m_view->database->backup( fileName, &errMsg ) )
			KMessageBox::error( this, errMsg, i18n("Backup Failed") );
	}
}

void Krecipes::restoreSlot()
{
	QString filename = QFileDialog::getOpenFileName(0, QString(), KUrl(QString());
		QString("*.krecbk|%1 (*.krecbk)").arg(i18n("Krecipes Backup File")),
		this,i18n("Restore Backup"));

	if ( !filename.isEmpty() ) {
		switch ( KMessageBox::warningContinueCancel(this,i18n("<b>Restoring this file will erase ALL data currently in the database.</b><br /><br />If you want to keep the recipes in your database, click \"Cancel\" and first export your recipes.  These can then be imported once the restore is complete.<br /><br />Are you sure you want to proceed?"),QString(),KStandardGuiItem::cont(), KStandardGuiItem::cancel(), "RestoreWarning") ) {
		case KMessageBox::Continue: {
			ProgressInterface pi(this);
			pi.listenOn(m_view->database);

			QString errMsg;
			if ( m_view->database->restore( filename, &errMsg ) )
				KMessageBox::information(this,i18n("Restore successful."));
			else
				KMessageBox::error( this, errMsg, i18n("Restore Failed") );

			pi.listenOn(0);
			m_view->reload();
		}
		case KMessageBox::Cancel:
		default: break;
		}
	}
}

void Krecipes::mergeSimilarCategories()
{
	ElementList categories;
	m_view->database->loadCategories(&categories);
	QPointer<SimilarCategoriesDialog> dlg = new SimilarCategoriesDialog( categories, this );
	if ( dlg->exec() == QDialog::Accepted ) {
		QList<int> ids = dlg->matches();
		QString name = dlg->element();

		int id = m_view->database->findExistingCategoryByName(name);
		if ( id == -1 ) {
			id = m_view->database->createNewCategory(name);
		}

		for ( QList<int>::const_iterator it = ids.constBegin(); it != ids.constEnd(); ++it ) {
			if ( id != *it )
				m_view->database->mergeCategories(id, *it);
		}
	}
	delete dlg;
}

void Krecipes::mergeSimilarIngredients()
{
	ElementList ingredients;
	m_view->database->loadIngredients(&ingredients);
	QPointer<SimilarCategoriesDialog> dlg = new SimilarCategoriesDialog( ingredients, this );
	if ( dlg->exec() == QDialog::Accepted ) {
		QList<int> ids = dlg->matches();
		QString name = dlg->element();

		if ( ids.isEmpty() || name.isEmpty() ) return;

		int id = m_view->database->findExistingIngredientByName(name);
		if ( id == -1 ) {
			id = m_view->database->createNewIngredient(name);
		}

		for ( QList<int>::const_iterator it = ids.constBegin(); it != ids.constEnd(); ++it ) {
			if ( id != *it )
				m_view->database->mergeIngredients(id, *it);
		}
	}
	delete dlg;
}

//return true to close app
bool Krecipes::queryClose()
{
	if ( !m_view->inputPanel->everythingSaved() ) {
		switch ( KMessageBox::questionYesNoCancel( this,
		         i18n( "A recipe contains unsaved changes.\n"
		               "Do you want to save the changes before exiting?" ),
		         i18n( "Unsaved Changes" ) ) ) {
		case KMessageBox::Yes:
			return m_view->save();
		case KMessageBox::No:
			return true;
		case KMessageBox::Cancel:
			return false;
		default:
			return true;
		}
	}
	else
		return true;
}

void Krecipes::optionsShowStatusbar()
{
	// this is all very cut and paste code for showing/hiding the
	// statusbar
	if ( m_statusbarAction->isChecked() )
		statusBar() ->show();
	else
		statusBar() ->hide();
}

void Krecipes::optionsConfigureKeys()
{
	KShortcutsDialog::configure( actionCollection(), KShortcutsEditor::LetterShortcutsAllowed , this, true );
}

void Krecipes::optionsConfigureToolbars()
{
	// use the standard toolbar editor
	saveMainWindowSettings( KConfigGroup(KSharedConfig::openConfig(), autoSaveGroup() ));

	QPointer<KEditToolBar> dlg = new KEditToolBar( actionCollection() );
	connect( dlg, SIGNAL( newToolbarConfig() ), this, SLOT( newToolbarConfig() ) );
	dlg->exec();
	delete dlg;
}

void Krecipes::newToolbarConfig()
{
	// this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
	// recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
	createGUI();

	applyMainWindowSettings( KConfigGroup(KSharedConfig::openConfig(), autoSaveGroup() ));
}

void Krecipes::optionsPreferences()
{

	// popup some sort of preference dialog, here
	QPointer<KrecipesPreferences> dlg = new KrecipesPreferences( this );
	dlg->exec();
	m_view->inputPanel->reloadCheckSpelling();
	delete dlg;

}

void Krecipes::changeStatusbar( const QString& text )
{
	// display the text on the statusbar
	statusLabel->setText( text );
}

void Krecipes::changeCaption( const QString& text )
{
	// display the text on the caption
	setWindowTitle( text );
}
void Krecipes::enableSaveOption( bool en )
{
	saveAction->setEnabled( en );
}

#include "krecipes.moc"
