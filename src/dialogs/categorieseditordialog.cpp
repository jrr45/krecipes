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

#include "categorieseditordialog.h"
#include "createelementdialog.h"
#include "DBBackend/recipedb.h"
#include <klocale.h>

CategoriesEditorDialog::CategoriesEditorDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

// Store pointer to database
database=db;

// Top & left spacers
layout = new QGridLayout(this);
QSpacerItem *spacerTop=new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Fixed);
layout->addItem(spacerTop,0,1);
QSpacerItem *spacerLeft=new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Minimum);
layout->addItem(spacerLeft,1,0);

//Category List
categoryListView=new KListView(this);
categoryListView->addColumn(i18n("Id"));
categoryListView->addColumn(i18n("Category Name"));
categoryListView->setRenameable(1, true);
categoryListView->setDefaultRenameAction(QListView::Reject);
categoryListView->setAllColumnsShowFocus(true);
layout->addWidget(categoryListView,1,1);

//Buttons
buttonBar=new QHBox(this);
layout->addWidget(buttonBar,2,1);

il=new KIconLoader;

newCategoryButton=new QPushButton(buttonBar);
newCategoryButton->setText(i18n("Create New Category"));
QPixmap pm=il->loadIcon("fileNew2", KIcon::NoGroup,16); newCategoryButton->setIconSet(pm);

removeCategoryButton=new QPushButton(buttonBar);
removeCategoryButton->setText(i18n("Remove"));
pm=il->loadIcon("editshred", KIcon::NoGroup,16); removeCategoryButton->setIconSet(pm);
removeCategoryButton->setMaximumWidth(100);

//Load the data from the database
reload();

//Connect Signals & Slots

connect (newCategoryButton,SIGNAL(clicked()),this,SLOT(createNewCategory()));
connect (removeCategoryButton,SIGNAL(clicked()),this,SLOT(removeCategory()));
connect(this->categoryListView,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )),this, SLOT(modCategory( QListViewItem* )));
connect(this->categoryListView,SIGNAL(itemRenamed (QListViewItem*)),this, SLOT(saveCategory( QListViewItem* )));
}

CategoriesEditorDialog::~CategoriesEditorDialog()
{
}

// (Re)loads the data from the database
void CategoriesEditorDialog::reload(void)
{

//Clear the listview first
categoryListView->clear();

ElementList categoryList;
database->loadCategories(&categoryList);
for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it )
	{
	(void)new QListViewItem(categoryListView,QString::number((*cat_it).id),(*cat_it).name);
	}
}

void CategoriesEditorDialog::createNewCategory(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(this,i18n("New Category"));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewCategory(result); // Create the new category in the database
   reload(); // Reload the list from the database
}
}

void CategoriesEditorDialog::removeCategory(void)
{
// Find the selected category item
QListViewItem *it;
int categoryID=-1;

if ( (it=categoryListView->selectedItem()) ) categoryID=it->text(0).toInt();

if (categoryID>=0) // a category was selected previously
{
database->removeCategory(categoryID);
}

reload();// Reload the list from the database

}

void CategoriesEditorDialog::modCategory(QListViewItem* i)
{
  newCategoryButton->setEnabled(false);
  removeCategoryButton->setEnabled(false);
  categoryListView->rename(i, 1);
}

void CategoriesEditorDialog::saveCategory(QListViewItem* i)
{
  database->modCategory((i->text(0)).toInt(), i->text(1));
  newCategoryButton->setEnabled(true);
  removeCategoryButton->setEnabled(true);
}

