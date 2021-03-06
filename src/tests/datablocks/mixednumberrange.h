/****************************************************************************
 *   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 ****************************************************************************/

#ifndef TESTMIXEDNUMBERRANGE_H
#define TESTMIXEDNUMBERRANGE_H

#include <QtCore/QObject>


class TestMixedNumberRange : public QObject
{
    Q_OBJECT

private slots:
	void testFromString_data();
	void testFromString();

	void testToString_data();
	void testToString();

	void testToFromString_data();
	void testToFromString();
};


#endif
