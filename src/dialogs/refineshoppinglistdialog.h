/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef REFINESHOPPINGLISTDIALOG_H
#define REFINESHOPPINGLISTDIALOG_H

#include <qmap.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <QDialog>

#include "datablocks/ingredientlist.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QTreeWidgetItem;
class QPushButton;

class RecipeDB;
class ElementList;
class KreListView;

class RefineShoppingListDialog : public QDialog
{
	Q_OBJECT

public:
	RefineShoppingListDialog( QWidget* parent, RecipeDB *db, const ElementList &recipeList );
	~RefineShoppingListDialog();

	QLabel* helpLabel;
	KreListView* allIngListView;
	QPushButton* addButton;
	QPushButton* removeButton;
	KreListView* ingListView;

protected:
	QVBoxLayout* layout1;

protected slots:
	virtual void languageChange();
	virtual void accept();
	void addIngredient();
	void removeIngredient();
	void itemRenamed( QTreeWidgetItem*, const QString &, int );

private:
	void loadData();

	RecipeDB *database;
	IngredientList ingredientList;
	QMap<QTreeWidgetItem*, IngredientList::iterator> item_ing_map;
};

#endif // REFINESHOPPINGLISTDIALOG_H
