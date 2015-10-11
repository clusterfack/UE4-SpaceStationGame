// Fill out your copyright notice in the Description page of Project Settings.

// This is a server only class

#include "SpaceStationGame.h"

#include "mysql_connection.h"
#include "mysql_driver.h"
#include "mysql_error.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "StringConv.h"
#include "StringHelpers.h"

#include "Online.h"
#include "OnlineSubsystem.h"

#include "MySQLObject.h"

UMySQLObject::UMySQLObject(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UMySQLObject::BeginPlay()
{
#if UE_BUILD_DEBUG
	try
	{
#endif
		driver = sql::mysql::get_mysql_driver_instance();

		con = driver->connect(StringHelpers::ConvertToString(ServerUrl), StringHelpers::ConvertToString(ServerUsername), StringHelpers::ConvertToString(ServerPassword));

		sql::Statement* stmt(con->createStatement());

		stmt->execute("CREATE DATABASE IF NOT EXISTS " + StringHelpers::ConvertToString(ServerDatabase));

		stmt->execute("USE " + StringHelpers::ConvertToString(ServerDatabase));

		stmt->execute("CREATE TABLE IF NOT EXISTS `players` ("
			"`steamid` BIGINT(20) UNSIGNED NOT NULL,"
			"`preferredjob` TINYINT(3) UNSIGNED NOT NULL,"
			"PRIMARY KEY (`steamid`));");

		delete stmt;
#if UE_BUILD_DEBUG
	}
	catch (sql::SQLException &e)
	{
		UE_LOG(LogTemp, Warning, TEXT("MySQL error code: %d"), e.getErrorCode());

		/*std::string State = e.getSQLState();

		UE_LOG(LogTemp, Warning, TEXT("MySQL state: %s"), StringHelpers::ConvertToFString(State));*/
	}
#endif
}

void UMySQLObject::SetUpMySQLPlayerData(FString SteamID)
{
#if UE_BUILD_DEBUG
	try
	{
#endif
		sql::Statement* stmt(con->createStatement());

		stmt->execute("USE " + StringHelpers::ConvertToString(ServerDatabase));

		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get("Steam");

		if (OnlineSub)
		{
			UE_LOG(LogTemp, Warning, TEXT("Steam works"));

			// 19 is the assistant id
			stmt->execute("INSERT IGNORE INTO players (steamid, preferredjob) VALUES (" + StringHelpers::ConvertToString(*SteamID) + ", 19);");
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Steam subsystem failed, using whatever"));

			//My steam id
			stmt->execute("INSERT IGNORE INTO players (steamid, preferredjob) VALUES (76561198004815982, 19);");
		}

		delete stmt;
#if UE_BUILD_DEBUG
	}
	catch (sql::SQLException &e)
	{
		UE_LOG(LogTemp, Warning, TEXT("MySQL error code: %d"), e.getErrorCode());

		//std::string State = e.getSQLState();

		//UE_LOG(LogTemp, Warning, TEXT("MySQL state: %s"), StringHelpers::ConvertToFString(State));
	}
#endif
}

uint8 UMySQLObject::GetMySQLPreferredJob(FString SteamID)
{
#if UE_BUILD_DEBUG
	try
	{
#endif
		sql::Statement* stmt(con->createStatement());

		sql::ResultSet* res;

		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get("Steam");

		stmt->execute("USE " + StringHelpers::ConvertToString(ServerDatabase));

		if (OnlineSub)
		{
			res = stmt->executeQuery("SELECT * FROM players WHERE steamid LIKE " + StringHelpers::ConvertToString(*SteamID));
		}
		else
		{
			res = stmt->executeQuery("SELECT * FROM players WHERE steamid LIKE 76561198004815982");
		}

		//delete stmt;
		if (res->next())
		{
			return res->getUInt("preferredjob");
		}
		else return 19;
#if UE_BUILD_DEBUG
	}
	catch (sql::SQLException &e)
	{
		UE_LOG(LogTemp, Warning, TEXT("MySQL error code: %d"), e.getErrorCode());

		//std::string State = e.getSQLState();

		//UE_LOG(LogTemp, Warning, TEXT("MySQL state: %s"), StringHelpers::ConvertToFString(State));

		return 19;
	}
	return 19;
#endif
}

void UMySQLObject::BeginDestroy()
{
	delete con;

	Super::BeginDestroy();
}