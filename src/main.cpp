/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

//#include "krecipes.h"
#include "krecipes_version.h"

#include <iostream>

#include "convert_sqlite3.h"
#include "datablocks/elementlist.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>

#include <KLocalizedString>


int main( int argc, char **argv )
{

	qRegisterMetaType<ElementList>();

/*FIXME add to about later
    K4AboutData about( "krecipes", 0, , , , K4AboutData::License_GPL, ki18n( "(C) 2003 Unai Garro\n(C) 2004-2006 Jason Kivlighn"),
ki18n("This product is RecipeML compatible.\nYou can get more information about this file format in:\nhttp://www.formatdata.com/recipeml" ),
"http://krecipes.sourceforge.net/" );
	about.addAuthor( ki18n("Unai Garro"), KLocalizedString(), "ugarro@gmail.com", 0 );
	about.addAuthor( ki18n("Jason Kivlighn"), KLocalizedString(), "jkivlighn@gmail.com", 0 );
	about.addAuthor( ki18n("Cyril Bosselut"), KLocalizedString(), "bosselut@b1project.com", "http://b1project.com" );

	about.addCredit( ki18n("Colleen Beamer"), ki18n("Testing, bug reports, suggestions"), "colleen.beamer@gmail.com", 0 );
	about.addCredit( ki18n("Robert Wadley"), ki18n("Icons and artwork"), "rob@robntina.fastmail.us", 0 );
	about.addAuthor( ki18n("Daniel Sauvé"), ki18n("Porting to KDE4"), "megametres@gmail.com", "http://metres.homelinux.com" );

    about.addAuthor( ki18n("Laurent Montel"), ki18n("Porting to KDE4"), "montel@kde.org", 0 );
    about.addAuthor( ki18n("José Manuel Santamaría Lema"), ki18n("Porting to KDE4, current maintainer"), "panfaust@gmail.com", 0 );
    about.addAuthor( ki18n("Martin Engelmann"), ki18n("Porting to KDE4, developer"), "murphi.oss@googlemail.com", 0 );

	about.addCredit( ki18n("Patrick Spendrin"), ki18n("Patches to make Krecipes work under Windows"), "ps_ml@gmx.de", 0 );
	about.addCredit( ki18n("Mike Ferguson"), ki18n("Help with bugs, patches"), "", 0 );
	about.addCredit( ki18n("Warren Severin"), ki18n("Code to export recipes to *.mx2 files"), "", 0 );
	
	about.addCredit( ki18n("Eduardo Robles Elvira"),
		ki18n("He advised using WebKit to fix printing support during Akademy-es 2010."),
		"edulix@gmail.com", 0 );
	about.addCredit( ki18n("José Millán Soto"),
		ki18n("He advised using WebKit to fix printing support during Akademy-es 2010."),
		"", 0 );
*/

    /*if ( !QApplication::start() ) {
		std::cerr << "Krecipes is already running!" << std::endl;
		return 0;
    }*/

    // set up application
    QGuiApplication app(argc, argv );

    QCoreApplication::setApplicationName("Krecipes");
    QCoreApplication::setApplicationVersion(KRECIPES_VERSION);
    QCoreApplication::setOrganizationDomain("kde.org");
    QGuiApplication::setApplicationDisplayName(i18n("Krecipes"));
    QGuiApplication::setDesktopFileName("Krecipes");
    //FIXME: QGuiApplication::setWindowIcon(QIcon::fromTheme("..."));

    // Set up arguments
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(i18n( "The KDE Cookbook" ));

    QCommandLineOption sqlconvert("convert-sqlite3", i18n("Convert the current SQLite 2.x database to SQLite 3 and exit"));
    parser.addOption(sqlconvert);

    // Process the actual command line arguments given by the user
    parser.process(app);

    // see if we are starting with session management
	if ( app.isSessionRestored() ) {
//		RESTORE( Krecipes );
	}
	else {
		// no session.. just start up normally

        QGuiApplication::flush();

        if (parser.isSet("convert-sqlite3")) {
			ConvertSQLite3 sqliteConverter;
			sqliteConverter.convert();
			return 0;
		}

//		Krecipes * widget = new Krecipes;
//		app.setTopWidget( widget );
//		widget->show();
	}

	return app.exec();
}

