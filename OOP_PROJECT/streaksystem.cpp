#include "streaksystem.h"
#include <QLabel>
#include <QCalendarWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QProgressBar>
#include <cstdlib>
#include <ctime>

StreakSystem::StreakSystem(QWidget *parent) : QWidget(parent), streakLabel(nullptr), levelLabel(nullptr),progressBar(nullptr),calendar(nullptr), logEdit(nullptr),logButton(nullptr), streakGroup(nullptr), calendarGroup(nullptr), logGroup(nullptr),currentStreak(0)
{
    std::srand(std::time(nullptr));
    setupUI();
    setupConnections();
    loadData();
}

StreakSystem::~StreakSystem()
{
    saveData();
}

void StreakSystem::setupUI()
{
    streakLabel = new QLabel("Your current streak is: 0");
    levelLabel = new QLabel("Level: 0");
    progressBar = new QProgressBar();
    progressBar->setRange(0, 7);
    progressBar->setValue(0);

    calendar = new QCalendarWidget();
    logEdit = new QTextEdit();
    logButton = new QPushButton("Log Session");

    streakGroup = new QGroupBox("Streak Tracker");
    calendarGroup = new QGroupBox("Calendar");
    logGroup = new QGroupBox("Log");

    QVBoxLayout *streakLayout = new QVBoxLayout();
    streakLayout->addWidget(streakLabel);
    streakLayout->addWidget(levelLabel);
    streakLayout->addWidget(progressBar);
    streakGroup->setLayout(streakLayout);

    QVBoxLayout *calendarLayout = new QVBoxLayout();
    calendarLayout->addWidget(calendar);
    calendarGroup->setLayout(calendarLayout);

    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(logEdit);
    logLayout->addWidget(logButton);
    logGroup->setLayout(logLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(streakGroup);
    mainLayout->addWidget(calendarGroup);
    mainLayout->addWidget(logGroup);
}

void StreakSystem::setupConnections()
{
    connect(logButton, &QPushButton::clicked, this, &StreakSystem::logSession);
    connect(calendar, &QCalendarWidget::selectionChanged, this, &StreakSystem::updateCalendar);
}

void StreakSystem::logSession()
{
    QDate selected = calendar->selectedDate();

    if (!studyDates.contains(selected)) {
        markStudyDay(selected);
        updateStreakDisplay();
        updateProgressBar();
        QMessageBox::information(this, "Motivation", getRandomQuote());
        checkAchievements();
    } else {
        QMessageBox::warning(this, "Already Logged", "This day has already been logged.");
    }
}

void StreakSystem::updateCalendar()
{
    QDate selectedDate = calendar->selectedDate();

    logEdit->append("----");

    if (studyDates.contains(selectedDate)) {
        logEdit->append(QString("%1 - Already logged").arg(selectedDate.toString("yyyy-MM-dd")));
    } else {
        logEdit->append(QString("%1 - Not yet logged").arg(selectedDate.toString("yyyy-MM-dd")));
    }
}

void StreakSystem::markStudyDay(const QDate &date)
{
    studyDates.insert(date);
    updateLog(date, "Study session logged.");
    recalculateCurrentStreak();
    saveData();
}

void StreakSystem::recalculateCurrentStreak()
{
    currentStreak = 0;
    QDate date = QDate::currentDate();

    while (studyDates.contains(date)) {
        currentStreak++;
        date = date.addDays(-1);
    }

    emit streakUpdated(currentStreak);
}

void StreakSystem::updateStreakDisplay()
{
    streakLabel->setText(QString("Your current streak is: %1").arg(currentStreak));
    levelLabel->setText(QString("Level: %1").arg(getLevel()));
}

void StreakSystem::updateProgressBar()
{
    progressBar->setValue(currentStreak > 7 ? 7 : currentStreak);
}

void StreakSystem::checkAchievements()
{
    if (currentStreak == 3) {
        QMessageBox::information(this, "Achievement Unlocked 🎉", "3-Day Streak! Keep it going!");
    } else if (currentStreak == 7) {
        QMessageBox::information(this, "Achievement Unlocked 🏅", "1 Week Streak! You're on fire!");
    } else if (currentStreak == 30) {
        QMessageBox::information(this, "Achievement Unlocked 🌟", "30-Day Streak! You're unstoppable!");
    }
}

QString StreakSystem::getRandomQuote() const
{
    QStringList quotes = {
        "Keep going, you're doing great!","One step at a time!", "Consistency is key.", "Progress, not perfection.","Every day counts!"
};
    return quotes[std::rand() % quotes.size()];
}

void StreakSystem::updateLog(const QDate &date, const QString &message)
{
    logEdit->append(QString("%1 - %2").arg(date.toString("yyyy-MM-dd"), message));
}

QString StreakSystem::getDataFilePath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path + "/streak_data.txt";
}

void StreakSystem::loadData()
{
    QFile file(getDataFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No existing data found.";
        return;
    }

    QTextStream in(&file);
    studyDates.clear();
    logEdit->clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QDate date = QDate::fromString(line, "yyyy-MM-dd");
        if (date.isValid()) {
            studyDates.insert(date);
            logEdit->append(QString("%1 - Previously logged").arg(date.toString("yyyy-MM-dd")));
        }
    }

    file.close();
    recalculateCurrentStreak();
    updateStreakDisplay();
    updateProgressBar();
}

void StreakSystem::saveData()
{
    QFile file(getDataFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not save data!";
        return;
    }

    QTextStream out(&file);
    for (const QDate &date : studyDates) {
        out << date.toString("yyyy-MM-dd") << "\n";
    }

    file.close();
}

int StreakSystem::getCurrentStreak() const
{
    return currentStreak;
}

int StreakSystem::getLevel() const
{
    return currentStreak / 7; // Each 7 days = 1 level
}
