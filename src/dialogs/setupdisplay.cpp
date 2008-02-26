/***************************************************************************
*   Copyright (C) 2003-2005                                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "setupdisplay.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfontdialog.h>
#include <kcolordialog.h>
#include <klocale.h>
#include <kmenu.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <kdialog.h>

#include <khtmlview.h>
#include <dom/dom_doc.h>
#include <dom/css_rule.h>

#include <qinputdialog.h>
#include <qaction.h>
#include <qlabel.h>
#include <qfile.h>
#include <qregexp.h>
#include <q3textedit.h>
#include <qtooltip.h>
#include <qobject.h>
#include <q3valuelist.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <Q3ActionGroup>
#include <Q3Action>
#include <QTextDocument>

#include "datablocks/mixednumber.h"
#include "dialogs/borderdialog.h"
#include "exporters/htmlexporter.h"
#include "exporters/xsltexporter.h"

#include <cmath>

KreDisplayItem::KreDisplayItem( const QString &n, const QString &_name ) : nodeId(n), name(_name)
{
	clear();
}

void KreDisplayItem::clear()
{
	alignment = -1;
	show = true;
	backgroundColor = QColor();
	textColor = QColor();
	columns = 1;
}

SetupDisplay::SetupDisplay( const Recipe &sample, QWidget *parent ) : KHTMLPart(parent),
		box_properties( new PropertiesMap ),
		node_item_map( new QMap<QString, KreDisplayItem*> ),
		has_changes( false ),
		popup(0)
{
	connect( this, SIGNAL( popupMenu(const QString &,const QPoint &) ), SLOT( nodeClicked(const QString &,const QPoint &) ) );

	if ( sample.recipeID != -1 )
		m_sample = sample;
	else {
		m_sample.title = i18n("Recipe Title");
		m_sample.yield.amount = 1;
		m_sample.yield.type = i18n("serving");
		m_sample.categoryList.append( Element(i18n( "Category 1, Category 2, ..." ) ) );
		m_sample.instructions = i18n("Instructions");
		m_sample.prepTime = QTime(0,0);
	
		m_sample.authorList.append( Element(i18n( "Author 1, Author 2, ..." )) );
	
		Ingredient ing;
		ing.name = i18n("Ingredient 1");
		m_sample.ingList.append( ing );
	
		ing.name = i18n("Ingredient 2");
		m_sample.ingList.append( ing );

		ing.name = "...";
		m_sample.ingList.append( ing );
	
		RatingCriteria rc;
		Rating rating1;
		rating1.rater = i18n("Rater");
		rating1.comment = i18n("Comment");
	
		rc.name = i18n("Criteria 1");
		rc.stars = 5.0;
		rating1.append(rc);
	
		rc.name = i18n("Criteria 2");
		rc.stars = 2.5;
		rating1.append(rc);

		IngredientProperty prop;
		prop.name = i18n("Property 1");
		m_sample.properties.append(prop);
		prop.name = i18n("Property 2");
		m_sample.properties.append(prop);
		prop.name = "...";
		m_sample.properties.append(prop);

		m_sample.ratingList.append(rating1);
	}

	kDebug()<<"first load"<<endl;
	loadHTMLView();
	show();

	createItem( "background", i18n("Background"), Font | BackgroundColor | TextColor );
	createItem( "title", i18n("Title"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "instructions", i18n("Instructions"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "prep_time", i18n("Preparation Time"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "photo", i18n("Photo"), Visibility | Border );
	createItem( "authors", i18n("Authors"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "categories", i18n("Categories"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "ingredients", i18n("Ingredients"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "properties", i18n("Properties"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "ratings", i18n("Ratings"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
	createItem( "yield", i18n("Yield"), Font | BackgroundColor | TextColor | Visibility | Alignment | Border );
}

SetupDisplay::~SetupDisplay()
{
	delete box_properties;
	delete node_item_map;
}

void SetupDisplay::loadHTMLView( const QString &templateFile, const QString &styleFile )
{
	kDebug()<<"loading template: "<<templateFile<<" style: "<<styleFile<<endl;
	QString tmp_filename = KStandardDirs::locateLocal( "tmp", "krecipes_recipe_view" );
	XSLTExporter exporter( tmp_filename + ".html", "html" );
	if ( templateFile != QString::null )
		exporter.setTemplate( templateFile );
	exporter.setStyle( styleFile );

	RecipeList recipeList;
	recipeList.append(m_sample);

	QFile file(tmp_filename + ".html");
	if ( file.open( QIODevice::WriteOnly ) ) {
		Q3TextStream stream(&file);
		exporter.writeStream(stream,recipeList);
	}
	else {
		kDebug()<<"Unable to open file for writing"<<endl;
	}
	file.close();

	KUrl url;
	url.setPath( tmp_filename + ".html" );
	kDebug() << "Opening URL: " << Qt::escape(url.prettyUrl()) << endl;

	//KDE4 port
	KParts::OpenUrlArguments argsUrl ( arguments() ); 
	argsUrl.setReload( true ); // Don't use the cache
	setArguments( argsUrl );
	
	KParts::BrowserArguments args;
	browserExtension()->setBrowserArguments(args);

	openUrl( url );
}

void SetupDisplay::reload()
{
	loadHTMLView( m_activeTemplate, m_activeStyle );
}

void SetupDisplay::loadTemplate( const QString &filename )
{
	bool storeChangedState = has_changes;
	KTemporaryFile tmpFile;
	saveLayout(tmpFile.name());
	has_changes = storeChangedState; //saveLayout() sets changes to false
	
	loadHTMLView( filename, tmpFile.name() );
	
	//KDE4 port
	//tmpFile.unlink();

	m_activeTemplate = filename;
}

void SetupDisplay::createItem( const QString &node, const QString &name, unsigned int properties )
{
	KreDisplayItem * item = new KreDisplayItem( node, name );
	box_properties->insert( item, properties );
	node_item_map->insert( node, item );
}

void SetupDisplay::loadLayout( const QString &filename )
{
	QMap<QString,KreDisplayItem*>::iterator it;
	for ( it = node_item_map->begin(); it != node_item_map->end(); ++it ) {
		it.value()->clear();
	}

	m_activeStyle = filename;

	if ( filename.isEmpty() ) {
		loadHTMLView(m_activeTemplate, filename);
		has_changes = false;
		return;
	}

	QFile input( filename );
	if ( input.open( QIODevice::ReadOnly ) ) {
		QDomDocument doc;
		QString error;
		int line;
		int column;
		if ( !doc.setContent( &input, &error, &line, &column ) ) {
			kDebug() << QString( i18n( "\"%1\" at line %2, column %3.  This may not be a Krecipes layout file." ) ).arg( error ).arg( line ).arg( column ) << endl;
			return ;
		}

		processDocument( doc );

		loadHTMLView(m_activeTemplate, filename);
		m_activeStyle = filename;

		has_changes = false;
	}
	else
		kDebug() << "Unable to open file: " << filename << endl;
}

void SetupDisplay::beginObject( const QString &object )
{
	QMap<QString, KreDisplayItem*>::iterator map_it = node_item_map->find( object );
	if ( map_it != node_item_map->end() )
		m_currentItem = map_it.value();
	else
		m_currentItem = 0;
}

void SetupDisplay::endObject()
{
	m_currentItem = 0;
}

void SetupDisplay::loadBackgroundColor( const QString &/*object*/, const QColor &color )
{
	if ( m_currentItem ) {
		m_currentItem->backgroundColor = color;
	}
}

