/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as publishfed by *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "kremenu.h"

#include <qbitmap.h>
#include <qcursor.h>
#include <qfont.h>
#include <qimage.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsignalmapper.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QChildEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kimageeffect.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

KreMenu::KreMenu( QWidget *parent, const char *name ) :
#if QT_VERSION >= 0x030200
		QWidget( parent, name, Qt::WNoAutoErase )
#else
		QWidget( parent, name )
#endif
{
	Menu newMenu;

	mainMenuId = menus.append( newMenu );

	currentMenuId = mainMenuId;
	m_currentMenu = &( *currentMenuId );

	setMouseTracking( true );
	//setFocusPolicy( QWidget::StrongFocus );FIXME:Qt4
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
}


KreMenu::~KreMenu()
{}

void KreMenu::childEvent ( QChildEvent *e )
{
	if ( e->type() == QChildEvent::ChildInserted ) {

		QObject * child = e->child();
		if ( child->inherits( "KreMenuButton" ) ) {
			KreMenuButton * button = ( KreMenuButton* ) child;

			Menu *buttonMenu = &( *( button->menuId ) );



			if ( !( buttonMenu->activeButton ) )   // Highlight the button if it's the first in the menu
			{
				button->setActive( true );
				buttonMenu->activeButton = button;
			}

			buttonMenu->addButton( button );

			if ( buttonMenu != m_currentMenu )
				button->hide();
			else
				button->show();

			connect ( button, SIGNAL( clicked( KreMenuButton* ) ), this, SLOT( collectClicks( KreMenuButton* ) ) );
		}
	}
	else if ( e->type() == QChildEvent::ChildRemoved ) {
		QObject * child = e->child();

		KreMenuButton *button = ( KreMenuButton* ) child;
		if ( m_currentMenu->positionList.find( button ) != m_currentMenu->positionList.end() )  // Ensure that what was removed was a button
		{
			// Remove the button from the list first
			int pos = m_currentMenu->positionList[ button ]; // FIXME: this works only if the button is removed from the main menu
			m_currentMenu->widgetList.remove( pos ); // FIXME: this works only if the button is removed from the main menu
			m_currentMenu->positionList.remove( button ); // FIXME: this works only if the button is removed from the main menu

			// Now recalculate the position of the next button
			( m_currentMenu->widgetNumber ) --; // FIXME: this works only if the button is removed from the main menu

			KreMenuButton *lastButton = m_currentMenu->widgetList[ ( m_currentMenu->widgetNumber ) - 1 ];
			if ( lastButton )
				m_currentMenu->childPos = lastButton->y() + lastButton->height();
			m_currentMenu->activeButton = 0;

			setMinimumWidth( minimumSizeHint().width() + 10 ); //update the minimum width
		}

	}
	QWidget::childEvent( e );
}

void KreMenu::collectClicks( KreMenuButton *w )
{
	setFocus();

	highlightButton( w );

	// Emit signal indicating button activation with button ID
	KrePanel panel = w->getPanel();
	emit clicked( panel );
}

MenuId KreMenu::createSubMenu( const QString &title, const QString &icon )
{

	// Create the new menu
	Menu newMenu;
	MenuId id = menus.append( newMenu );

	// Add a button to the main menu for this submenu
	KIconLoader il;
	KreMenuButton *newMenuButton = new KreMenuButton( this );
	newMenuButton->subMenuId = id;
	newMenuButton->setTitle( title );
	newMenuButton->setIconSet( il.loadIconSet( icon, KIcon::Panel ) );

	// Add a button to the submenu to go back to the top menu
	KreMenuButton *newSubMenuButton = new KreMenuButton( this );
	newSubMenuButton->menuId = id;
	newSubMenuButton->subMenuId = mainMenuId;
	newSubMenuButton->setTitle( i18n( "Up" ) );
	newSubMenuButton->setIconSet( il.loadIconSet( "1uparrow", KIcon::Panel ) );

	connect( newMenuButton, SIGNAL( clicked( MenuId ) ), this, SLOT( showMenu( MenuId ) ) );
	connect( newSubMenuButton, SIGNAL( clicked( MenuId ) ), this, SLOT( showMenu( MenuId ) ) );


	return id;
}

