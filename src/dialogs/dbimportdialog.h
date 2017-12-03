/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef DBIMPORTDIALOG_H
#define DBIMPORTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QButtonGroup>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QRadioButton;
class QStackedWidget;
class QWidget;
class KLineEdit;
class QLabel;

class KUrlRequester;
class QSpinBox;

/**
@author Jason Kivlighn
*/

class DBImportDialog: public QDialog
{
	Q_OBJECT
public:
	explicit DBImportDialog( QWidget *parent = 0, const char *name = 0 );

	QString dbType() const;
	void serverParams( QString &host, QString &user, QString &pass, int &port, QString &table ) const;
	QString dbFile() const;

private:
	// Widgets
    QButtonGroup* dbButtonGroup;
	QRadioButton* liteRadioButton;
	QRadioButton* mysqlRadioButton;
	QRadioButton* psqlRadioButton;
	QStackedWidget* paramStack;
	QWidget* serverPage;
	KLineEdit* nameEdit;
	QLabel* passwordLabel;
	KLineEdit* hostEdit;
	KLineEdit* passwordEdit;
	KLineEdit* userEdit;
	QLabel* userLabel;
	QLabel* hostLabel;
	QLabel* nameLabel;
	QLabel* portLabel;
	QSpinBox *portEdit;
	QWidget* sqlitePage;
	KUrlRequester* sqliteDBRequester;

	QVBoxLayout* dbButtonGroupLayout;
	QVBoxLayout* serverPageLayout;
	QGridLayout* layout5;
	QVBoxLayout* serverPageLayout_2;

protected slots:
	void languageChange();
	void switchDBPage( int id );

};

#endif