void SetupDisplay::loadFont( const QString &/*object*/, const QFont &font )
{
	if ( m_currentItem ) {
		m_currentItem->font = font;
	}
}

void SetupDisplay::loadTextColor( const QString &/*object*/, const QColor &color )
{
	if ( m_currentItem ) {
		m_currentItem->textColor = color;
	}
}

void SetupDisplay::loadVisibility( const QString &/*object*/, bool visible )
{
	if ( m_currentItem ) {
		m_currentItem->show = visible;
		emit itemVisibilityChanged( m_currentItem, visible );
	}
}

void SetupDisplay::loadAlignment( const QString &/*object*/, int alignment )
{
	if ( m_currentItem ) {
		m_currentItem->alignment = alignment;
	}
}

void SetupDisplay::loadBorder( const QString &/*object*/, const KreBorder& border )
{
	if ( m_currentItem ) {
		m_currentItem->border = border;
	}
}

void SetupDisplay::loadColumns( const QString &/*object*/, int cols )
{
	if ( m_currentItem ) {
		m_currentItem->columns = cols;
	}
}

void SetupDisplay::loadSize( const QString &/*object*/, const QSize &size )
{
	if ( m_currentItem ) {
		m_currentItem->size = size;
	}
}

void SetupDisplay::saveLayout( const QString &filename )
{
	QDomImplementation dom_imp;
	QDomDocument doc = dom_imp.createDocument( QString::null, "krecipes-layout", dom_imp.createDocumentType( "krecipes-layout", QString::null, QString::null ) );

	QDomElement layout_tag = doc.documentElement();
	layout_tag.setAttribute( "version", 0.5 );
	//layout_tag.setAttribute( "generator", QString("Krecipes v%1").arg(krecipes_version()) );
	doc.appendChild( layout_tag );

	for ( QMap<QString, KreDisplayItem*>::const_iterator it = node_item_map->begin(); it != node_item_map->end(); ++it ) {
		QDomElement base_tag = doc.createElement( it.key() );
		layout_tag.appendChild( base_tag );

		int properties = (*box_properties)[it.value()];
		if ( properties & BackgroundColor && it.value()->backgroundColor.isValid() ) {
			QDomElement backgroundcolor_tag = doc.createElement( "background-color" );
			backgroundcolor_tag.appendChild( doc.createTextNode( it.value()->backgroundColor.name() ) );
			base_tag.appendChild( backgroundcolor_tag );
		}

		if ( properties & TextColor && it.value()->textColor.isValid() ) {
			QDomElement textcolor_tag = doc.createElement( "text-color" );
			textcolor_tag.appendChild( doc.createTextNode( it.value()->textColor.name() ) );
			base_tag.appendChild( textcolor_tag );
		}

		if ( properties & Font && it.value()->font != QFont() ) {
			QDomElement font_tag = doc.createElement( "font" );
			font_tag.appendChild( doc.createTextNode( it.value()->font.toString() ) );
			base_tag.appendChild( font_tag );
		}

		if ( properties & Visibility ) {
			QDomElement visibility_tag = doc.createElement( "visible" );
			visibility_tag.appendChild( doc.createTextNode( (it.value()->show) ? "true" : "false" ) );
			base_tag.appendChild( visibility_tag );
		}

		if ( properties & Alignment && it.value()->alignment >= 0 ) {
			QDomElement alignment_tag = doc.createElement( "alignment" );
			alignment_tag.appendChild( doc.createTextNode( QString::number( it.value()->alignment ) ) );
			base_tag.appendChild( alignment_tag );
		}

		if ( properties & Border ) {
			QDomElement border_tag = doc.createElement( "border" );
			border_tag.setAttribute( "width", it.value()->border.width );
			border_tag.setAttribute( "style", it.value()->border.style );
			border_tag.setAttribute( "color", it.value()->border.color.name() );
			base_tag.appendChild( border_tag );
		}

		if ( properties & Columns ) {
			QDomElement columns_tag = doc.createElement( "columns" );
			columns_tag.appendChild( doc.createTextNode( QString::number( it.value()->columns ) ) );
			base_tag.appendChild( columns_tag );
		}

		if ( properties & Size ) {
			QDomElement size_tag = doc.createElement( "size" );
			size_tag.setAttribute( "width", it.value()->size.width() );
			size_tag.setAttribute( "height", it.value()->size.height() );
			base_tag.appendChild( size_tag );
		}
	}

	QFile out_file( filename );
	if ( out_file.open( QIODevice::WriteOnly ) ) {
		has_changes = false;

		Q3TextStream stream( &out_file );
		stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" << doc.toString();
	}
	else
		kDebug() << "Error: Unable to write to file " << filename << endl;
}

