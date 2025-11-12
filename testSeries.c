#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include<sys/select.h>
#include<time.h>
#include "testSeries.h"
#include "globals.h"

void mcqDetails(TestSeries *self, char *subject);
bool isAvailable(TestSeries *self, int mockId, char *subject);
void addQuestions(TestSeries *self, int mockId);
void showQuestions(TestSeries *self, int mcqId);
void showAllMcqForStudent(TestSeries *self, char *subject);
void subjectSelectionForStudent(TestSeries *self);
void showUserInfo(TestSeries *self);
void showAllMcqForStudent(TestSeries *self, char *subject);
void startTest(TestSeries *self, int id);

void waitForEnter()
{
    printf("Press Enter to continue...");
    while (getchar() != '\n')
        ;
}

void subjectSelection(TestSeries *self)
{
    int choice;
    while (1)
    {
        system("clear");
        printf("===== Select Subject =====\n");
        printf("1. C programming\n");
        printf("2. Java programming\n");
        printf("3. Database\n");
        printf("4. Back\n");
        printf("Choose subject: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input! Please enter a number between 1 and 4.\n");
            while (getchar() != '\n')
                ;
            waitForEnter();
            continue;
        }
        getchar();
        switch (choice)
        {
        case 1:
            mcqDetails(self, "C programming");
            break;
        case 2:
            mcqDetails(self, "Java programming");
            break;
        case 3:
            mcqDetails(self, "Database");
            break;
        case 4:
            return;
        default:
            printf("Invalid choice!\n");
            waitForEnter();
        }
    }
}

void forAdminScreen(TestSeries *self)
{
    int option;
    while (1)
    {
        system("clear");
        printf("===========================\n");
        printf(">>> Loading... <<<\n");
        printf("===========================\n");
        sleep(1);
        system("clear");
        printf("===================================\n");
        printf("      Welcome, %s", glob_user);
        printf("===================================\n");
        printf("===== Admin Panel =====\n");
        printf("1. Create MCQ and Add Questions\n");
        printf("2. Display All MCQ and Questions\n");
        printf("3. Update MCQ\n");
        printf("4. Delete MCQ with Question\n");
        printf("5. Show User Info\n");
        printf("6. Logout\n");
        printf("Choose option: ");
        if (scanf("%d", &option) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("Invalid input! Press Enter to continue...");
            waitForEnter();
            continue;
        }
        getchar();
        switch (option)
        {
        case 1:
            self->subjectSelect(self);
            break;
        case 2:
            if (self->display)
                self->display(self);
            int selectedMCQ = -1;
            printf("Enter MCQ ID to show questions: ");
            if (scanf("%d", &selectedMCQ) == 1 && selectedMCQ > 0)
            {
                showQuestions(self, selectedMCQ);
            }
            else
            {
                printf("Invalid Input\n");
            }
            waitForEnter();
            break;
        case 3:
            if (self->update)
                self->update(self);
            break;
        case 4:
            if (self->delete)
                self->delete(self);
            break;
        case 5:
            showUserInfo(self);
            break;
        case 6:
            return;
        default:
            printf("Invalid choice!\n");
            waitForEnter();
        }
    }
}

void forUserScreen(TestSeries *self)
{
    int option;
    while (1)
    {
        system("clear");
        printf("===========================\n");
        printf(">>> Loading... <<<\n");
        printf("===========================\n");
        sleep(1);
        system("clear");
        printf("===================================\n");
        printf("      Welcome, %s\n", glob_user);
        printf("===================================\n");
        printf("===== User Test Menu =====\n");
        printf("1. Show All MCQ\n");
        printf("2. Exit\n");
        printf("Choose option: ");
        if (scanf("%d", &option) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("Invalid input! Press Enter to continue...");
            waitForEnter();
            continue;
        }
        getchar();
        switch (option)
        {
        case 1:
            subjectSelectionForStudent(self);
            break;
        case 2:
            return;
        default:
            printf("Invalid choice!\n");
            waitForEnter();
        }
    }
}

