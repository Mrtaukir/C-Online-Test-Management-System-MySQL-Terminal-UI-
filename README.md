# C Online Test Management System (MySQL + Terminal UI)

A complete terminal-based **Online Test System** written in **C language**, using **MySQL** as the backend database.  
It provides an interactive environment for conducting multiple-choice tests, with real-time countdown timers, automatic result calculation, and a clean terminal-based interface.

---

## Features

### Admin Panel
- Create and manage subjects and MCQs.
- Add, edit, and delete questions.
- Set test duration and difficulty level.
- View student information and performance reports.

### Student Panel
- Select subjects and available MCQs.
- Attempt live timed tests with countdown.
- Receive instant feedback on each question.
- Auto-submit when time runs out.
- View scores saved to the database.

---

## Key Functionalities
- **Dynamic Timer:** Live countdown in real-time.
- **Auto Submission:** Ends test automatically when time is over.
- **Database Integration:** MySQL stores all users, questions, and results.
- **Cross-Platform:** Works on any Linux terminal.
- **Clean UI:** Exam-style layout for focused experience.

---

## Tech Stack
- **Language:** C  
- **Database:** MySQL  
- **Libraries:** `<mysql/mysql.h>`, `<pthread.h>`, `<unistd.h>`, `<time.h>`  
- **Compiler Flags:**  
  ```bash
  gcc -o test globals.c database.c user.c testSeries.c `mysql_config --cflags --libs`
  ./test

