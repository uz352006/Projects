#ifndef STREAKSYSTEM_H
#define STREAKSYSTEM_H

#include <QWidget>
#include <QSet>
#include <QDate>

class QLabel;
class QCalendarWidget;
class QTextEdit;
class QPushButton;
class QGroupBox;
class QVBoxLayout;
class QProgressBar;

class StreakSystem : public QWidget
{
    Q_OBJECT

public:
    explicit StreakSystem(QWidget *parent = nullptr);
    void loadData();
    void saveData();
    ~StreakSystem();
    int getCurrentStreak() const;

signals:
    void streakUpdated(int newStreak);

private slots:
    void logSession();
    void updateCalendar();
    void updateProgressBar();
    void checkAchievements();

private:
    void setupUI();
    void setupConnections();
    void markStudyDay(const QDate &date);
    void recalculateCurrentStreak();
    void updateStreakDisplay();
    void updateLog(const QDate &date, const QString &message);

    QString getDataFilePath() const;

    QString getRandomQuote() const;
    int getLevel() const;

    QLabel *streakLabel;
    QLabel *levelLabel;
    QProgressBar *progressBar;
    QCalendarWidget *calendar;
    QTextEdit *logEdit;
    QPushButton *logButton;
    QGroupBox *streakGroup;
    QGroupBox *calendarGroup;
    QGroupBox *logGroup;
    QSet<QDate> studyDates;
    int currentStreak;
};

#endif
