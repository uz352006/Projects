#ifndef STUDYPLANNER_H
#define STUDYPLANNER_H

#include <QWidget>
#include <QSettings>
#include <QDateTime>

class QComboBox;
class QLineEdit;
class QDateTimeEdit;
class QTableWidget;
class QPushButton;

struct Task {
    QString subject;
    QString topic;
    QDateTime deadline;
    bool completed = false;
};

class StudyPlanner : public QWidget {
    Q_OBJECT

public:
    explicit StudyPlanner(QWidget *parent = nullptr);
    ~StudyPlanner();

    QStringList getUpcomingTasks() const;
    void loadData(QSettings *settings);
    void saveData(QSettings *settings);

signals:
    void tasksUpdated(const QStringList &upcomingTasks);

private slots:
    void addTask();
    void markTaskComplete();
    void filterTasks();
    void updateTaskList();

private:
    void setupUI();
    void clearUI();
    void populateFilterCombo();
    void refreshTable();

    QComboBox *subjectCombo;
    QLineEdit *topicEdit;
    QDateTimeEdit *deadlineEdit;
    QPushButton *addButton;
    QPushButton *completeButton;
    QTableWidget *tasksTable;
    QComboBox *filterCombo;

    QList<Task> taskList;
};

#endif
