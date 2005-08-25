/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mysqlrecipedb.h"

#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

MySQLRecipeDB::MySQLRecipeDB( const QString &host, const QString &user, const QString &pass, const QString &DBname ) : QSqlRecipeDB( host, user, pass, DBname )
{}

MySQLRecipeDB::~MySQLRecipeDB()
{}

void MySQLRecipeDB::createDB()
{
	QString real_db_name = database->databaseName();

	//we have to be connected to some database in order to create the Krecipes database
	//so long as the permissions given are allowed access to "mysql', this works
	database->setDatabaseName( "mysql" );
	if ( database->open() ) {
		// Create the Database (Note: needs permissions)
		//FIXME: I've noticed certain characters cause this to fail (such as '-').  Somehow let the user know.
		QSqlQuery query( QString( "CREATE DATABASE %1" ).arg( real_db_name ), database );
		if ( !query.isActive() )
			kdDebug() << "create query failed: " << database->lastError().databaseText() << endl;

		database->close();
	}
	else
		kdDebug() << "create open failed: " << database->lastError().databaseText() << endl;

	database->setDatabaseName( real_db_name );
}

QStringList MySQLRecipeDB::backupCommand() const
{
	KConfig *config = KGlobal::config();
	config->setGroup("Server");

	QStringList command;
	command<<config->readEntry( "MySQLDumpPath", "mysqldump" )<<"-q";

	QString pass = config->readEntry("Password", QString::null);
	if ( !pass.isEmpty() )
		command<<"-p"+pass;

	command<<database->databaseName();
	return command;
}

