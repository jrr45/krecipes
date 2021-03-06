/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/


#include <KApplication>
#include <K4AboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <QIcon>
 
#include "mainwindow.h"
 
int main (int argc, char *argv[])
{
	K4AboutData aboutData( "nukedb", 0,
		ki18n("NukeDB"), "0.0",
		ki18n(""),
		K4AboutData::License_GPL,
		ki18n("Copyright © 2012 José Manuel Santamaría Lema") );
	KCmdLineArgs::init( argc, argv, &aboutData );
 
	KApplication app;
	app.setWindowIcon( QIcon::fromTheme(QStringLiteral("edit-bomb")) );
 
	MainWindow* window = new MainWindow();
	window->show();
 
	return app.exec();
}
