/**************************************************************************
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>	          *
*	                                                                  *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or	  *
*   (at your option) any later version.	                                  *
***************************************************************************/

#ifndef ADVANCEDSEARCHDIALOG_H
#define ADVANCEDSEARCHDIALOG_H

#include <QWidget>

//FIXME: Port criteriaListView
class K3ListView;

class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QScrollArea;
class QPushButton;
class QFrame;
class QLineEdit;
class QLabel;
class QCheckBox;
class KComboBox;
class QSpinBox;
class QDateTimeEdit;
class QPushButton;
class KreSearchResultListWidget;
class QRadioButton;
class QGroupBox;
class QAction;

class KDateEdit;
class RecipeDB;
class KreRecipeActionsHandler;
class FractionInput;
class CriteriaComboBox;


class AdvancedSearchDialog : public QWidget
{
	Q_OBJECT

public:
	AdvancedSearchDialog( QWidget *parent, RecipeDB * );
	~AdvancedSearchDialog();

	KreRecipeActionsHandler *actionHandler;
	void addAction( QAction * action );

public slots:
	void haveSelectedItems();

protected:
	virtual void showEvent( QShowEvent * event );

	QLabel* searchLabel;
	QScrollArea* scrollArea1;
	QFrame* parametersFrame;
	QLineEdit* titleEdit;
	QPushButton* titleButton;
	QFrame* titleFrame;
	QPushButton* ingredientButton;
	QFrame* ingredientFrame;
	QLineEdit* ingredientsAllEdit;
	QLineEdit* ingredientsAnyEdit;
	QLineEdit* ingredientsWithoutEdit;
	QPushButton* categoriesButton;
	QFrame* categoryFrame;
	QLineEdit* categoriesAllEdit;
	QLineEdit* categoriesAnyEdit;
	QLineEdit* categoriesNotEdit;
	QPushButton* authorsButton;
	QFrame* authorsFrame;
	QLineEdit* authorsAnyEdit;
	QLineEdit* authorsAllEdit;
	QLineEdit* authorsWithoutEdit;
	QPushButton* servingsButton;
	QFrame* servingsFrame;
	QCheckBox* enableServingsCheckBox;
	KComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	QPushButton* prepTimeButton;
	QFrame* prepTimeFrame;
	QCheckBox* enablePrepTimeCheckBox;
	KComboBox* prepTimeComboBox;
	QDateTimeEdit* prepTimeEdit;
	QLineEdit* instructionsEdit;
	QPushButton* instructionsButton;
	QFrame* instructionsFrame;
	QPushButton* clearButton;
	QPushButton* findButton;
	KreSearchResultListWidget* resultsListWidget;
	QCheckBox *requireAllTitle;
	QCheckBox *requireAllInstructions;
	KDateEdit *createdStartDateEdit;
	KDateEdit *createdEndDateEdit;
	KDateEdit *modifiedStartDateEdit;
	KDateEdit *modifiedEndDateEdit;
	KDateEdit *accessedStartDateEdit;
	KDateEdit *accessedEndDateEdit;
	QPushButton* metaDataButton;
	QFrame* metaDataFrame;
	QRadioButton* ratingAvgRadioButton;
	FractionInput* avgStarsEdit;
	QLabel* avgStarsLabel;
	QRadioButton* criterionRadioButton;
	CriteriaComboBox* criteriaComboBox;
	FractionInput* starsWidget;
	QPushButton* addCriteriaButton;
	QPushButton* removeCriteriaButton;
	K3ListView* criteriaListView;
	QPushButton* ratingsButton;
	QGroupBox *ratingButtonGroup;
	QFrame *ratingAvgFrame;
	QFrame *criterionFrame;
	QVBoxLayout *criterionFrameLayout;
	QHBoxLayout *ratingAvgFrameLayout;
	QVBoxLayout *ratingButtonGroupLayout;

	QHBoxLayout* AdvancedSearchDialogLayout;
	QVBoxLayout* layout7;
	QHBoxLayout* scrollView1Layout;
	QVBoxLayout* parametersFrameLayout;
	QSpacerItem* ingredientspacer;
	QSpacerItem* categoryspacer;
	QSpacerItem* authorspacer;
	QSpacerItem* servingsspacer;
	QSpacerItem* titleFrameSpacer;
	QSpacerItem* instructionsFrameSpacer;
	QSpacerItem* metaDataFrameSpacer;
	QSpacerItem* spacer15;
	QVBoxLayout* titleFrameLayout;
	QFormLayout* ingredientFrameLayout;
	QFormLayout* categoryFrameLayout;
	QFormLayout* authorsFrameLayout;
	QVBoxLayout* servingsFrameLayout;
	QHBoxLayout* layout5;
	QVBoxLayout* prepTimeFrameLayout;
	QVBoxLayout* instructionsFrameLayout;
	QVBoxLayout* metaDataFrameLayout;
	QHBoxLayout* layout6;
	QHBoxLayout* layout9;
	QSpacerItem* spacer3;
	QVBoxLayout* ratingsFrameLayout;
	QHBoxLayout* layout11;
	QHBoxLayout* layout12;
	QSpacerItem* ratingsFrameSpacer;

	RecipeDB *database;

signals:
	void recipeSelected( bool );
	void recipeSelected( int, int );
	void recipesSelected( const QList<int> &, int );

private slots:
	void search();
	void clear();
	void titleButtonSwitched( bool );
	void ingredientButtonSwitched( bool );
	void authorsButtonSwitched( bool );
	void categoriesButtonSwitched( bool );
	void servingsButtonSwitched( bool );
	void prepTimeButtonSwitched( bool );
	void instructionsButtonSwitched( bool );
	void metaDataButtonSwitched( bool );
	void ratingsButtonSwitched( bool );
	void activateRatingOptionAvg();
	void activateRatingOptionCriterion();
	void slotAddRatingCriteria();
	void slotRemoveRatingCriteria();

private:
	QStringList split( const QString &text, bool sql_wildcards = false ) const;
};

#endif //ADVANCEDSEARCHDIALOG_H