void SetupDisplay::nodeClicked(const QString &/*url*/,const QPoint &point)
{
	DOM::Node node = nodeUnderMouse();
	if ( node.isNull() ) {
		return;
	}

	while ( !node.isNull() ) {
		if ( node.nodeType() == DOM::Node::ELEMENT_NODE ) {
			DOM::Element element = (DOM::Element)node;
			if ( element.hasAttribute("class") ) {
				QString id = element.getAttribute("class").string();
				if ( node_item_map->keys().contains(id) )
					break;
			}
		}

		node = node.parentNode();
	}

	if ( node.isNull() || node.nodeType() != DOM::Node::ELEMENT_NODE ) {
		kDebug() << "No relevant node" << endl;
		return;
	}

	DOM::Element element = (DOM::Element)node;

	m_currNodeId = element.getAttribute("class").string();
	if ( m_currNodeId.isEmpty() ) {
		kDebug()<<"Code error: unable to determine class of selected element"<<endl;
		return;
	}

	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	
	delete popup;
	popup = new KMenu( view() );
	popup->addTitle( item->name );

	unsigned int properties = 0;
	for ( PropertiesMap::const_iterator it = box_properties->begin(); it != box_properties->end(); ++it ) {
		if ( it.key()->nodeId == m_currNodeId ) {
			properties = it.value();
			break;
		}
	}

	KIconLoader *il = KIconLoader::global();

	if ( properties & BackgroundColor )
		popup->insertItem( i18n( "Background Color..." ), this, SLOT( setBackgroundColor() ) );

	if ( properties & TextColor )
		popup->insertItem( i18n( "Text Color..." ), this, SLOT( setTextColor() ) );

	if ( properties & Font )
		popup->insertItem( il->loadIconSet( "text", KIconLoader::Small, 16 ), i18n( "Font..." ), this, SLOT( setFont() ) );

	if ( properties & Visibility ) {
		int id = popup->insertItem( i18n( "Show" ), this, SLOT( setShown( int ) ) );
		popup->setItemChecked( id, item->show );
	}

	if ( properties & Alignment ) {
		Q3PopupMenu * sub_popup = new Q3PopupMenu( popup );

		Q3ActionGroup *alignment_actions = new Q3ActionGroup( this );
		alignment_actions->setExclusive( true );

		Q3Action *c_action = new Q3Action( i18n( "Center" ), i18n( "Center" ), 0, alignment_actions, 0, true );
		Q3Action *l_action = new Q3Action( i18n( "Left" ), i18n( "Left" ), 0, alignment_actions, 0, true );
		Q3Action *r_action = new Q3Action( i18n( "Right" ), i18n( "Right" ), 0, alignment_actions, 0, true );

		int align = item->alignment;
		if ( align & Qt::AlignHCenter )
			c_action->setOn(true);
		if ( align & Qt::AlignLeft )
			l_action->setOn(true);
		if ( align & Qt::AlignRight )
			r_action->setOn(true);

		connect( alignment_actions, SIGNAL( selected( QAction* ) ), SLOT( setAlignment( QAction* ) ) );

		popup->insertItem( i18n( "Alignment" ), sub_popup );

		alignment_actions->addTo( sub_popup );
	}

	if ( properties & Border )
		popup->insertItem( i18n( "Border..." ), this, SLOT( setBorder() ) );

	if ( properties & Columns )
		popup->insertItem( i18n( "Columns..." ), this, SLOT( setColumns() ) );

	popup->popup( point );
}

