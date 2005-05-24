/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNITLISTVIEW_H
#define UNITLISTVIEW_H

#include "dblistviewbase.h"

#include "datablocks/unit.h"

class RecipeDB;
class KPopupMenu;

class UnitListView : public DBListViewBase
{
	Q_OBJECT

public:
	UnitListView( QWidget *parent, RecipeDB *db );

public slots:
	virtual void load( int curr_limit, int curr_offset );

protected slots:
	virtual void createUnit( const Unit & ) = 0;
	virtual void removeUnit( int ) = 0;

	void checkCreateUnit( const Unit &el );
};

class StdUnitListView : public UnitListView
{
	Q_OBJECT

public:
	StdUnitListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createUnit( const Unit & );
	virtual void removeUnit( int );

private slots:
	void showPopup( KListView *, Q3ListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modUnit( Q3ListViewItem* i, const QPoint &p, int c );
	void saveUnit( Q3ListViewItem* i, const QString &text, int c );

private:
	bool checkBounds( const Unit &unit );

	KPopupMenu *kpop;
};

#endif //UNITLISTVIEW_H
