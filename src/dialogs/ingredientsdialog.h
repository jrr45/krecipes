/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTSDIALOG_H
#define INGREDIENTSDIALOG_H

#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <knuminput.h>

#include "widgets/krelistview.h"
#include "datablocks/unit.h"

class KDoubleNumInput;

class RecipeDB;
class ElementList;
class SelectUnitDialog;
class CreateElementDialog;
class IngredientPropertyList;
class SelectPropertyDialog;
class UnitsDialog;
class IngredientGroupsDialog;
class MixedNumber;
class AmountUnitInput;
class WeightInput;

class IngredientsDialog: public QWidget
{
Q_OBJECT

public:
	IngredientsDialog( QWidget* parent, RecipeDB *db );
	~IngredientsDialog();
	void reload( bool force = true );

private:
	// Widgets
	QGridLayout* layout;
	QPushButton* addIngredientButton;
	QPushButton* removeIngredientButton;
	QPushButton* addUnitButton;
	QPushButton* removeUnitButton;
	QPushButton* addPropertyButton;
	QPushButton* removePropertyButton;
	QPushButton* addWeightButton;
	QPushButton* removeWeightButton;
	KreListView* ingredientListView;
	KreListView* unitsListView;
	KreListView* propertiesListView;
	KreListView* weightsListView;
	QPushButton* pushButton5;
	KDoubleNumInput* inputBox;
	AmountUnitInput* weightInputBox;
	WeightInput* perAmountInputBox;
	IngredientGroupsDialog *groupsDialog;

	// Internal Methods
	void reloadIngredientList( bool force = true );
	void reloadUnitList( void );
	void reloadPropertyList( void );
	void reloadWeightList( void );
	int findPropertyNo( QListViewItem *it );
	void insertIntoListView( QListViewItem *it, int col, AmountUnitInput *amountEdit );

	// Internal Variables
	RecipeDB *database;
	UnitList *unitList;
	IngredientPropertyList *propertiesList;
	ElementList *perUnitListBack;

private slots:
	void addUnitToIngredient( void );
	void removeUnitFromIngredient( void );
	void addWeight();
	void removeWeight();
	void updateLists( void );
	void addPropertyToIngredient( void );
	void removePropertyFromIngredient( void );
	void insertPropertyEditBox( QListViewItem* it );
	void setPropertyAmount( double amount );
	void openUSDADialog( void );
	void itemRenamed( QListViewItem*, const QPoint &, int col );
	void setWeights();
};

#endif