void initTestSeries(TestSeries *mcq)
{
    mcq->title = malloc(400);
    mcq->subject = malloc(400);
    mcq->level = malloc(100);
    mcq->date = malloc(100);
    mcq->question = malloc(2000);
    for (int i = 0; i < 4; i++)
        mcq->options[i] = malloc(200);
    mcq->userResponse = malloc(200);

    mcq->adminScreen = forAdminScreen;
    mcq->userScreen = forUserScreen;
    mcq->subjectSelect = subjectSelection;

    mcq->create = createMCQ;
    mcq->display = displayMCQ;
    mcq->update = updateMCQ;
    mcq->delete = deleteMCQ;
}

void freeTestSeries(TestSeries *mcq)
{
    free(mcq->title);
    free(mcq->subject);
    free(mcq->level);
    free(mcq->date);
    free(mcq->question);
    free(mcq->userResponse);
    for (int i = 0; i < 4; i++)
        free(mcq->options[i]);
}

void displayMCQ(TestSeries *self)
{
    system("clear");
    printf("===== All MCQs =====\n");

    char query[500];
    sprintf(query, "SELECT mockId, title, subject, level FROM mockDetails;");

    if (mysql_query(self->db.conn, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(self->db.conn));
        return;
    }

    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res)
        return;

    printf("%-8s %-20s %-20s %-10s\n", "MCQ ID", "TITLE", "SUBJECT", "LEVEL");
    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        char *id = self->db.row[0] ? self->db.row[0] : "0";
        char *title = self->db.row[1] ? self->db.row[1] : "NULL";
        char *subject = self->db.row[2] ? self->db.row[2] : "NULL";
        char *level = self->db.row[3] ? self->db.row[3] : "NULL";

        trim_newline(title);
        trim_newline(subject);
        trim_newline(level);

        printf("%-8s %-20s %-20s %-10s\n", id, title, subject, level);
    }

    mysql_free_result(self->db.res);
    // waitForEnter();
}

