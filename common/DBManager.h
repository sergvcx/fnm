#pragma once
#include "main.h"
#include "shared.h"

void sql_create_seccode_deal(QSqlDatabase& db_trading, QString seccode);
void sql_create_seccode_quote(QSqlDatabase& db_trading, QString seccode);
bool sql_open_database(QString db_name, QSqlDatabase& db_trading);
void sql_close_database(QSqlDatabase& db_trading);
uint sql_get_last_id_from_tbl(QSqlDatabase& db, QString tbl);
uint sql_get_last_datetime_from_seccode_deal(QSqlDatabase& db, QString seccode);
bool sql_drop_table (QSqlDatabase& db, QString Table );
bool sql_drop_tables(QSqlDatabase& db, QList<QString>& listTable, QString suffix );
uint sql_read_ticks (QSqlDatabase& db, QString StockCode, QDateTime& fromDT, QDateTime& toDT, S_RingEasyTicks& ticks);
uint sql_read_quotes(QSqlDatabase& db, QString StockCode, QDateTime& fromDT, QDateTime& toDT, S_RingEasyQuotes& quotes);
uint sql_show_tables(QSqlDatabase& db, QString suffix, QList<QString>& listTables);
bool sql_check_database(QSqlDatabase& db);

void Ticks2Mysql ( QSqlQuery& query, QString seccode, S_RingEasyTicks& ticks,    unsigned afterDateTime);
void Quotes2Mysql( QSqlQuery& query, QString seccode, S_RingEasyQuotes& quotes,  bool echo);