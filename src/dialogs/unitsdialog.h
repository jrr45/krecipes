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

#ifndef UNITSDIALOG_H
#define UNITSDIALOG_H
#include <qwidget.h>
#include <klistview.h>
#include <qpushbutton.h>

#include "unitratiolist.h"

class RecipeDB;
class ConversionTable;

/**
@author Unai Garro
*/
class UnitsDialog:public QWidget{
Q_OBJECT
public:
    UnitsDialog(QWidget *parent, RecipeDB *db);
    ~UnitsDialog();
    void reload (void);
    virtual QSize sizeHint () const{return QSize(600,400);};
private:
	// Widgets
	KListView *unitListView;
	ConversionTable *conversionTable;
	QPushButton *newUnitButton;
	QPushButton *removeUnitButton;
	// Internal methods
	void loadConversionTable(void);
	void loadUnitsList(void);
	void reloadData(void);
	void saveAllRatios( UnitRatioList &ratioList );
	// Internal Variables
	RecipeDB *database;
private slots:
	void createNewUnit(void);
	void modUnit(QListViewItem*);
	void removeUnit(void);
	void saveUnit(QListViewItem*);
	void saveRatio(int r, int c, double value);

};

#endif