void KreMenu::highlightButton( KreMenuButton *button )
{
	MenuId buttonMenuId = button->menuId;
	Menu *buttonMenu = &( *buttonMenuId );

	//Deactivate the old button
	if ( buttonMenu->activeButton ) {
		buttonMenu->activeButton->setActive( false );
		buttonMenu->activeButton->update();
	}

	//Activate the new button

	button->setActive( true );
	button->update();
	buttonMenu->activeButton = button;
}

void KreMenu::keyPressEvent( QKeyEvent *e )
{
	switch ( e->key() ) {
	case Qt::Key_Up: {
			int current_index = m_currentMenu->positionList[ m_currentMenu->activeButton ];
			if ( current_index > 0 ) {
				highlightButton( m_currentMenu->widgetList[ current_index - 1 ] );

				//simulate a mouse click
				QMouseEvent me( QEvent::MouseButtonPress, QPoint(), 0, 0 );
				KApplication::sendEvent( m_currentMenu->activeButton, &me );

				e->accept();
			}
			break;
		}
	case Qt::Key_Down: {
			int current_index = m_currentMenu->positionList[ m_currentMenu->activeButton ];
			if ( current_index < int( m_currentMenu->positionList.count() ) - 1 ) {
				highlightButton( m_currentMenu->widgetList[ current_index + 1 ] );

				//simulate a mouse click
				QMouseEvent me( QEvent::MouseButtonPress, QPoint(), 0, 0 );
				KApplication::sendEvent( m_currentMenu->activeButton, &me );

				e->accept();
			}
			break;
		}
	case Qt::Key_Enter:
	case Qt::Key_Return:
	case Qt::Key_Space: {
			//simulate a mouse click
			QMouseEvent me( QEvent::MouseButtonPress, QPoint(), 0, 0 );
			KApplication::sendEvent( m_currentMenu->activeButton, &me );

			e->accept();
			break;
		}
	default:
		e->ignore();
	}
}

QSize KreMenu::sizeHint() const
{
	return minimumSizeHint();
}

//the minimum size hint will be the minimum size hint of the largest child
QSize KreMenu::minimumSizeHint() const
{
	int width = 30;

	QObjectList childElements = queryList( 0, 0, false, false ); //only first-generation children (not recursive)
	QListIterator<QObject*> it( childElements );

	QObject *obj;
	while ( it.hasNext() ) {
		obj = it.next();

		if ( obj->isWidgetType() ) {
			int obj_width_hint = ( ( QWidget* ) obj ) ->minimumSizeHint().width();

			if ( obj_width_hint > width )
				width = obj_width_hint;
		}
	}

	return QSize( width, 150 );
}

void KreMenu::paintEvent( QPaintEvent * )
{
	// Make sure the size is bigger than the minimum necessary
	//if (minimumWidth() <45) setMinimumWidth(45); // FIXME: can somehow setMinimumWidth be restricted? This may not be the best place to do this

	// Get gradient colors
	QColor c = colorGroup().button();
	QColor c1 = c.dark( 130 );
	QColor c2 = c.light( 120 );

	// Draw the gradient
	KPixmap kpm;
	kpm.resize( size() );
	KPixmapEffect::unbalancedGradient ( kpm, c2, c1, KPixmapEffect::HorizontalGradient, -150, -150 );

	// Draw the handle
	QPainter painter( &kpm );
	painter.setPen( c1 );
	painter.drawLine( width() - 5, 20, width() - 5, height() - 20 );
	painter.end();

	//Set the border transparent using a mask
	QBitmap mask( kpm.size() );
	mask.fill( Qt::color0 );
	painter.begin( &mask );
	painter.setPen( Qt::color1 );
	painter.setBrush( Qt::color1 );
	painter.drawRoundRect( 0, 0, width(), height(), ( int ) ( 2.0 / width() * height() ), 2 );
	painter.end();
	kpm.setMask( mask );

	//Draw the border line
	painter.begin( &kpm );
	painter.setPen( c1 );
	painter.drawRoundRect( 0, 0, width(), height(), ( int ) ( 2.0 / width() * height() ), 2 );

	//Draw the top line bordering with the first button
	if ( m_currentMenu->activeButton )  // draw only if there's a button
	{
		int w = m_currentMenu->activeButton->width();
		painter.setPen( c1 );
		painter.drawLine( w / 5, 8, w - 1, 8 );
		painter.setPen( c2 );
		painter.drawLine( w / 5, 9, w - 1, 9 );
	}

	painter.end();

	// Copy the pixmap to the widget
	bitBlt( this, 0, 0, &kpm );
}

