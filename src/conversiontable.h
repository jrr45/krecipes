/***************************************************************************
 *   Copyright (C) 2003-2004 by                                            *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONVERSIONTABLE_H
#define CONVERSIONTABLE_H
#include <qstring.h>
#include <qtable.h>
#include <qobject.h>

#include "unitratio.h"
#include "elementlist.h"
#include "datablocks/unit.h"

/**
@author Unai Garro
*/


class ConversionTable:public QTable{
Q_OBJECT
public:

	ConversionTable(QWidget* parent,int maxrows,int maxcols);
	~ConversionTable();
	void createNewItem(int r, int c, double amount);
	void setUnitIDs(IDList &idList);
	void setRatio(int ingID1, int ingID2, double ratio);
	void setRatio(const UnitRatio &r){ setRatio(r.uID1,r.uID2,r.ratio); }
	int getUnitID(int rc);
	QString text(int r, int c ) const; //Reimplement, otherwise it won't work this way
	void resize(int r,int c);
	void clear(void);
private:

	//Internal Variables
	double editBoxValue;
	QIntDict<QTableItem> items;
	QIntDict<QWidget> widgets;
	IDList unitIDs; // unit ID list to know the units by ID, not name
	//Internal Methods
	void resizeData(int) {};
	QTableItem *item( int r, int c ) const;
	void setItem(int r, int c, QTableItem *i );
	void clearCell( int r, int c );
	void takeItem(QTableItem *item);
	void insertWidget(int r, int c, QWidget *w );
 	QWidget *cellWidget( int r, int c ) const;
	void clearCellWidget( int r, int c );
	void initTable();
	void swapRows(int,int,bool);
	void swapColumns(int,int,bool);
	void swapCells(int,int,int,int);
protected:
	QWidget* beginEdit ( int row, int col, bool replace );
private slots:
	void acceptValueAndClose(void);
	void repaintCell(int r,int c);
	
	void unitRemoved(int);
	void unitCreated(const Unit&);
signals:
	void ratioChanged(int row, int col, double value);
};

class ConversionTableItem:public QObject, public QTableItem
{
Q_OBJECT
public:
	ConversionTableItem( QTable *t, EditType et );
	QWidget *createEditor() const;
	void setContentFromEditor( QWidget *w );
	void setText( const QString &s );
	void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
	void setTextAndSave(const QString &s);
	int alignment() const{ return Qt::AlignRight; }
signals:
	void ratioChanged(int row, int col, double value);
	void signalRepaintCell(int r,int c);

};


#endif