void updateMCQ(TestSeries *self)
{
    system("clear");
    // getchar();
    if (mysql_query(self->db.conn, "SELECT mockId, title, subject, duration, level FROM mockDetails;"))
    {
        fprintf(stderr, "Failed to fetch mock data: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (self->db.res == NULL)
    {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(self->db.conn));
        return;
    }
    printf("=====================================================================================\n");
    printf("                                      MCQ DETAILS                                    \n");
    printf("=====================================================================================\n");
    printf("%-10s %-20s %-20s %-20s %-20s\n", "MOCK ID", "TITLE", "SUBJECT", "DURATION(MIN)", "LEVEL");
    int rowCount = 0;
    while (self->db.row = mysql_fetch_row(self->db.res))
    {
        int mockId = atoi(self->db.row[0] ? self->db.row[0] : "0");
        char *title = self->db.row[1] ? self->db.row[1] : "NULL";
        char *subject = self->db.row[2] ? self->db.row[2] : "NULL";
        char *duration = self->db.row[3] ? self->db.row[3] : "NULL";
        char *level = self->db.row[4] ? self->db.row[4] : "NULL";
        trim_newline(title);
        trim_newline(subject);
        trim_newline(duration);
        trim_newline(level);
        char concDuration[50];
        snprintf(concDuration, sizeof(concDuration), "%s min", duration);
        printf("%-10d %-20s %-20s %-20s %-20s\n", mockId, title, subject, concDuration, level);
        rowCount++;
    }
    if (rowCount == 0)
    {
        printf("No MCQs found!\n");
        mysql_free_result(self->db.res);
        return;
    }
    printf("-------------------------------------------------------------------------------------\n");
    int mcqId;
    printf("Enter MCQ ID to update: ");
    if (scanf("%d", &mcqId) != 1)
    {
        printf("Invalid input.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    getchar();
    char newTitle[200], newLevel[50];
    int newDuration;

    printf("Enter new title: ");
    fgets(newTitle, sizeof(newTitle), stdin);
    trim_newline(newTitle);

    printf("Enter new duration (in minutes): ");
    scanf("%d", &newDuration);
    getchar();

    printf("Enter new level (Easy/Medium/Hard): ");
    fgets(newLevel, sizeof(newLevel), stdin);
    trim_newline(newLevel);

    char updateQuery[512];
    sprintf(updateQuery,
            "UPDATE mockDetails SET title='%s', duration=%d, level='%s' WHERE mockId=%d;",
            newTitle, newDuration, newLevel, mcqId);

    if (mysql_query(self->db.conn, updateQuery))
    {
        fprintf(stderr, "Failed to update MCQ: %s\n", mysql_error(self->db.conn));
    }
    else
    {
        printf("MCQ details updated successfully!\n");
    }
    waitForEnter();
}

void deleteMCQ(TestSeries *self)
{
    system("clear");
    // getchar();
    if (mysql_query(self->db.conn, "SELECT mockId, title, subject, duration, level FROM mockDetails;"))
    {
        fprintf(stderr, "Failed to fetch mock data: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (self->db.res == NULL)
    {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(self->db.conn));
        return;
    }
    printf("=====================================================================================\n");
    printf("                                      MCQ DETAILS                                    \n");
    printf("=====================================================================================\n");
    printf("%-10s %-20s %-20s %-20s %-20s\n", "MOCK ID", "TITLE", "SUBJECT", "DURATION(MIN)", "LEVEL");
    int rowCount = 0;
    while (self->db.row = mysql_fetch_row(self->db.res))
    {
        int mockId = atoi(self->db.row[0] ? self->db.row[0] : "0");
        char *title = self->db.row[1] ? self->db.row[1] : "NULL";
        char *subject = self->db.row[2] ? self->db.row[2] : "NULL";
        char *duration = self->db.row[3] ? self->db.row[3] : "NULL";
        char *level = self->db.row[4] ? self->db.row[4] : "NULL";
        trim_newline(title);
        trim_newline(subject);
        trim_newline(duration);
        trim_newline(level);
        char concDuration[50];
        snprintf(concDuration, sizeof(concDuration), "%s min", duration);
        printf("%-10d %-20s %-20s %-20s %-20s\n", mockId, title, subject, concDuration, level);
        rowCount++;
    }
    if (rowCount == 0)
    {
        printf("No MCQs found!\n");
        mysql_free_result(self->db.res);
        return;
    }
    printf("-------------------------------------------------------------------------------------\n");
    int mcqId;
    printf("Enter MCQ ID to delete: ");
    if (scanf("%d", &mcqId) != 1)
    {
        printf("Invalid input.\n");
        while (getchar() != '\n')
            ;
        return;
    }

    char deleteQuery[512];
    sprintf(deleteQuery,
            "DELETE from mcqDetails WHERE mockId = %d;", mcqId);

    if (mysql_query(self->db.conn, deleteQuery))
    {
        fprintf(stderr, "Failed to delete MCQ: %s\n", mysql_error(self->db.conn));
    }
    else
    {
        printf("MCQ details delete successfully!\n");
    }
    waitForEnter();
}

void mcqDetails(TestSeries *self, char *subject)
{
    system("clear");
    char checkQuery[300];
    sprintf(checkQuery, "SELECT * FROM mockDetails WHERE subject='%s'", subject);
    if (mysql_query(self->db.conn, checkQuery))
    {
        fprintf(stderr, "Query Failed! %s\n", mysql_error(self->db.conn));
        return;
    }

    self->db.res = mysql_store_result(self->db.conn);

    if (!self->db.res || mysql_num_rows(self->db.res) == 0)
    {
        printf("\nNo MCQ found for subject: %s\n", subject);
        char ch;
        printf("Do you want to create a new MCQ? (Y/N): ");
        scanf(" %c", &ch);
        while (getchar() != '\n')
            ;
        if (toupper(ch) == 'Y')
            self->create(self, subject);
        return;
    }

    printf("====================================================\n");
    printf("MCQ Details for subject: %s\n", subject);
    printf("====================================================\n");

    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        int mockId = atoi(self->db.row[0] ? self->db.row[0] : "0");
        char *title = self->db.row[1] ? self->db.row[1] : "NULL";
        trim_newline(title);
        printf("%-10d %-20.20s\n", mockId, title);
    }

    int choice = 0, selected_mcq_id = -1;
    printf("\n1. Add Questions to Existing MCQ\n");
    printf("2. Create New MCQ\n");
    printf("3. Show Student Marks\n");
    printf("4. Back\n");
    printf("Enter choice (1-4): ");
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ;

    if (choice == 1)
    {
        printf("Enter MCQ ID to add questions: ");
        if (scanf("%d", &selected_mcq_id) == 1 && selected_mcq_id > 0)
        {
            while (getchar() != '\n')
                ;
            if (isAvailable(self, selected_mcq_id, subject))
                addQuestions(self, selected_mcq_id);
            else
                printf("Error: MCQ ID %d not found for subject '%s'.\n", selected_mcq_id, subject);
        }
    }
    else if (choice == 2)
    {
        self->create(self, subject);
    }
    else if (choice == 3)
    {
        printf("Enter MCQ ID to view marks: ");
        if (scanf("%d", &selected_mcq_id) == 1 && selected_mcq_id > 0)
        {
            while (getchar() != '\n')
                ;
            if (isAvailable(self, selected_mcq_id, subject))
                showMarks(self, selected_mcq_id, subject);
            else
                printf("Error: MCQ ID %d not found for subject '%s'.\n", selected_mcq_id, subject);
        }
    }
    else if (choice == 4)
    {
        return;
    }
    else
    {
        printf("Invalid choice");
    }

    mysql_free_result(self->db.res);
}

void createMCQ(TestSeries *self, char *subject)
{
    system("clear");
    getchar();
    printf("Enter MCQ title: ");
    fgets(self->title, 300, stdin);
    trim_newline(self->title);
    printf("Enter MCQ level(Easy, Medium, Hard): ");
    fgets(self->level, 100, stdin);
    trim_newline(self->level);
    printf("Enter duration(30 min): ");
    scanf("%d", &self->duration);
    while (getchar() != '\n')
        ;

    char createQuery[1024];
    sprintf(createQuery, "INSERT INTO mockDetails(title,subject,duration,level) VALUES('%s','%s',%d,'%s')",
            self->title, subject, self->duration, self->level);

    if (mysql_query(self->db.conn, createQuery))
        fprintf(stderr, "Failed to create MCQ: %s\n", mysql_error(self->db.conn));
    else
        printf("MCQ created successfully: %s\n", self->title);

    waitForEnter();
}

bool isAvailable(TestSeries *self, int mockId, char *subject)
{
    char checkQuery[300];
    sprintf(checkQuery, "SELECT * FROM mockDetails WHERE mockId=%d AND subject='%s' LIMIT 1", mockId, subject);
    if (mysql_query(self->db.conn, checkQuery))
    {
        fprintf(stderr, "Failed to find MCQ: %s\n", mysql_error(self->db.conn));
        return false;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res)
        return false;
    int rows = mysql_num_rows(self->db.res);
    mysql_free_result(self->db.res);
    return rows > 0;
}

void addQuestions(TestSeries *self, int mockId)
{
    int no_of_que;
    printf("Enter number of questions to add: ");
    scanf("%d", &no_of_que);
    getchar();

    for (int i = 0; i < no_of_que; i++)
    {
        printf("\n--- Question %d ---\n", i + 1);
        printf("Enter question: ");
        fgets(self->question, 2000, stdin);
        trim_newline(self->question);

        for (int j = 0; j < 4; j++)
        {
            printf("Enter option %d: ", j + 1);
            fgets(self->options[j], 200, stdin);
            trim_newline(self->options[j]);
        }

        printf("Enter correct answer (A, B, C, D): ");
        scanf(" %c", &self->correctAnswer);
        printf("Enter marks: ");
        scanf("%lf", &self->marks);
        while (getchar() != '\n')
            ;

        char insertQuery[500];
        sprintf(insertQuery,
                "INSERT INTO questionDetails(mockId,question,option1,option2,option3,option4,correctAnswer,marks) "
                "VALUES(%d,'%s','%s','%s','%s','%s','%c',%lf)",
                mockId, self->question, self->options[0], self->options[1], self->options[2], self->options[3], self->correctAnswer, self->marks);

        if (mysql_query(self->db.conn, insertQuery))
            fprintf(stderr, "Insert failed: %s\n", mysql_error(self->db.conn));
        else
            printf("Question %d added successfully!\n", i + 1);
    }
    waitForEnter();
}

void showMarks(TestSeries *self, int mockId, char *subject)
{
    char fetchQuery[500];
    sprintf(fetchQuery,
            "SELECT u.fullName, u.mobileNo, m.subject, SUM(um.totalMarks), SUM(um.obtainMarks), um.score "
            "FROM userMockDetails um "
            "JOIN user u ON um.studentId = u.id "
            "JOIN mockDetails m ON um.mockId = m.mockId "
            "WHERE m.subject='%s' AND m.mockId=%d "
            "GROUP BY u.id,u.fullName,u.mobileNo,m.subject,um.score;",
            subject, mockId);

    if (mysql_query(self->db.conn, fetchQuery))
    {
        fprintf(stderr, "Failed to fetch marks: %s\n", mysql_error(self->db.conn));
        return;
    }

    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res)
    {
        fprintf(stderr, "mysql_store_result failed\n");
        return;
    }

    printf("====================================================\n");
    printf("             STUDENT MARKS REPORT\n");
    printf("====================================================\n");
    printf("%-20s %-15s %-15s %-10s %-10s %-10s\n", "NAME", "MOBILE", "SUBJECT", "TOTAL", "OBTAINED", "SCORE");
    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        char *name = self->db.row[0] ? self->db.row[0] : "NULL";
        char *mobile = self->db.row[1] ? self->db.row[1] : "NULL";
        char *subject = self->db.row[2] ? self->db.row[2] : "NULL";
        int total = atoi(self->db.row[3] ? self->db.row[3] : "0");
        int obtain = atoi(self->db.row[4] ? self->db.row[4] : "0");
        float score = atof(self->db.row[5] ? self->db.row[5] : "0.0");

        trim_newline(name);
        trim_newline(mobile);
        trim_newline(subject);
        printf("%-20s %-15s %-15s %-10d %-10d %-10.2f\n", name, mobile, subject, total, obtain, score);
        printf("----------------------------------------------------\n");
    }
    waitForEnter();
}

void showQuestions(TestSeries *self, int mcqId)
{

    system("clear");
    getchar();
    char query[500];
    sprintf(query, "SELECT questionId, question, option1, option2, option3, option4, correctAnswer, marks FROM questionDetails WHERE mockId=%d", mcqId);

    if (mysql_query(self->db.conn, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(self->db.conn));
        return;
    }

    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res)
    {
        fprintf(stderr, "mysql_store_result failed\n");
        return;
    }

    int qnum = 1;
    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        char *question = self->db.row[1] ? self->db.row[1] : "NULL";
        char *option1 = self->db.row[2] ? self->db.row[2] : "NULL";
        char *option2 = self->db.row[3] ? self->db.row[3] : "NULL";
        char *option3 = self->db.row[4] ? self->db.row[4] : "NULL";
        char *option4 = self->db.row[5] ? self->db.row[5] : "NULL";
        char *correctAnswer = self->db.row[6] ? self->db.row[6] : "NULL";
        double marks = atof(self->db.row[7] ? self->db.row[7] : "0.0");

        trim_newline(question);
        trim_newline(option1);
        trim_newline(option2);
        trim_newline(option3);
        trim_newline(option4);
        trim_newline(correctAnswer);

        printf("Q%d: %s\n", qnum, question);
        printf("A) %s\n", option1);
        printf("B) %s\n", option2);
        printf("C) %s\n", option3);
        printf("D) %s\n", option4);
        printf("Correct Answer: %s\nMarks: %.2lf\n", correctAnswer, marks);
        qnum++;
    }

    mysql_free_result(self->db.res);
}

