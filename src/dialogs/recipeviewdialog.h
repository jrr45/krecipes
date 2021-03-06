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

#ifndef RECIPEVIEWDIALOG_H
#define RECIPEVIEWDIALOG_H


#include <QString>
#include <kvbox.h>
#include <QList>
class QTemporaryDir;
class RecipeDB;
class Recipe;
class KHTMLPart;
class QFrame;
class QToolButton;

/**
@author Unai Garro
*/

class RecipeViewDialog : public KVBox
{
	Q_OBJECT

public:
	RecipeViewDialog( QWidget *parent, RecipeDB *db, int recipeID = -1 );

	~RecipeViewDialog();

	/** @return Boolean indicating whether or not the recipe was successfully loaded */
	bool loadRecipe( int recipeID );

	/** @return Boolean indicating whether or not the recipes were successfully loaded */
	bool loadRecipes( const QList<int> &ids );

	int recipesLoaded() const
	{
		return ids_loaded.count();
	}
	const QList<int> currentRecipes() const
	{
		return ids_loaded;
	}

	void reload();

	void showButtons();
	void hideButtons();

signals:
	void closeRecipeView();
	void editRecipe();

private:

	// Internal Variables
	KVBox *khtmlpartBox;
	KHTMLPart *recipeView;
	QFrame *functionsBox;
	QToolButton *editButton;
	QToolButton *closeButton;
	RecipeDB *database;
	bool recipe_loaded;
	QList<int> ids_loaded;
	QString tmp_filename;

	QTemporaryDir * m_tempdir;

	// Internal Methods
	bool showRecipes( const QList<int> &ids );

private slots:
	void recipeRemoved(int);

};


#endif
