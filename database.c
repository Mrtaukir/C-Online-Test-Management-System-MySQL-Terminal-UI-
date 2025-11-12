#include <stdio.h>
#include <stdlib.h>
#include "database.h"

void ConnectDB(Db *self)
{
    const char *server = "localhost";
    const char *user = "root";
    const char *password = "12345";
    const char *databaseName = "mockTests";

    self->conn = mysql_init(NULL);
    if (!self->conn)
    {
        fprintf(stderr, "mysql_init failed\n");
        exit(1);
    }

    if (!mysql_real_connect(self->conn, server, user, password, databaseName, 0, NULL, 0))
    {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(self->conn));
        mysql_close(self->conn);
        exit(1);
    }

    printf("Connected successfully to database: %s\n", databaseName);
}

void closeDB(Db *self)
{
    if (self->conn)
    {
        mysql_close(self->conn);
        self->conn = NULL;
        printf("Connection closed successfully\n");
    }
}

void initDb(Db *db)
{
    db->connect = ConnectDB;
    db->close = closeDB;
    db->conn = NULL;
    db->res = NULL;
    db->row = NULL;
}