void showUserInfo(TestSeries *self)
{
    system("clear");
    char fetchQuery[300];
    sprintf(fetchQuery, "SELECT fullName, city, mobileNo, email FROM user WHERE role='student';");

    if (mysql_query(self->db.conn, fetchQuery))
    {
        fprintf(stderr, "Failed to fetch data: %s\n", mysql_error(self->db.conn));
        return;
    }

    self->db.res = mysql_store_result(self->db.conn);
    if (self->db.res == NULL)
    {
        fprintf(stderr, "mysql_store_result error: %s\n", mysql_error(self->db.conn));
        return;
    }

    printf("====================================================\n");
    printf("                 STUDENT INFORMATION                \n");
    printf("====================================================\n");
    printf("%-20s %-20s %-15s %-30s\n", "FULL NAME", "CITY", "MOBILE NO", "EMAIL");
    printf("----------------------------------------------------\n");

    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        char *fullName = self->db.row[0] ? self->db.row[0] : "NULL";
        char *city = self->db.row[1] ? self->db.row[1] : "NULL";
        char *mobileNo = self->db.row[2] ? self->db.row[2] : "NULL";
        char *email = self->db.row[3] ? self->db.row[3] : "NULL";

        trim_newline(fullName);
        trim_newline(city);
        trim_newline(mobileNo);
        trim_newline(email);

        printf("%-20s %-20s %-15s %-30s\n", fullName, city, mobileNo, email);
    }

    mysql_free_result(self->db.res);
    waitForEnter();
}

