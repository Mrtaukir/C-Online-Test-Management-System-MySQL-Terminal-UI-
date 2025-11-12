#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>

typedef struct Database
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    void (*connect)(struct Database *self);
    void (*close)(struct Database *self);
} Db;

void ConnectDB(Db *self);
void closeDB(Db *self);
void initDb(Db *db);

#endif