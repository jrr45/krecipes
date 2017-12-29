/***************************************************************************
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "usda_ingredient_data.h"

#include <QList>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>

namespace USDA {

	bool localizedIngredientsAvailable()
	{
        return !QStandardPaths::locate(QStandardPaths::DataLocation, "data/ingredient-data-" + QLocale().uiLanguages().first() + ".txt" ).isEmpty();
	}

    QList<IngredientData> loadIngredients()
	{
        QList<IngredientData> result;

        QString dataFilename = QStandardPaths::locate(QStandardPaths::DataLocation, "data/ingredient-data-" + QLocale().uiLanguages().first() + ".txt" );
		if ( dataFilename.isEmpty() ) {
            qDebug() << "No localized property data available for " << QLocale().uiLanguages().first() ;
	
			dataFilename = QStandardPaths::locate(QStandardPaths::DataLocation, "data/ingredient-data-en_US.txt" ); //default to English
		}

		QFile dataFile( dataFilename );
		if ( dataFile.open( QIODevice::ReadOnly ) ) {
			qDebug() << "Loading: " << dataFilename ;
			QTextStream stream( &dataFile );

			QString line;
			while ( !(line = stream.readLine()).isEmpty()) {
				if ( line.trimmed().isEmpty() ) continue;

				IngredientData data;

				int sepIndex = line.indexOf(':');
				if ( sepIndex != -1 ) {
					data.name = line.left(sepIndex);
					data.usda_id = line.right(line.length()-sepIndex-1).toInt();
				}
				else {
					data.name = line;
					data.usda_id = -1;
				}

				result << data;
			}

			dataFile.close();
		}
		else
			qDebug() << "Unable to find or open property data file (ingredient-data-en_US.sql)" ;

		return result;
	}
} //namespace USDA

