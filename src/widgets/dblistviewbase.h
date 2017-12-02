/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DBLISTVIEWBASE_H
#define DBLISTVIEWBASE_H

#include <QMap>
#include <QKeyEvent>
#include <QListWidget>
#include <QListWidgetItem>

#define PREVLISTITEM_RTTI 1002
#define NEXTLISTITEM_RTTI 1003

class KProgressDialog;

class RecipeDB;

enum ReloadFlags { 
	Load, 			/** Only performs the reload if the list hasn't already been loaded */
	ReloadIfPopulated,	/** Only performs the reload if the list has been loaded */
	ForceReload		/** Load/reload the list regardless of whether or not it's been loaded */
};

class DBListViewBase : public QListWidget
{
Q_OBJECT

public:
	DBListViewBase( QWidget *, RecipeDB *, int total );
	~DBListViewBase();

	void reload( ReloadFlags flag = Load );

signals:
	void nextGroupLoaded();
	void prevGroupLoaded();

protected:
	/**
	 * Called when the list view is ready to be used, i.e., it has been loaded with data.
	 * Until the list view has been loaded, we can ignore all database signals regarding changes
	 * of data.  Therefore, subclasses should connect to these signals during this call.
	 */
	virtual void init(){}
	virtual void load(int limit, int offset) = 0;
	virtual void keyPressEvent( QKeyEvent *e );
	bool handleElement( const QString & );
    virtual void createElement( QListWidgetItem * );
    void removeElement( QListWidgetItem *, bool delete_item = true );

	bool reloading(){ return bulk_load; }
    void setSorting(int c,bool order=true){QListWidget::setSortingEnabled(false);} //don't do sorting, the database comes sorted from the database anyways
	void setTotalItems(int);

	RecipeDB *database;
	int curr_limit;
	int curr_offset;

	//make this protected because the data should always be synced with the database
    void clear(){QListWidget::clear();}

public slots:
    void rename( QListWidgetItem *, int c );

protected slots:
    void slotDoubleClicked( QListWidgetItem * );

private:
	void activatePrev();
	void activateNext();

	int total;
	
	bool bulk_load;

    QMap<QListWidgetItem*,QListWidgetItem*> lastElementMap;
    QListWidgetItem *delete_me_later;

	KProgressDialog *m_progress;
	int m_totalSteps;
};

#endif //LISTVIEWBASE_H