void KreMenu::resizeEvent( QResizeEvent* e )
{
	emit resized( ( e->size() ).width(), ( e->size() ).height() );
}

void KreMenu::showMenu( MenuId id )
{

	// Hide the buttons in the current menu
	// and show the ones in the new menu

	QObjectList childElements = queryList();
	QListIterator<QObject*> it( childElements );

	QObject *obj;
	while ( it.hasNext() ) {
		obj = it.next();
		if ( obj->inherits( "KreMenuButton" ) ) {
			KreMenuButton * button = ( KreMenuButton* ) obj;
			if ( button->menuId == currentMenuId )
				button->hide();
			else if ( button->menuId == id )
				button->show();
		}
	}


	// Set the new menu as current
	currentMenuId = id;
	m_currentMenu = &( *( currentMenuId ) );
}





KreMenuButton::KreMenuButton( KreMenu *parent, KrePanel _panel, MenuId id, const char *name ) :
#if QT_VERSION >= 0x030200
		QWidget( parent, name, Qt::WNoAutoErase ),
#else
		QWidget( parent, name ),
#endif
		panel( _panel )
{
	icon = 0;
	highlighted = false;
	text = QString::null;

	menuId = id;

	//FIXME:Qt4 subMenuId = 0; // By default it's not a submenu button

	resize( parent->size().width(), 55 );
	connect ( parent, SIGNAL( resized( int, int ) ), this, SLOT( rescale() ) );
	connect( this, SIGNAL( clicked() ), this, SLOT( forwardClicks() ) );
	setCursor( QCursor( KCursor::handCursor() ) );
}

KreMenuButton::KreMenuButton( KreMenu *parent, KrePanel _panel ) :
#if QT_VERSION >= 0x030200
		QWidget( parent, "", Qt::WNoAutoErase ),
#else
		QWidget( parent ),
#endif
		panel( _panel )
{
	icon = 0;
	highlighted = false;
	text = QString::null;

	menuId = parent->mainMenu();

	//FIXME:Qt4 subMenuId = 0; // By default it's not a submenu button

	resize( parent->size().width(), 55 );
	connect ( parent, SIGNAL( resized( int, int ) ), this, SLOT( rescale() ) );
	connect( this, SIGNAL( clicked() ), this, SLOT( forwardClicks() ) );
	setCursor( QCursor( KCursor::handCursor() ) );
}


KreMenuButton::~KreMenuButton()
{
	delete icon;
}

void KreMenuButton::setTitle( const QString &s )
{
	text = s;

#if 0 //this causes problems for the button to go back to editing a recipe
	//adjust text to two lines if needed
	if ( fontMetrics().width( text ) > 110 ) {
		text.replace( ' ', "\n" );
	}
#endif

	setMinimumWidth( minimumSizeHint().width() );
	if ( parentWidget() ->minimumWidth() < minimumSizeHint().width() )
		parentWidget() ->setMinimumWidth( minimumSizeHint().width() + 10 );

	update();
}

void KreMenuButton::mousePressEvent ( QMouseEvent * )
{
	emit clicked();
}

void KreMenuButton::rescale()
{
	resize( parentWidget() ->width() - 10, height() );
}
QSize KreMenuButton::sizeHint() const
{
	if ( parentWidget() )
		return ( QSize( parentWidget() ->size().width() - 10, 40 ) );
	else
		return QSize( 100, 30 );
}

