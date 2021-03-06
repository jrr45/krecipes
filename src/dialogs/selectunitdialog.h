/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SELECTUNITDIALOG_H
#define SELECTUNITDIALOG_H

#include <qwidget.h>
#include <QPushButton>
#include <QGroupBox>


#include <QListWidget>
#include <QDialog>
#include <kvbox.h>

#include "datablocks/unit.h"

/**
@author Unai Garro
*/
class SelectUnitDialog : public QDialog
{
public:
	enum OptionFlag { ShowEmptyUnit, HideEmptyUnit } ;

	SelectUnitDialog( QWidget* parent, const UnitList &unitList, OptionFlag = ShowEmptyUnit );

	~SelectUnitDialog();

	int unitID( void );

private:
	//Widgets
	QGroupBox *box;
    QListWidget *unitChooseView;
	OptionFlag m_showEmpty;

	void loadUnits( const UnitList &unitList );

};

#endif
