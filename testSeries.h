#ifndef TESTSERIES_H
#define TESTSERIES_H

#include "database.h"
#include "globals.h"

typedef struct TestSeries TestSeries;

typedef struct TestSeries {
    char *title;
    char *subject;
    char *level;
    int totalQuestions;
    int totalMark;
    int duration;
    char *date;
    char *question;
    char *options[4];
    char correctAnswer;
    double marks;
    char *userResponse;
    Db db;

    void (*create)(struct TestSeries *self,char *subject);
    void (*display)(struct TestSeries *self);
    void (*marksDetails)(struct TestSeries *self);
    void (*update)(struct TestSeries *self);
    void (*delete)(struct TestSeries *self);
    void (*adminScreen)(struct TestSeries *self);
    void (*userScreen)(struct TestSeries *self);
    void (*subjectSelect)(struct TestSeries *self);
} TestSeries;

void subjectSelection(TestSeries *self);
void forAdminScreen(TestSeries *self);
void forUserScreen(TestSeries *self);
void initTestSeries(TestSeries *mcq);
void freeTestSeries(TestSeries *mcq);
void displayMCQ(TestSeries *self);
void createMCQ(TestSeries *self,char *subject);
void showMarks(TestSeries *self, int mockId, char *subject);
void updateMCQ(TestSeries *self);
void deleteMCQ(TestSeries *self);
bool isAvailable(TestSeries *self, int mockId, char *subject);
void addQuestions(TestSeries *self, int mockId);
void mcqDetails(TestSeries *self, char *subject);

#endif 