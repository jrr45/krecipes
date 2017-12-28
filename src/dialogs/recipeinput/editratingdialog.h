/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef EDITRATINGDIALOG_H
#define EDITRATINGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <kvbox.h>

class KHBox;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class KComboBox;
class QPushButton;
class QTreeWidgetItem;
class QTextEdit;
class QLineEdit;
class KRatingWidget;

class Rating;
class RatingCriteria;
class ElementList;
class RatingCriteriaListView;

class EditRatingDialog : public QDialog
{
Q_OBJECT

public:
	explicit EditRatingDialog( const ElementList &criteriaList, const Rating &, QWidget* parent = 0, const char* name = 0 );
	explicit EditRatingDialog( const ElementList &criteriaList, QWidget* parent = 0, const char* name = 0 );
	~EditRatingDialog();

	QLabel* criteriaLabel;
	KComboBox* criteriaComboBox;
	QLabel* starsLabel;
	KRatingWidget *starsWidget;
	QPushButton* addButton;
	QPushButton* removeButton;
	RatingCriteriaListView* criteriaListView;
	QLabel* commentsLabel;
    QTextEdit* commentsEdit;
	QLabel* raterLabel;
	QLineEdit* raterEdit;

	Rating rating() const;

protected:
	KHBox* layout8;
	KHBox* layout2;

protected slots:
	virtual void languageChange();
	void slotAddRatingCriteria();
	void slotRemoveRatingCriteria();
	void itemRenamed(QTreeWidgetItem* it, const QString &, int c);

private:
	void init(const ElementList &criteriaList);

	void loadRating( const Rating & );
	void addRatingCriteria( const RatingCriteria &rc );

	int ratingID;
};

#endif // EDITRATINGDIALOG_H
