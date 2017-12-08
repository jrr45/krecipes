/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "progressinterface.h"

#include <QApplication>

#include <QProgressDialog>

#include "recipedb.h"

ProgressInterface::ProgressInterface( QWidget *parent ) : progress_dlg(0), database(0), m_rate(1), m_rate_at(0)
{
	slot_obj = new ProgressSlotObject(parent,this);
}

ProgressInterface::~ProgressInterface()
{
	listenOn(0);
	delete slot_obj;
}

void ProgressInterface::progressBegin( int steps, const QString &caption, const QString &text, int rate )
{
	m_rate = rate;

	progress_dlg = new QProgressDialog((QWidget*)slot_obj->parent());
    progress_dlg->setLabelText( caption );
    progress_dlg->setWindowTitle( text );
	progress_dlg->setModal( true );

	progress_dlg->setRange( 0, steps );
}

void ProgressInterface::progressDone()
{
	delete progress_dlg;
	progress_dlg = 0;

	m_rate_at = 0;
}

void ProgressInterface::progress()
{
	if ( progress_dlg->wasCanceled() ) {
		database->cancelOperation();
	}
	else {
		++m_rate_at;

		if ( m_rate_at % m_rate == 0 ) {
			progress_dlg->setValue(progress_dlg->value()+1);
			m_rate_at = 0;
		}
		qApp->processEvents();
	}
}

void ProgressInterface::listenOn( RecipeDB *db )
{
	if ( database)
		database->disconnect(slot_obj);

	if ( db ) {
		slot_obj->connect( db, SIGNAL(progressBegin(int,const QString&,const QString&,int)), slot_obj, SLOT(progressBegin(int,const QString&,const QString&,int)) );
		slot_obj->connect( db, SIGNAL(progressDone()), slot_obj, SLOT(progressDone()) );
		slot_obj->connect( db, SIGNAL(progress()), slot_obj, SLOT(progress()) );
	}

	database = db;
}

