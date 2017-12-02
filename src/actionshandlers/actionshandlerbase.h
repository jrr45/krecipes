/***************************************************************************
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef ACTIONSHANDLERBASE_H
#define ACTIONSHANDLERBASE_H

#include <QObject>

class DBListViewBase;
class KMenu;
class QAction;
class RecipeDB;
class QListWidgetItem;
class QListWidget;
class QPoint;

class ActionsHandlerBase : public QObject
{
	Q_OBJECT

public:
	ActionsHandlerBase( DBListViewBase *_parentListView, RecipeDB *db );

public slots:
	virtual void createNew() = 0;
	virtual void rename();
	virtual void remove() = 0;
	void addAction( QAction * action );

protected:
	KMenu *kpop;

	DBListViewBase *parentListView;
	RecipeDB *database;

protected slots:
    virtual void renameElement( QListWidgetItem* i, const QPoint &p, int c );
    virtual void saveElement( QListWidgetItem* i ) = 0;
    virtual void showPopup( QListWidget *, QListWidgetItem *, const QPoint &);
};

#endif //ACTIONSHANDLERBASE_H

