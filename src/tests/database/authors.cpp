/****************************************************************************
 *   Copyright © 2012-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 ****************************************************************************/


#include "authors.h"

#include "backends/recipedb.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QtTest/QTest>
#include <QDebug>
#include <QStandardPaths>


RecipeDB * TestDatabaseAuthors::createDatabase(const QString & configFilename )
{
	RecipeDB * database;

	QString configFilePath = ":/" + configFilename;
	qDebug() << "opening database configured in" << configFilePath;

	QStringList strList;
	strList << configFilePath;
	KSharedConfig::openConfig()->addConfigSources( strList );

	KConfigGroup dbtypeGroup = KSharedConfig::openConfig()->group( "DBType" );
	QString dbType = dbtypeGroup.readEntry( "Type", "" );

	KConfigGroup serverGroup = KSharedConfig::openConfig()->group( "Server" );
	QString host = serverGroup.readEntry( "Host", "localhost" );
	QString user = serverGroup.readEntry( "Username", QString() );
	QString pass = serverGroup.readEntry( "Password", QString() );
	QString dbname = serverGroup.readEntry( "DBName", "Krecipes" );
	int port = serverGroup.readEntry( "Port", 0 );
	QString dbfile = serverGroup.readEntry( "DBFile",
		QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') + "krecipes.krecdb" ) ;

	database = RecipeDB::createDatabase( dbType, host, user, pass, dbname, port, dbfile );

	database->connect();
	
	return database;
}


void TestDatabaseAuthors::initTestCase()
{
	m_sqliteDatabase = createDatabase( "sqliterc" );
	qDebug() << m_sqliteDatabase->dbErr;
        QVERIFY( m_sqliteDatabase->ok() );
	m_sqliteDatabase->transaction();
	m_sqliteDatabase->wipeDatabase();
	m_sqliteDatabase->commit();
	//m_sqliteDatabase->checkIntegrity();

#ifdef KRE_TESTS_MYSQL
	m_mysqlDatabase = createDatabase( "mysqlrc" );
	qDebug() << m_mysqlDatabase->dbErr;
        QVERIFY( m_mysqlDatabase->ok() );
	m_mysqlDatabase->transaction();
	m_mysqlDatabase->wipeDatabase();
	m_mysqlDatabase->commit();
	//m_mysqlDatabase->checkIntegrity();
#endif

#ifdef KRE_TESTS_POSTGRESQL
	m_postgresqlDatabase = createDatabase( "postgresqlrc" );
	qDebug() << m_postgresqlDatabase->dbErr;
        QVERIFY( m_postgresqlDatabase->ok() );
	m_postgresqlDatabase->transaction();
	m_postgresqlDatabase->wipeDatabase();
	m_postgresqlDatabase->commit();
	//m_postgresqlDatabase->checkIntegrity();
#endif
}


void TestDatabaseAuthors::cleanupTestCase()
{
	if ( m_sqliteDatabase ) {
		delete m_sqliteDatabase;
	}
#ifdef KRE_TESTS_MYSQL
	if ( m_mysqlDatabase ) {
		delete m_mysqlDatabase;
	}
#endif
#ifdef KRE_TESTS_POSTGRESQL
	if ( m_postgresqlDatabase ) {
		delete m_postgresqlDatabase;
	}
#endif
}

void TestDatabaseAuthors::createAuthors_data()
{
	QTest::addColumn<QString>("authorName");

	QTest::newRow("normal") << "Ethan";
	QTest::newRow("SQL injection") << "Eth'an";
}

void TestDatabaseAuthors::createAuthors( RecipeDB * database )
{
	QFETCH( QString, authorName );

	RecipeDB::IdType last_insert_id = database->createNewAuthor( authorName );
	QVERIFY( last_insert_id != RecipeDB::InvalidId );

	RecipeDB::IdType author_id = database->findExistingAuthorByName( authorName );
	QVERIFY( author_id != RecipeDB::InvalidId );
	QCOMPARE( last_insert_id, author_id);
}

void TestDatabaseAuthors::testCreateSQLite_data()
{
	createAuthors_data();
}

void TestDatabaseAuthors::testCreateSQLite()
{
	createAuthors( m_sqliteDatabase );
}


#ifdef KRE_TESTS_MYSQL
void TestDatabaseAuthors::testCreateMySQL_data()
{
	createAuthors_data();
}

void TestDatabaseAuthors::testCreateMySQL()
{
	createAuthors( m_mysqlDatabase );
}
#endif


#ifdef KRE_TESTS_POSTGRESQL
void TestDatabaseAuthors::testCreatePostgreSQL_data()
{
	createAuthors_data();
}

void TestDatabaseAuthors::testCreatePostgreSQL()
{
	createAuthors( m_postgresqlDatabase );
}
#endif

QTEST_MAIN(TestDatabaseAuthors)