void subjectSelectionForStudent(TestSeries *self)
{
    int choice;
    while (1)
    {
        system("clear");
        printf("===== Select Subject =====\n");
        printf("1. C programming\n");
        printf("2. Java programming\n");
        printf("3. Database\n");
        printf("4. Back\n");
        printf("Choose subject: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input! Please enter a number between 1 and 4.\n");
            while (getchar() != '\n')
                ;
            waitForEnter();
            continue;
        }
        getchar();
        switch (choice)
        {
        case 1:
            showAllMcqForStudent(self, "C programming");
            break;
        case 2:
            showAllMcqForStudent(self, "C programming");
            break;
        case 3:
            showAllMcqForStudent(self, "C programming");
            break;
        case 4:
            showAllMcqForStudent(self, "C programming");
            break;
        default:
            printf("Invalid choice!\n");
            waitForEnter();
        }
    }
}

void showAllMcqForStudent(TestSeries *self, char *subject)
{
    system("clear");
    char fetchQuery[300];
    sprintf(fetchQuery,
            "SELECT mockId, title, subject, level FROM mockDetails WHERE subject='%s'",
            subject);
    if (mysql_query(self->db.conn, fetchQuery))
    {
        fprintf(stderr, "Failed to fetch MCQs: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (self->db.res == NULL)
    {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(self->db.conn));
        return;
    }
    printf("====================================================\n");
    printf("                    MCQ DETAILS                     \n");
    printf("====================================================\n");
    printf("%-10s %-20s %-20s %-15s\n", "MOCK ID", "TITLE", "SUBJECT", "LEVEL");
    printf("----------------------------------------------------\n");
    int count = 0;
    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        count++;
        int id = atoi(self->db.row[0] ? self->db.row[0] : "0");
        char *title = self->db.row[1] ? self->db.row[1] : "NULL";
        char *subj = self->db.row[2] ? self->db.row[2] : "NULL";
        char *level = self->db.row[3] ? self->db.row[3] : "NULL";
        trim_newline(title);
        trim_newline(subj);
        trim_newline(level);
        printf("%-10d %-20s %-20s %-15s\n", id, title, subj, level);
    }
    if (count == 0)
    {
        printf("No MCQs found for subject: %s\n", subject);
        mysql_free_result(self->db.res);
        waitForEnter();
        return;
    }
    printf("----------------------------------------------------\n");
    printf("1. Start Test\n");
    printf("2. Back\n");
    printf("Choose an option: ");
    int choice;
    if (scanf("%d", &choice) != 1)
    {
        printf("\nInvalid input! Please enter a number between 1 and 2.\n");
        while (getchar() != '\n')
            ;
        mysql_free_result(self->db.res);
        waitForEnter();
        return;
    }
    if (choice == 1)
    {
        int mcqId;
        printf("Enter MCQ ID to start Test: ");
        if (scanf("%d", &mcqId) != 1)
        {
            printf("Invalid MCQ ID!\n");
            while (getchar() != '\n')
                ;
            mysql_free_result(self->db.res);
            waitForEnter();
            return;
        }
        printf("Starting test...\n");
        sleep(1);
        mysql_free_result(self->db.res);
        startTest(self, mcqId);
    }
    else if (choice == 2)
    {
        mysql_free_result(self->db.res);
        return;
    }
    else
    {
        printf("Invalid choice!\n");
        mysql_free_result(self->db.res);
        waitForEnter();
    }
}

