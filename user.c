#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "database.h"
#include "testSeries.h"
#include "globals.h"

typedef struct UserDB
{
    char *fullName;
    char *city;
    char *mobileNo;
    char *email;
    char *userId;
    char *password;

    void (*signUp)(struct UserDB *self);
    void (*login)(struct UserDB *self);
    void (*forget)(struct UserDB *self);
    bool (*already)(char *mobileNo, char *email, struct UserDB *self);
    void (*home)(struct UserDB *self);

    Db db;
    TestSeries test;
} User;

void privatePassword(char *password)
{
    int i = 0;
    char ch;
    system("stty -echo");
    system("stty -icanon");
    while (1)
    {
        ch = getchar();
        if (ch == '\n' || ch == '\r')
            break;
        if (ch == 127 || ch == '\b')
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else
        {
            password[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }
    password[i] = '\0';
    system("stty echo");
    system("stty sane");
    printf("\n");
}

bool alreadyExists(char *mobileNo, char *email, User *self)
{
    trim_newline(mobileNo);
    trim_newline(email);
    char query[300];
    sprintf(query, "SELECT * FROM user WHERE mobileNo='%s' OR email='%s';", mobileNo, email);
    if (mysql_query(self->db.conn, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(self->db.conn));
        return false;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res)
        return false;
    int rows = mysql_num_rows(self->db.res);
    mysql_free_result(self->db.res);
    return (rows > 0);
}

void signUpUser(User *self)
{
    system("clear");
    char confirm[200];
    printf("Enter full name: ");
    fgets(self->fullName, 200, stdin);
    trim_newline(self->fullName);
    printf("Enter city: ");
    fgets(self->city, 200, stdin);
    trim_newline(self->city);
    printf("Enter mobile number: ");
    fgets(self->mobileNo, 100, stdin);
    trim_newline(self->mobileNo);
    printf("Enter email: ");
    fgets(self->email, 100, stdin);
    trim_newline(self->email);
    printf("Enter password: ");
    privatePassword(self->password);
    printf("Confirm password: ");
    privatePassword(confirm);
    if (strcmp(self->password, confirm) != 0)
    {
        printf("\nPassword mismatch!\n");
    }
    else if (self->already(self->mobileNo, self->email, self))
    {
        printf("\nUser already exists!\n");
    }
    else
    {
        char query[600];
        sprintf(query,
                "INSERT INTO user(fullName, city, mobileNo, email, password) VALUES('%s','%s','%s','%s','%s');",
                self->fullName, self->city, self->mobileNo, self->email, self->password);
        if (mysql_query(self->db.conn, query))
            fprintf(stderr, "Insert failed: %s\n", mysql_error(self->db.conn));
        else
            printf("\nUser signed up successfully!\n");
    }
    printf("\nPress Enter to continue...");
    getchar();
}

void loginUser(User *self)
{
    system("clear");
    printf("Enter email or mobile: ");
    fgets(self->userId, 100, stdin);
    trim_newline(self->userId);
    printf("Enter password: ");
    privatePassword(self->password);
    char query[400];
    sprintf(query,
            "SELECT * FROM user WHERE (mobileNo='%s' OR email='%s') AND password='%s';",
            self->userId, self->userId, self->password);
    if (mysql_query(self->db.conn, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    self->db.row = mysql_fetch_row(self->db.res);
    if (self->db.row)
    {
        char *role = self->db.row[6];
        initTestSeries(&self->test);
        self->test.db = self->db;
        glob_userId = atoi(self->db.row[0]);
        strcpy(glob_user, self->db.row[1]);

        if (strcmp(role, "student") == 0)
        {
            self->test.userScreen(&self->test);
            freeTestSeries(&self->test);
        }
        else if (strcmp(role, "admin") == 0)
        {
            self->test.adminScreen(&self->test);
            freeTestSeries(&self->test);
        }
    }
    else
    {
        printf("\nIncorrect user or password!\n");
    }
    mysql_free_result(self->db.res);
    printf("\nPress Enter to continue...");
    getchar();
}

void forgetPassword(User *self)
{
    system("clear");
    printf("Enter email or mobile: ");
    fgets(self->userId, 100, stdin);
    trim_newline(self->userId);
    char query[300];
    sprintf(query, "SELECT id FROM user WHERE email='%s' OR mobileNo='%s';", self->userId, self->userId);
    if (mysql_query(self->db.conn, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    self->db.row = mysql_fetch_row(self->db.res);
    if (!self->db.row)
    {
        printf("\nUser not found!\n");
        mysql_free_result(self->db.res);
        return;
    }
    int id = atoi(self->db.row[0]);
    mysql_free_result(self->db.res);
    char newPass[200], confirm[200];
    printf("Enter new password: ");
    privatePassword(newPass);
    printf("Confirm password: ");
    privatePassword(confirm);
    if (strcmp(newPass, confirm) != 0)
    {
        printf("\nPassword mismatch!\n");
        return;
    }
    char updateQuery[300];
    sprintf(updateQuery, "UPDATE user SET password='%s' WHERE id=%d;", newPass, id);
    if (mysql_query(self->db.conn, updateQuery))
        fprintf(stderr, "Password update failed: %s\n", mysql_error(self->db.conn));
    else
        printf("\nPassword updated successfully!\n");
    printf("\nPress Enter to continue...");
    getchar();
}

void homeScreen(User *self)
{
    int choice;
    while (1)
    {
        system("clear");
        printf("===== USER PORTAL =====\n");
        printf("1. Signup\n");
        printf("2. Login\n");
        printf("3. Forget Password\n");
        printf("4. Exit\n");
        printf("Choose option: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input! Please enter a number between 1 and 4.\n");
            printf("Press Enter to continue....\n");
            while (getchar() != '\n')
                ;
            getchar();
            continue;
        }
        getchar();
        switch (choice)
        {
        case 1:
            self->signUp(self);
            break;
        case 2:
            self->login(self);
            break;
        case 3:
            self->forget(self);
            break;
        case 4:
            self->db.close(&self->db);
            exit(0);
        default:
            printf("Invalid choice!\n");
            getchar();
        }
    }
}

int main()
{
    struct UserDB userDb;
    User *user = &userDb;
    user->fullName = malloc(200);
    user->city = malloc(200);
    user->mobileNo = malloc(100);
    user->email = malloc(100);
    user->userId = malloc(100);
    user->password = malloc(200);
    initDb(&user->db);
    user->db.connect(&user->db);
    user->signUp = signUpUser;
    user->login = loginUser;
    user->forget = forgetPassword;
    user->already = alreadyExists;
    user->home = homeScreen;
    user->home(user);
    free(user->fullName);
    free(user->city);
    free(user->mobileNo);
    free(user->email);
    free(user->userId);
    free(user->password);
    user->db.close(&user->db);
    return 0;
}
