/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DIETWIZARDDIALOG_H
#define DIETWIZARDDIALOG_H

#include <stdlib.h> // For RAND_MAX

#include <QCheckBox>

#include <QLabel>
#include <QSlider>
#include <qspinbox.h>
#include <QList>

#include <QStackedWidget>
//Added by qt3to4:
#include <QPaintEvent>

#include <kvbox.h>

#include "datablocks/constraintlist.h"
#include "datablocks/recipelist.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/recipe.h"
#include "datablocks/unitratiolist.h"

#include "widgets/dblistviewbase.h"

class QPushButton;
class QGroupBox;
class QTabWidget;

class MealInput;
class RecipeDB;

/**
@author Unai Garro
*/


class DietWizardDialog: public KVBox
{

	Q_OBJECT

public:

	DietWizardDialog( QWidget *parent, RecipeDB* db );
	~DietWizardDialog();

private:
	//Private variables
	RecipeDB *database;

	int dayNumber;
	int mealNumber;

	RecipeList *dietRList;

	//Widgets
	KHBox *optionsBox;
	QGroupBox *mealsSliderBox;
	QLabel *mealNumberLabel;
	QSlider *mealNumberSelector;
	QGroupBox *daysSliderBox;
	QLabel *dayNumberLabel;
	QSlider *dayNumberSelector;
	QTabWidget *mealTabs;
	MealInput *mealTab; // points to the current tab
	QPushButton *okButton;

	//Methods
	bool checkCategories( Recipe &rec, int meal, int dish );
	bool checkConstraints( Recipe &rec, int meal, int dish );
	bool checkLimits( IngredientPropertyList &properties, ConstraintList &constraints );
	void loadConstraints( int meal, int dish, ConstraintList *constraints ) const;
	void loadEnabledCategories( int meal, int dish, ElementList *categories );
	void newTab( const QString &name );
	bool categoryFiltering( int meal, int dish ) const;
	int getNecessaryFlags() const;

public:
	//Methods
	void reload( ReloadFlags flags = Load );
	RecipeList& dietList( void );

private slots:
	void changeDayNumber( int dn );
	void changeMealNumber( int mn );
	void createDiet( void );
	void clear();
	void createShoppingList( void );
    void populateIteratorList( RecipeList &rl, QList <RecipeList::Iterator> *il );
signals:
	void dietReady( void );
};

#endif
