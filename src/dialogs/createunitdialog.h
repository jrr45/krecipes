/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CREATEUNITDIALOG_H
#define CREATEUNITDIALOG_H

#include <QDialog>

#include "datablocks/unit.h"

class KComboBox;
class QLineEdit;

/**
@author Unai Garro
*/
class CreateUnitDialog : public QDialog
{
Q_OBJECT

public:
	explicit CreateUnitDialog( QWidget *parent, const QString &name = QString(), const QString &plural = QString(), const QString &name_abbrev = QString(), const QString &plural_abbrev = QString(), Unit::Type type = Unit::Other, bool newUnit = true );
	~CreateUnitDialog();
	Unit newUnit( void );

protected slots:
	void nameAbbrevTextChanged(const QString &);

private:
	//Widgets
	QLineEdit *nameEdit;
	QLineEdit *pluralEdit;
	QLineEdit *nameAbbrevEdit;
	QLineEdit *pluralAbbrevEdit;
	KComboBox *typeComboBox;
};

#endif
