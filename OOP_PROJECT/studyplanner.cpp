#include "studyplanner.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QDate>
#include <QDebug>

StudyPlanner::StudyPlanner(QWidget *parent) : QWidget(parent),subjectCombo(nullptr), topicEdit(nullptr), deadlineEdit(nullptr),addButton(nullptr), completeButton(nullptr),tasksTable(nullptr),filterCombo(nullptr)
{
    setupUI();
    QSettings settings;
    loadData(&settings);
}

StudyPlanner::~StudyPlanner()
{
    QSettings settings;
    saveData(&settings);
    clearUI();
}

void StudyPlanner::clearUI()
{    if (addButton) disconnect(addButton, nullptr, nullptr, nullptr);
    if (completeButton) disconnect(completeButton, nullptr, nullptr, nullptr);
    if (filterCombo) disconnect(filterCombo, nullptr, nullptr, nullptr);
    if (tasksTable) disconnect(tasksTable, nullptr, nullptr, nullptr);

    taskList.clear();
}

void StudyPlanner::setupUI()
{

    if (layout()) {
        QLayoutItem* item;
        while ((item = layout()->takeAt(0))) {
            delete item->widget();
            delete item;
        }
        delete layout();
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *inputGroup = new QGroupBox("Add New Task");
    QFormLayout *formLayout = new QFormLayout;

    subjectCombo = new QComboBox(this);
    subjectCombo->addItems({"Math", "Physics", "Chemistry", "Biology", "Computer Science", "English", "History"});

    topicEdit = new QLineEdit(this);
    topicEdit->setPlaceholderText("Enter topic or chapter");

    deadlineEdit = new QDateTimeEdit(this);
    deadlineEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    deadlineEdit->setCalendarPopup(true);
    deadlineEdit->setMinimumDateTime(QDateTime::currentDateTime());

    formLayout->addRow("Subject:", subjectCombo);
    formLayout->addRow("Topic:", topicEdit);
    formLayout->addRow("Deadline:", deadlineEdit);

    addButton = new QPushButton("Add Task", this);
    addButton->setStyleSheet("QPushButton { padding: 5px; background-color: #4CAF50; color: white; }");
    connect(addButton, &QPushButton::clicked, this, &StudyPlanner::addTask);
    formLayout->addRow(addButton);

    inputGroup->setLayout(formLayout);

    QGroupBox *listGroup = new QGroupBox("Task List", this);
    QVBoxLayout *listLayout = new QVBoxLayout;

    tasksTable = new QTableWidget(0, 4, this);
    tasksTable->setHorizontalHeaderLabels({"", "Subject", "Topic", "Deadline"});
    tasksTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tasksTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tasksTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tasksTable->verticalHeader()->setVisible(false);
    tasksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    tasksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    tasksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterCombo = new QComboBox(this);
    populateFilterCombo();

    completeButton = new QPushButton("Mark Complete", this);
    completeButton->setStyleSheet("QPushButton { padding: 5px; background-color: #2196F3; color: white; }");
    completeButton->setEnabled(false);
    connect(completeButton, &QPushButton::clicked, this, &StudyPlanner::markTaskComplete);
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StudyPlanner::filterTasks);
    connect(tasksTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StudyPlanner::updateTaskList);

    filterLayout->addWidget(filterCombo);
    filterLayout->addWidget(completeButton);

    listLayout->addWidget(tasksTable);
    listLayout->addLayout(filterLayout);
    listGroup->setLayout(listLayout);

    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(listGroup);

    setLayout(mainLayout);
}

void StudyPlanner::populateFilterCombo()
{
    if (!filterCombo) return;

    filterCombo->clear();
    filterCombo->addItems({"All", "Pending", "Completed", "Due Today", "Due This Week", "Overdue"});

    if (subjectCombo) {
        for (int i = 0; i < subjectCombo->count(); ++i) {
            filterCombo->addItem(subjectCombo->itemText(i));
        }
    }
}

QStringList StudyPlanner::getUpcomingTasks() const
{
    QStringList upcoming;
    QDateTime now = QDateTime::currentDateTime();
    QDateTime endOfWeek = now.addDays(7);

    for (const Task &task : taskList) {
        if (!task.completed && task.deadline >= now && task.deadline <= endOfWeek) {
            upcoming.append(QString("%1: %2 (%3)").arg(task.subject, task.topic, task.deadline.toString("MMM d h:mm AP")));
        }
    }
    return upcoming;
}

void StudyPlanner::loadData(QSettings *settings)
{
    if (!settings) return;

    taskList.clear();

    settings->beginGroup("StudyPlanner");
    int size = settings->beginReadArray("tasks");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Task task;
        task.subject = settings->value("subject").toString();
        task.topic = settings->value("topic").toString();
        task.deadline = settings->value("deadline").toDateTime();
        task.completed = settings->value("completed", false).toBool();
        taskList.append(task);
    }
    settings->endArray();
    settings->endGroup();

    refreshTable();
    emit tasksUpdated(getUpcomingTasks());
}

