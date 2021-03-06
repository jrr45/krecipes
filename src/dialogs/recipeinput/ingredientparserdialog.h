/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTPARSERDIALOG_H
#define INGREDIENTPARSERDIALOG_H

#include <QDialog>

#include "datablocks/ingredientlist.h"
#include "datablocks/unit.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class KTextEdit;
class QPushButton;
class QListWidget;
class QTreeWidgetItem;
class QPushButton;

class IngredientParserDialog : public QDialog
{
	Q_OBJECT

public:
	explicit IngredientParserDialog( const UnitList &units, QWidget* parent = 0, const char* name = 0 );
	~IngredientParserDialog();

	IngredientList ingredients() const { return m_ingList; }

protected:
	QLabel* textLabel1;
	QLabel *previewLabel;
	KTextEdit* ingredientTextEdit;
	QPushButton* parseButton;
	QListWidget* previewIngView;
	QPushButton* buttonGroup;

protected slots:
	virtual void accept();
	void parseText();
	void removeIngredient();

	//Set Header button slot
	virtual void convertToHeader();
	virtual void languageChange();

private:
	void convertToHeader( const QList<QTreeWidgetItem*> &items );

	UnitList m_unitList;
	IngredientList m_ingList;
};

#endif // INGREDIENTPARSERDIALOG_H