QSize KreMenuButton::minimumSizeHint() const
{
	int text_width = QMAX( fontMetrics().width( text.section( '\n', 0, 0 ) ), fontMetrics().width( text.section( '\n', 1, 1 ) ) );

	if ( icon )
		return QSize( 40 + icon->width() + text_width, 30 );
	else
		return QSize( 40 + text_width, 30 );
}

void KreMenuButton::paintEvent( QPaintEvent * )
{
	if ( !isShown() )
		return ;
	// First draw the gradient
	int darken = 130, lighten = 120;
	QColor c1, c2, c1h, c2h; //non-highlighted and highlighted versions

	// Set the gradient colors

	c1 = colorGroup().button().dark( darken );
	c2 = colorGroup().button().light( lighten );

	if ( highlighted ) {
		darken -= 10;
		lighten += 10;
		c1h = KGlobalSettings::highlightColor().dark( darken );
		c2h = KGlobalSettings::highlightColor().light( lighten );
	}

	// draw the gradient now

	QPainter painter;
	KPixmap kpm;
	kpm.resize( ( ( QWidget * ) parent() ) ->size() ); // use parent's same size to obtain the same gradient

	if ( !highlighted ) {

		// first the gradient
		KPixmapEffect::unbalancedGradient ( kpm, c2, c1, KPixmapEffect::HorizontalGradient, -150, -150 );

	}
	else {

		// top gradient (highlighted)
		kpm.resize( width(), height() );
		KPixmapEffect::unbalancedGradient ( kpm, c2h, c1h, KPixmapEffect::HorizontalGradient, -150, -150 );
		// low gradient besides the line (not hightlighted)
		KPixmap kpmb;
		kpmb.resize( width(), 2 );
		KPixmapEffect::unbalancedGradient ( kpmb, c2, c1, KPixmapEffect::HorizontalGradient, -150, -150 );
		// mix the two
		bitBlt( &kpm, 0, height() - 2, &kpmb );

	}

	// Draw the line
	painter.begin( &kpm );
	painter.setPen( colorGroup().button().dark( darken ) );
	painter.drawLine( width() / 5, height() - 2, width() - 1, height() - 2 );
	painter.setPen( colorGroup().button().light( lighten ) );
	painter.drawLine( width() / 5, height() - 1, width() - 1, height() - 1 );
	painter.end();


	// Now Add the icon

	painter.begin( &kpm );
	int xPos, yPos;
	if ( icon ) {
		// Set the icon's desired horizontal position

		xPos = 10;
		yPos = 0;


		// Make sure it fits in the area
		// If not, resize and reposition horizontally to be centered

		QPixmap scaledIcon = *icon;

		if ( ( icon->height() > height() ) || ( icon->width() > width() / 3 ) )  // Nice effect, make sure you take less than half in width and fit in height (try making the menu very short in width)
		{
			QImage image;
			image = ( *icon );
			scaledIcon.convertFromImage( image.smoothScale( width() / 3, height(), Qt::KeepAspectRatio ) );
		}

		// Calculate the icon's vertical position

		yPos = ( height() - scaledIcon.height() ) / 2 - 1;


		// Now draw it

		painter.drawPixmap( xPos, yPos, scaledIcon );

		xPos += scaledIcon.width(); // increase it to place the text area correctly
	}

	painter.end();

	// If it's highlighted, draw a rounded area around the text

	// Calculate the rounded area

	int areax = xPos + 10;
	int areah = fontMetrics().height() * ( text.count( '\n' ) + 1 ) + fontMetrics().lineSpacing() * text.count( '\n' ) + 6; // Make sure the area is sensible for text and adjust for multiple lines

	int areaw = width() - areax - 10;

	if ( areah > ( height() - 4 ) ) {
		areah = height() - 4; // Limit to button height
	}

	int areay = ( height() - areah - 2 ) / 2 + 1; // Center the area vertically


	// Calculate roundness

	int roundy = 99, roundx = ( int ) ( ( float ) roundy * areah / areaw ); //Make corners round


	if ( highlighted && areaw > 0 )  // If there is no space for the text area do not draw it
	{

		// Draw the gradient
		KPixmap area;
		area.resize( areaw, areah );


		KPixmapEffect::gradient( area, c2h.light( 150 ), c1h.light( 150 ), KPixmapEffect::VerticalGradient );

		painter.begin( &area );
		painter.setPen( c1h );
		painter.setBrush( Qt::NoBrush );
		painter.drawRoundRect( 0, 0, areaw, areah, roundx, roundy );
		painter.end();

		// Make it round
		QBitmap mask( QSize( areaw, areah ) );
		mask.fill( Qt::color0 );
		painter.begin( &mask );
		painter.setPen( Qt::color1 );
		painter.setBrush( Qt::color1 );
		painter.drawRoundRect( 0, 0, areaw, areah, roundx, roundy );
		painter.end();
		area.setMask( mask );

		// Copy it to the button
		bitBlt( &kpm, areax, areay, &area );
	}

	// Finally, draw the text besides the icon
	QRect r = rect();
	r.setLeft( areax + 5 );
	r.setWidth( areaw - 10 );

	painter.begin( &kpm );
	if ( highlighted )
		painter.setPen( KGlobalSettings::highlightedTextColor() );
	else
		painter.setPen( KGlobalSettings::textColor() );
	painter.setClipRect( r );
	painter.drawText( r, Qt::AlignVCenter, text );
	painter.end();

	// Copy the offscreen button to the widget
	bitBlt( this, 0, 0, &kpm, 0, 0, width(), height() ); // Copy the image with correct button size (button is already smaller than parent in width to leave space for the handle, so no need to use -10)

}

