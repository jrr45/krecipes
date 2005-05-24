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

#ifndef SELECTCATEGORIESDIALOG_H
#define SELECTCATEGORIESDIALOG_H

#include <qdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QGridLayout>
#include <klistview.h>

#include "datablocks/elementlist.h"

class CategoryTree;
class CategoryCheckListItem;
class CategoryCheckListView;
class RecipeDB;

/**
@author Unai Garro
*/
class SelectCategoriesDialog: public QDialog
{

	Q_OBJECT

public:

	SelectCategoriesDialog( QWidget *parent, const ElementList &items_on, RecipeDB* db );
	~SelectCategoriesDialog();
	void getSelectedCategories( ElementList *selected );
private:

	//Widgets
	CategoryCheckListView *categoryListView;
	QGridLayout *layout;
	QPushButton *okButton;
	QPushButton *cancelButton;

	//Variables
	RecipeDB *database;

	//Private methods
	void loadCategories( const ElementList &items_on );

private slots:
	void createNewCategory( void );
};

#endif
