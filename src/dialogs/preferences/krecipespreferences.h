/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KRECIPESPREFERENCES_H
#define KRECIPESPREFERENCES_H

#include <KPageDialog>

#include <QMap>

class ServerPrefs;
class NumbersPrefs;
class ImportPrefs;
class PerformancePrefs;
class SpellCheckingPrefs;


class KrecipesPreferences : public KPageDialog
{
	Q_OBJECT
public:
	KrecipesPreferences( QWidget *parent );

protected slots:
	void slotHelp();

private:
	ServerPrefs *m_pageServer;
	NumbersPrefs *m_pageNumbers;
	ImportPrefs *m_pageImport;
	PerformancePrefs *m_pagePerformance;
	SpellCheckingPrefs *m_pageSpellChecking;

	QMap<QString,QString> m_helpMap;

private slots:
	void saveSettings( void );
};

#endif