void KreMenuButton::setIconSet( const QIcon &is )
{
	delete icon;

	icon = new QPixmap( is.pixmap( QIcon::Small, QIcon::Normal, QIcon::On ) );

	setMinimumWidth( minimumSizeHint().width() );
	if ( parentWidget() ->minimumWidth() < minimumSizeHint().width() )
		parentWidget() ->setMinimumWidth( minimumSizeHint().width() + 10 );
}

Menu::Menu( void )
{
	childPos = 10; // Initial button is on top (10px), then keep scrolling down
	widgetNumber = 0; // Initially we have no buttons
	activeButton = 0; // Button that is highlighted
}


Menu::Menu( const Menu &m )
{
	activeButton = m.activeButton;
	childPos = m.childPos;
	widgetNumber = m.widgetNumber;

	copyMap( positionList, m.positionList );
	copyMap( widgetList, m.widgetList );
}

Menu::~Menu( void )
{}

Menu& Menu::operator=( const Menu &m )
{

	activeButton = m.activeButton;
	childPos = m.childPos;
	widgetNumber = m.widgetNumber;

	copyMap( positionList, m.positionList );
	copyMap( widgetList, m.widgetList );

	return *this;
}


void Menu::addButton( KreMenuButton* button )
{
	button->move( 0, childPos );
	button->rescale();
	childPos += button->height();
	positionList[ button ] = widgetNumber; // Store index for this widget, and increment number
	widgetList[ widgetNumber ] = button; // Store the button in the list (the inverse mapping of the previous one)
	widgetNumber++;
}

void Menu::copyMap( QMap <int, KreMenuButton*> &destMap, const QMap <int, KreMenuButton*> &origMap )
{
	QMap<int, KreMenuButton*>::ConstIterator it;
	destMap.clear();
	for ( it = origMap.begin(); it != origMap.end(); ++it ) {
		destMap[ it.key() ] = it.data();
	}
}

void Menu::copyMap( QMap <KreMenuButton*, int> &destMap, const QMap <KreMenuButton*, int> &origMap )
{
	QMap<KreMenuButton*, int>::ConstIterator it;
	destMap.clear();
	for ( it = origMap.begin(); it != origMap.end(); ++it ) {
		destMap[ it.key() ] = it.data();
	}
}

#include "kremenu.moc"
