/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CATEGORIESEDITORDIALOG_H
#define CATEGORIESEDITORDIALOG_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <q3hbox.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <kiconloader.h>
#include <klistview.h>

#include "datablocks/categorytree.h"

class RecipeDB;
class StdCategoryListView;

/**
@author Unai Garro
*/
class CategoriesEditorDialog: public QWidget
{

	Q_OBJECT

public:

	CategoriesEditorDialog( QWidget* parent, RecipeDB *db );
	~CategoriesEditorDialog();

	void reload();

private:
	// Internal data
	RecipeDB *database;
	//Widgets
	QGridLayout *layout;
	StdCategoryListView *categoryListView;
	Q3HBox *buttonBar;
	QPushButton *newCategoryButton;
	QPushButton *removeCategoryButton;
	KIconLoader *il;
};

#endif
