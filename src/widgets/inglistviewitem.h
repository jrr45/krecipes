/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGLISTVIEWITEM_H
#define INGLISTVIEWITEM_H

#include "QListWidget"

#include "datablocks/ingredient.h"

#define INGGRPLISTVIEWITEM_RTTI 1003
#define INGLISTVIEWITEM_RTTI 1004
#define INGSUBLISTVIEWITEM_RTTI 1009

class IngListViewItem : public QListWidgetItem
{
public:
    IngListViewItem( QListWidget* qlv, const Ingredient &i );
    IngListViewItem( QListWidget* qlv, QListWidgetItem *after, const Ingredient &i );

	int rtti() const;

	Ingredient ingredient() const;

	void setGroup( int groupID );
	void setAmount( double amount, double amount_offset );
	void setUnit( const Unit &unit );
	void setPrepMethod( const QString &prepMethod );

protected:
	Ingredient m_ing;
	QString amount_str;

public:
	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );

private:
	void init( const Ingredient &i );
};


class IngSubListViewItem : public IngListViewItem
{
public:
    IngSubListViewItem( QListWidgetItem* qli, const Ingredient &i );

	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );
	virtual int rtti() const;
};


class IngGrpListViewItem : public QListWidgetItem
{
public:
    IngGrpListViewItem( QListWidget* qlv, QListWidgetItem *after, const QString &group, int id );

	int rtti() const;

	QString group() const;
	int id() const;

	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );

protected:
	QString m_group;
	int m_id;

private:
	void init( const QString &group, int id );
};

#endif