void startTest(TestSeries *self, int mockId)
{
    system("clear");
    char durationQuery[200];
    sprintf(durationQuery, "SELECT duration FROM mockDetails WHERE mockId=%d", mockId);
    if (mysql_query(self->db.conn, durationQuery))
    {
        fprintf(stderr, "Failed to fetch duration: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res || mysql_num_rows(self->db.res) == 0)
    {
        printf("No duration found for this test.\n");
        return;
    }
    self->db.row = mysql_fetch_row(self->db.res);
    int durationMin = atoi(self->db.row[0] ? self->db.row[0] : "0");
    mysql_free_result(self->db.res);
    if (durationMin <= 0)
    {
        printf("Invalid test duration.\n");
        return;
    }
    int durationSec = durationMin * 60;
    printf("Duration: %d minutes\n", durationMin);
    printf("Your test will start in 3 seconds...\n");
    sleep(3);
    system("clear");
    char query[400];
    sprintf(query,
            "SELECT question, option1, option2, option3, option4, correctAnswer "
            "FROM questionDetails WHERE mockId=%d",
            mockId);
    if (mysql_query(self->db.conn, query))
    {
        fprintf(stderr, "Error fetching questions: %s\n", mysql_error(self->db.conn));
        return;
    }
    self->db.res = mysql_store_result(self->db.conn);
    if (!self->db.res)
    {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(self->db.conn));
        return;
    }
    int total = 0, correct = 0;
    time_t start = time(NULL);
    bool timeUp = false;
    while ((self->db.row = mysql_fetch_row(self->db.res)))
    {
        total++;
        time_t now = time(NULL);
        int elapsed = (int)difftime(now, start);
        int remaining = durationSec - elapsed;
        if (remaining <= 0)
        {
            timeUp = true;
            break;
        }
        char *question = self->db.row[0] ? self->db.row[0] : "";
        char *a = self->db.row[1] ? self->db.row[1] : "";
        char *b = self->db.row[2] ? self->db.row[2] : "";
        char *c = self->db.row[3] ? self->db.row[3] : "";
        char *d = self->db.row[4] ? self->db.row[4] : "";
        char *correctAns = self->db.row[5] ? self->db.row[5] : "";
        trim_newline(question);
        trim_newline(a);
        trim_newline(b);
        trim_newline(c);
        trim_newline(d);
        trim_newline(correctAns);
        system("clear");
        printf("====================================================\n");
        printf("              ONLINE TEST SYSTEM\n");
        printf("====================================================\n");
        int mins = remaining / 60;
        int secs = remaining % 60;
        printf("Time Left: %02d:%02d\n", mins, secs);
        printf("----------------------------------------------------\n");
        printf("Q%d: %s\n", total, question);
        printf("----------------------------------------------------\n");
        printf("A) %s\n", a);
        printf("B) %s\n", b);
        printf("C) %s\n", c);
        printf("D) %s\n", d);
        printf("----------------------------------------------------\n");

        char answer = '\0';
        printf("Enter your answer (A/B/C/D): ");
        fflush(stdout);

        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        timeout.tv_sec = remaining;
        timeout.tv_usec = 0;
        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        if (rv == -1)
        {
            perror("select");
            timeUp = true;
            break;
        }
        else if (rv == 0)
        {
            printf("\nTime’s up for this question!\n");
            timeUp = true;
            break;
        }
        else
        {
            scanf(" %c", &answer);
        }
        now = time(NULL);
        elapsed = (int)difftime(now, start);
        remaining = durationSec - elapsed;
        if (remaining <= 0)
        {
            timeUp = true;
            break;
        }
        answer = toupper(answer);
        char *p = correctAns;
        while (*p == ' ' || *p == '\t' || *p == '\n')
            p++;
        char correctChar = toupper(*p);
        if (correctChar == answer)
        {
            correct++;
            printf("\nCorrect!\n");
        }
        else
        {
            printf("\nWrong! Correct Answer: %c\n", correctChar);
        }

        sleep(2);
    }

    mysql_free_result(self->db.res);
    printf("\n\n====================================================\n");
    printf("             Test Completed! \n");
    printf("====================================================\n");
    printf("Total Questions Attempted: %d\n", total);
    printf("Correct Answers: %d\n", correct);

    float score = total ? (correct * 100.0 / total) : 0.0;
    printf("Score: %.2f%%\n", score);
    printf("----------------------------------------------------\n");
    char insertQuery[400];
    sprintf(insertQuery,
            "INSERT INTO userMockDetails(mockId, studentId, totalMarks, obtainMarks, score) "
            "VALUES(%d, %d, %d, %d, %f)",
            mockId, glob_userId, total, correct, score);

    if (mysql_query(self->db.conn, insertQuery))
        fprintf(stderr, "Failed to save result: %s\n", mysql_error(self->db.conn));
    else
        printf("Result saved successfully!\n");

    printf("====================================================\n");
    waitForEnter();
}