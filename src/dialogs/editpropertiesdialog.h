/***************************************************************************
*   Copyright © 2007 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef EDITPROPERTIESDIALOG_H
#define EDITPROPERTIESDIALOG_H

//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>

#include "datablocks/unit.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSplitter;
class QGroupBox;

class RecipeDB;
class QLineEdit;
class ElementList;
class KreListView;

class EditPropertiesDialog : public QDialog
{
	Q_OBJECT

public:
	EditPropertiesDialog( int ingID, const QString&ingName, RecipeDB *db, QWidget* parent = 0 );
	~EditPropertiesDialog();

	QLabel* infoLabel;
	KreListView* usdaListView;
	QPushButton* loadButton;
	QPushButton* propertyAddButton;
	QPushButton* propertyRemoveButton;
    QListWidget* propertyListView;
	QPushButton* weightAddButton;
	QPushButton* weightRemoveButton;
    QListWidget* weightListView;

protected:
	QSplitter* splitter;
	QVBoxLayout* EditPropertiesDialogLayout;
	QVBoxLayout* layout8;
	QVBoxLayout* layout7;
	QHBoxLayout* layout3;
	QHBoxLayout* layout3_2;
	QGroupBox* propertiesBox;
	QGroupBox* weightsBox;

protected slots:
	virtual void languageChange();

private slots:
	void addWeight();
	void removeWeight();
	void updateLists( void );
	void addPropertyToIngredient( void );
	void removePropertyFromIngredient( void );
    void insertPropertyEditBox( QListWidgetItem* it );
	void setPropertyAmount( const QString & amount );
	void setPropertyAmount( double amount );
	void loadUSDAData( void );
    void itemRenamed( QListWidgetItem*, const QPoint &, int col );
	void loadDataFromFile();

private:
	void reloadPropertyList( void );
	void reloadWeightList( void );
    int findPropertyNo( QListWidgetItem *it );

	QLineEdit* inputBox;

	ElementList *perUnitListBack;
	RecipeDB *db;
	QList<QStringList> loaded_data;

	int ingredientID;
	QString ingredientName;
};

#endif // EDITPROPERTIESDIALOG_H
