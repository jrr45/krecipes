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

#ifndef RECIPEACTIONSHANDLER_H
#define RECIPEACTIONSHANDLER_H

#include <QObject>
#include <QPoint>
#include <QList>
#include <QtWebEngineWidgets/QWebEnginePage>

class QTreeWidgetItem;
class QTreeWidget;
class QMenu;
class RecipeDB;
class QAction;
class QTemporaryDir;

/** @brief A class that centralizes common actions for recipes such as saving and editing.
  * 
  * It acts upon a given QTreeWidget that is assumed to be a list of recipes.  It
  * automagically enables this list view with a popup menu for user access to 
  * the provided actions.
  *
  * @author Jason Kivlighn
  */
class RecipeActionsHandler : public QObject
{
	Q_OBJECT

public:
	enum ItemType { Category, Recipe };

    RecipeActionsHandler( QTreeWidget *parentListView, RecipeDB *db );
	~RecipeActionsHandler()
	{}

	void addRecipeAction( QAction * action );
	void addCategoryAction( QAction * action );
	void setCategorizeAction( QAction * action );
	void setRemoveFromCategoryAction( QAction * action );

	static void exportRecipes( const QList<int> &ids, const QString & caption, const QString &selection, RecipeDB *db );
	static void exportRecipe( int id, const QString & caption, const QString &selection, RecipeDB *db );
	static void recipesToClipboard( const QList<int> &ids, RecipeDB *db );

signals:
	void recipeSelected( bool );
	void recipeSelected( int id, int action );
	void recipesSelected( const QList<int> &ids, int action );
	
	//For internal use only
	void printDone();

public slots:
	void exec( ItemType type, const QPoint &p );
    void showPopup( QTreeWidget *, QTreeWidgetItem *, const QPoint & );

	void categorize();

	/** Signals an open event (via the recipeSelected() signal) for the recipe(s) currently
	  * selected in the list view 
	  */
	void open();

	/** Signals an edit event (via the recipeSelected() signal) for the recipe currently
	  * selected in the list view 
	  */
	void edit();

	/** Saves the recipe(s) currently selected in the list view, prompting with a file
	         * dialog.
	  */
	void recipeExport();

	/** Prints the recipe(s) currently selected in the list view, showing a dialog to preview */
	void recipePrint();
	
	/** Prints the given recipes, showing a dialog to preview the result */
	void printRecipes( const QList<int> &ids, RecipeDB *db );

	/** Removes the recipe(s) currently selected in the list view from its current category */
	void removeFromCategory();

	/** Removes the recipe(s) currently selected in the list view from the database */
	void remove
		();

	/** Add the recipe(s) currently selected in the list view to the shopping list dialog */
	void addToShoppingList();

	/** Expands all items in the list view */
	void expandAll();

	/** Collapses all items in the list view */
	void collapseAll();

	void recipesToClipboard();
	
	QList<int> recipeIDs() const;

	void selectionChangedSlot();

private slots:
	void print( bool ok );
	void printDoneSlot();

private:
	QMenu *kpop;
	QMenu *catPop;

    QTreeWidget *parentListView;
	RecipeDB *database;

	QAction * categorizeAction;
	QAction * removeFromCategoryAction;

	QList<int> getAllVisibleItems();
    QList<int> recipeIDs( const QList<QTreeWidgetItem *> &items ) const;

	QTemporaryDir * m_tempdir;
    QWebEnginePage * m_printPage;
};

#endif //RECIPEACTIONSHANDLER_H