void MySQLRecipeDB::createTable( const QString &tableName )
{

	QStringList commands;

	if ( tableName == "recipes" )
		commands << QString( "CREATE TABLE recipes (id INTEGER NOT NULL AUTO_INCREMENT,title VARCHAR(%1),persons int(11),instructions TEXT, photo BLOB, prep_time TIME,   PRIMARY KEY (id));" ).arg( maxRecipeTitleLength() );

	else if ( tableName == "ingredients" )
		commands << QString( "CREATE TABLE ingredients (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxIngredientNameLength() );

	else if ( tableName == "ingredient_list" )
		commands << "CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, amount_offset FLOAT, unit_id INTEGER, prep_method_id INTEGER, order_index INTEGER, group_id INTEGER, INDEX ridil_index(recipe_id), INDEX iidil_index(ingredient_id));";

	else if ( tableName == "unit_list" )
		commands << "CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);";

	else if ( tableName == "units" )
		commands << QString( "CREATE TABLE units (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), plural VARCHAR(%2), PRIMARY KEY (id));" ).arg( maxUnitNameLength() ).arg( maxUnitNameLength() );

	else if ( tableName == "prep_methods" )
		commands << QString( "CREATE TABLE prep_methods (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxPrepMethodNameLength() );

	else if ( tableName == "ingredient_info" )
		commands << "CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);";

	else if ( tableName == "ingredient_properties" )
		commands << "CREATE TABLE ingredient_properties (id INTEGER NOT NULL AUTO_INCREMENT,name VARCHAR(20), units VARCHAR(20), PRIMARY KEY (id));";

	else if ( tableName == "units_conversion" )
		commands << "CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);";

	else if ( tableName == "categories" )
		commands << QString( "CREATE TABLE categories (id int(11) NOT NULL auto_increment, name varchar(%1) default NULL, parent_id int(11) NOT NULL default -1, PRIMARY KEY (id));" ).arg( maxCategoryNameLength() );

	else if ( tableName == "category_list" )
		commands << "CREATE TABLE category_list (recipe_id int(11) NOT NULL,category_id int(11) NOT NULL, INDEX  rid_index (recipe_id), INDEX cid_index (category_id));";

	else if ( tableName == "authors" )
		commands << QString( "CREATE TABLE authors (id int(11) NOT NULL auto_increment, name varchar(%1) default NULL,PRIMARY KEY (id));" ).arg( maxAuthorNameLength() );

	else if ( tableName == "author_list" )
		commands << "CREATE TABLE author_list (recipe_id int(11) NOT NULL,author_id int(11) NOT NULL);";

	else if ( tableName == "db_info" ) {
		commands << "CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);";
		commands << QString( "INSERT INTO db_info VALUES(%1,'Krecipes %2');" ).arg( latestDBVersion() ).arg( krecipes_version() );
	}
	else if ( tableName == "ingredient_groups" ) {
		commands << QString( "CREATE TABLE `ingredient_groups` (`id` int(11) NOT NULL auto_increment, `name` varchar(%1), PRIMARY KEY (`id`));" ).arg( maxIngGroupNameLength() );
	}

	else
		return ;

	QSqlQuery databaseToCreate( QString::null, database );

	// execute the queries
	for ( QStringList::const_iterator it = commands.begin(); it != commands.end(); ++it )
		databaseToCreate.exec( ( *it ) );
}

void MySQLRecipeDB::portOldDatabases( float version )
{
	kdDebug() << "Current database version is..." << version << "\n";
	QString command;

	if ( version < 0.3 ) 	// The database was generated with a version older than v 0.3. First update to 0.3 version
		// ( note that version no. means the version in which this DB structure
		// was introduced)
	{

		// Add new columns to existing tables (creating new tables is not necessary. Integrity check does that before)
		command = "ALTER TABLE recipes ADD COLUMN persons int(11) AFTER title;";
		QSqlQuery tableToAlter( command, database );

		// Set the version to the new one (0.3)

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.3,'Krecipes 0.4');"; // Set the new version
		tableToAlter.exec( command );
	}

	if ( version < 0.4 )   // Upgrade to the current DB version 0.4
	{

		// Add new columns to existing tables (creating any new tables is not necessary. Integrity check does that before)
		command = "ALTER TABLE ingredient_list ADD COLUMN order_index int(11) AFTER unit_id;";
		QSqlQuery tableToAlter( command, database );

		// Missing indexes in the previous versions
		command = "CREATE index rid_index ON category_list(recipe_id)";
		tableToAlter.exec( command );

		command = "CREATE index cid_index ON category_list(category_id)";
		tableToAlter.exec( command );

		command = "CREATE index ridil_index ON ingredient_list(recipe_id)";
		tableToAlter.exec( command );

		command = "CREATE index iidil_index ON ingredient_list(ingredient_id)";
		tableToAlter.exec( command );

		// Port data

		//*1:: Recipes have always category -1 to speed up searches (no JOINs needed)
		command = "SELECT r.id FROM recipes r;"; // Find all recipes
		QSqlQuery categoryToAdd( QString::null, database );
		tableToAlter.exec( command );
		if ( tableToAlter.isActive() )
		{
			while ( tableToAlter.next() ) {
				int recipeId = tableToAlter.value( 0 ).toInt();
				QString cCommand = QString( "INSERT INTO category_list VALUES (%1,-1);" ).arg( recipeId );
				categoryToAdd.exec( cCommand );
			}
		}

		// Set the version to the new one (0.4)

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.4,'Krecipes 0.4');"; // Set the new version
		tableToAlter.exec( command );
	}

	if ( version < 0.5 ) {
		command = QString( "CREATE TABLE prep_methods (id INTEGER NOT NULL AUTO_INCREMENT, name VARCHAR(%1), PRIMARY KEY (id));" ).arg( maxPrepMethodNameLength() );
		QSqlQuery tableToAlter( command, database );

		command = "ALTER TABLE ingredient_list ADD COLUMN prep_method_id int(11) AFTER unit_id;";
		tableToAlter.exec( command );
		command = "UPDATE ingredient_list SET prep_method_id=-1 WHERE prep_method_id IS NULL;";
		tableToAlter.exec( command );

		command = "ALTER TABLE authors MODIFY name VARCHAR(50);";
		tableToAlter.exec( command );
		command = "ALTER TABLE categories MODIFY name VARCHAR(40);";
		tableToAlter.exec( command );

		// Set the version to the new one (0.5)
		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.5,'Krecipes 0.5');";
		tableToAlter.exec( command );
	}

	if ( version < 0.6 ) {
		command = "ALTER TABLE categories ADD COLUMN parent_id int(11) NOT NULL default '-1' AFTER name;";
		QSqlQuery tableToAlter( command, database );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.6,'Krecipes 0.6');";
		tableToAlter.exec( command );
	}

	if ( version < 0.61 ) {
		QString command = "ALTER TABLE `recipes` ADD COLUMN `prep_time` TIME DEFAULT NULL";
		QSqlQuery tableToAlter( command, database );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.61,'Krecipes 0.6');";
		tableToAlter.exec( command );
	}

	if ( version < 0.62 ) {
		QString command = "ALTER TABLE `ingredient_list` ADD COLUMN `group_id` int(11) default '-1' AFTER order_index;";
		QSqlQuery tableToAlter( command, database );

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.62,'Krecipes 0.7');";
		tableToAlter.exec( command );
	}

	if ( version < 0.63 ) {
		QString command = "ALTER TABLE `units` ADD COLUMN `plural` varchar(20) DEFAULT NULL AFTER name;";
		QSqlQuery tableToAlter( command, database );

		QSqlQuery result( "SELECT id,name FROM units WHERE plural IS NULL", database );
		if ( result.isActive() ) {
			while ( result.next() ) {
				command = "UPDATE units SET plural='" + result.value( 1 ).toString() + "' WHERE id=" + QString::number( result.value( 0 ).toInt() );
				QSqlQuery query( command, database );
			}
		}

		command = "DELETE FROM db_info;"; // Remove previous version records if they exist
		tableToAlter.exec( command );
		command = "INSERT INTO db_info VALUES(0.63,'Krecipes 0.7');";
		tableToAlter.exec( command );
	}

	if ( version < 0.7 ) { //simply call 0.63 -> 0.7
		QString command = "UPDATE db_info SET ver='0.7';";
		QSqlQuery query( command, database );
	}

	if ( version < 0.81 ) {
		QString command = "ALTER TABLE `ingredient_list` ADD COLUMN `amount_offset` FLOAT DEFAULT '0' AFTER amount;";
		QSqlQuery tableToAlter( command, database );

		command = "UPDATE db_info SET ver='0.81',generated_by='Krecipes SVN (20050816)';";
		tableToAlter.exec( command );
	}
}

int MySQLRecipeDB::lastInsertID()
{
	QSqlQuery lastInsertID( "SELECT LAST_INSERT_ID();", database );

	int id = -1;
	if ( lastInsertID.isActive() && lastInsertID.next() )
		id = lastInsertID.value( 0 ).toInt();

	return id;
}

void MySQLRecipeDB::givePermissions( const QString &dbName, const QString &username, const QString &password, const QString &clientHost )
{
	QString command;

	if ( !password.isEmpty() )
		command = QString( "GRANT ALL ON %1.* TO '%2'@'%3' IDENTIFIED BY '%4';" ).arg( dbName ).arg( username ).arg( clientHost ).arg( password );
	else
		command = QString( "GRANT ALL ON %1.* TO '%2'@'%3';" ).arg( dbName ).arg( username ).arg( clientHost );

	kdDebug() << "I'm doing the query to setup permissions\n";

	QSqlQuery permissionsToSet( command, database );
}

#include "mysqlrecipedb.moc"
