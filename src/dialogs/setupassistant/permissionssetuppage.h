/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                    *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef PERMISSIONSSETUPPAGE_H
#define PERMISSIONSSETUPPAGE_H

#include <QWidget>

class QLabel;
class QCheckBox;
class QLineEdit;

class PermissionsSetupPage: public QWidget
{
	Q_OBJECT
public:
	// Methods
	PermissionsSetupPage( QWidget *parent );
	bool doUserSetup( void );
	bool useAdmin( void );
	void getAdmin( QString &adminName, QString &adminPass );
private:
	// Widgets
	QLabel *logo;
	QLabel *permissionsText;
	QCheckBox *noSetupCheckBox;
	QCheckBox *rootCheckBox;
	QLineEdit *userEdit;
	QLineEdit *passEdit;

private slots:
	void rootCheckBoxChanged( bool on );
	void noSetupCheckBoxChanged( bool on );

};

#endif