void SetupDisplay::applyStylesheet()
{
	loadTemplate( m_activeTemplate );
}

void SetupDisplay::setBackgroundColor()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KColorDialog::getColor( item->backgroundColor, view() ) == QDialog::Accepted ) {
		m_currentItem = item;
		loadBackgroundColor(m_currNodeId,item->backgroundColor);
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setBorder()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	BorderDialog borderDialog( item->border, view() );
	if ( borderDialog.exec() == QDialog::Accepted ) {
		m_currentItem = item;
		loadBorder( m_currNodeId, borderDialog.border() );
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setColumns()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	int cols = QInputDialog::getInteger( QString::null, i18n("Select the number of columns to use:"), item->columns, 1, 100, 1, 0, view() );
	if ( cols > 0 ) {
		m_currentItem = item;
		loadColumns( m_currNodeId, cols );
		m_currentItem = 0;

		loadTemplate( m_activeTemplate );
		has_changes = true;
	}
}

void SetupDisplay::setTextColor()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KColorDialog::getColor( item->textColor, view() ) == QDialog::Accepted ) {
		m_currentItem = item;
		loadTextColor(m_currNodeId,item->textColor);
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setShown( int id )
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	emit itemVisibilityChanged( item, !popup->isItemChecked( id ) );

	m_currentItem = item;
	loadVisibility(m_currNodeId,!popup->isItemChecked( id ));
	m_currentItem = 0;

	applyStylesheet();
	has_changes = true;
}

void SetupDisplay::setFont()
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );
	if ( KFontDialog::getFont( item->font, false, view() ) == QDialog::Accepted ) {
		m_currentItem = item;
		loadFont(m_currNodeId,item->font);
		m_currentItem = 0;

		applyStylesheet();
		has_changes = true;
	}
}

void SetupDisplay::setAlignment( QAction *action )
{
	KreDisplayItem *item = *node_item_map->find( m_currNodeId );

	//TODO: isn't there a simpler way to do this...
	//preserve non-horizontal alignment flags
	if ( item->alignment & Qt::AlignRight )
		item->alignment ^= Qt::AlignRight;
	if ( item->alignment & Qt::AlignHCenter )
		item->alignment ^= Qt::AlignHCenter;
	if ( item->alignment & Qt::AlignLeft )
		item->alignment ^= Qt::AlignLeft;

	if ( action->text() == i18n( "Center" ) )
		item->alignment |= Qt::AlignHCenter;
	else if ( action->text() == i18n( "Left" ) )
		item->alignment |= Qt::AlignLeft;
	else if ( action->text() == i18n( "Right" ) )
		item->alignment |= Qt::AlignRight;

	m_currentItem = item;
	loadAlignment(m_currNodeId,item->alignment);
	m_currentItem = 0;

	applyStylesheet();
	has_changes = true;
}

void SetupDisplay::setItemShown( KreDisplayItem *item, bool visible )
{
	item->show = visible;

	applyStylesheet();

	has_changes = true;
}

void SetupDisplay::changeMade( void )
{
	has_changes = true;
}

#include "setupdisplay.moc"
