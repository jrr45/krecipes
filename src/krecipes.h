/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECIPES_H
#define KRECIPES_H

#include <kapplication.h>
#include <QAction>

#include "krecipesview.h" //for KrePanel
#include <QDragEnterEvent>
#include <KXmlGuiWindow>

class KrecipesView;

class KToggleAction;
class KDialog;
class ConversionDialog;
class QLabel;

/**
 * This class serves as the main window for Krecipes.  It handles the
 * menus, toolbars, and status bars.
 */
class Krecipes : public KXmlGuiWindow
{
	Q_OBJECT
public:
	/**
	 * Default Constructor
	 */
	Krecipes();

	/**
	 * Default Destructor
	 */
	~Krecipes();


protected:
	/**
	 * Overridden virtuals for Qt drag 'n drop (XDND)
	 */
	virtual void dragEnterEvent( QDragEnterEvent *event );

protected:
	/**
	 * This function is called when it is time for the app to save its
	 * properties for session management purposes.
	 */
	void saveProperties( KConfigGroup& );

	/**
	 * This function is called when this app is restored.  The KConfig
	 * object points to the session management config file that was saved
	 * with @ref saveProperties
	 */
	void readProperties(const KConfigGroup& );

	virtual bool queryClose();


private slots:
	void fileNew();
	void fileOpen();
	void fileSave();
	void fileExport();
	void fileToClipboard();
	void filePrint();
	//void optionsShowToolbar();
	void optionsShowStatusbar();
	void optionsConfigureKeys();
	void optionsConfigureToolbars();
	void optionsPreferences();
	void newToolbarConfig();
	void import();
	void kreDBImport();
	void pageSetupSlot();
	void printSetupSlot();
	void conversionToolSlot();
	void backupSlot();
	void restoreSlot();
	void mergeSimilarCategories();
	void mergeSimilarIngredients();

	/**  This function is called whenever a panel is shown or hidden and then sets
	  *  actions as enabled as appropriate.
	  */
	void updateActions( KrePanel panel, bool show );

	void changeStatusbar( const QString& text );
	void changeCaption( const QString& text );

private:
	// Private methods
	void setupAccel();
	void setupActions();

private:
	KrecipesView *m_view;

	KToggleAction *m_toolbarAction;
	KToggleAction *m_statusbarAction;

private:
	// Internal variables
	QAction *saveAction;
	QAction *exportAction;
	QAction *editAction;
	QAction *printAction;
	QAction *reloadAction;
	QAction *copyToClipboardAction;
	QAction *converterAction;
	QAction *mergeCategoriesAction;
	QAction *mergeIngredientsAction;
	QAction *importAction;
	QAction *importDBAction;
	QAction *pageSetupAction;
	QAction *printSetupAction;
	QAction *backupAction;
	QAction *restoreAction;
	QAction *addToShoppingListAction;
	QAction *removeFromCategoryAction;
	QAction *deleteAction;
	QAction *showRecipeAction;
	QAction *categorizeAction;
	QAction *expandAllAction;
	QAction *collapseAllAction;
	QAction *createAction;
	QAction *renameAction;
	QAction *cutAction;
	QAction *pasteAction;
	QAction *pasteAsSubcategoryAction;

	QLabel * statusLabel;
	
	KDialog *parsing_file_dlg;
	ConversionDialog *convertDialog;

	QList<QAction *> recipe_actions;

private slots:
	void enableSaveOption( bool en = true );
	void recipeSelected( bool );
};

#endif // KRECIPES_H