void StudyPlanner::saveData(QSettings *settings)
{
    if (!settings) return;

    settings->beginGroup("StudyPlanner");
    settings->beginWriteArray("tasks");
    for (int i = 0; i < taskList.size(); ++i) {
        settings->setArrayIndex(i);
        settings->setValue("subject", taskList[i].subject);
        settings->setValue("topic", taskList[i].topic);
        settings->setValue("deadline", taskList[i].deadline);
        settings->setValue("completed", taskList[i].completed);
    }
    settings->endArray();
    settings->endGroup();

    settings->sync(); // Force write to disk
}

void StudyPlanner::addTask()
{
    if (!subjectCombo || !topicEdit || !deadlineEdit) return;

    QString subject = subjectCombo->currentText();
    QString topic = topicEdit->text().trimmed();
    QDateTime deadline = deadlineEdit->dateTime();

    if (topic.isEmpty()) {
        QMessageBox::warning(this, "Empty Topic", "Please enter a topic for the task.");
        return;
    }

    Task newTask;
    newTask.subject = subject;
    newTask.topic = topic;
    newTask.deadline = deadline;
    newTask.completed = false;

    taskList.append(newTask);
    refreshTable();

    topicEdit->clear();
    deadlineEdit->setDateTime(QDateTime::currentDateTime().addDays(1));

    emit tasksUpdated(getUpcomingTasks());
}

void StudyPlanner::markTaskComplete()
{
    if (!tasksTable) return;

    QModelIndexList selected = tasksTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    for (const QModelIndex &index : selected) {
        int row = tasksTable->item(index.row(), 1)->data(Qt::UserRole).toInt();
        if (row >= 0 && row < taskList.size()) {
            taskList[row].completed = true;
        }
    }

    refreshTable();
    emit tasksUpdated(getUpcomingTasks());
}

void StudyPlanner::filterTasks()
{
    if (!tasksTable || !filterCombo) return;

    QString filter = filterCombo->currentText();
    QDateTime now = QDateTime::currentDateTime();
    QDateTime endOfDay = now.addDays(1);
    endOfDay.setTime(QTime(23, 59, 59));
    QDateTime endOfWeek = now.addDays(7);

    for (int row = 0; row < tasksTable->rowCount(); ++row) {
        bool shouldShow = true;
        int taskIndex = tasksTable->item(row, 1)->data(Qt::UserRole).toInt();

        if (taskIndex < 0 || taskIndex >= taskList.size()) {
            tasksTable->setRowHidden(row, true);
            continue;
        }

        const Task &task = taskList[taskIndex];

        if (filter == "Pending") {
            shouldShow = !task.completed;
        }
        else if (filter == "Completed") {
            shouldShow = task.completed;
        }
        else if (filter == "Due Today") {
            shouldShow = !task.completed && task.deadline >= now && task.deadline <= endOfDay;
        }
        else if (filter == "Due This Week") {
            shouldShow = !task.completed && task.deadline >= now && task.deadline <= endOfWeek;
        }
        else if (filter == "Overdue") {
            shouldShow = !task.completed && task.deadline < now;
        }
        else if (filter != "All") {
            shouldShow = task.subject == filter;
        }

        tasksTable->setRowHidden(row, !shouldShow);
    }
}

void StudyPlanner::updateTaskList()
{
    if (completeButton) {
        completeButton->setEnabled(!tasksTable->selectionModel()->selectedRows().isEmpty());
    }
}

void StudyPlanner::refreshTable()
{
    if (!tasksTable) return;

    tasksTable->blockSignals(true);
    tasksTable->setRowCount(0);

    for (int i = 0; i < taskList.size(); ++i) {
        const Task &task = taskList[i];
        int row = tasksTable->rowCount();
        tasksTable->insertRow(row);

        QCheckBox *checkBox = new QCheckBox(tasksTable);
        checkBox->setChecked(task.completed);
        checkBox->setEnabled(false);
        tasksTable->setCellWidget(row, 0, checkBox);

        QTableWidgetItem *subjectItem = new QTableWidgetItem(task.subject);
        QTableWidgetItem *topicItem = new QTableWidgetItem(task.topic);
        QTableWidgetItem *deadlineItem = new QTableWidgetItem(task.deadline.toString("MMM d, yyyy h:mm AP"));

        subjectItem->setData(Qt::UserRole, i);
        topicItem->setData(Qt::UserRole, i);
        deadlineItem->setData(Qt::UserRole, i);

        if (task.completed) {
            subjectItem->setBackground(QColor(230, 230, 230));
            topicItem->setBackground(QColor(230, 230, 230));
            deadlineItem->setBackground(QColor(230, 230, 230));
            subjectItem->setForeground(Qt::gray);
            topicItem->setForeground(Qt::gray);
            deadlineItem->setForeground(Qt::gray);
        }
        else if (task.deadline < QDateTime::currentDateTime()) {
            deadlineItem->setForeground(Qt::red);
        }
        else if (task.deadline < QDateTime::currentDateTime().addDays(1)) {
            deadlineItem->setForeground(QColor(255, 140, 0));
        }

        tasksTable->setItem(row, 1, subjectItem);
        tasksTable->setItem(row, 2, topicItem);
        tasksTable->setItem(row, 3, deadlineItem);
    }
    tasksTable->blockSignals(false);
    filterTasks();
}
