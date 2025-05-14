#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QSettings>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QLabel;
class QPushButton;
class QCalendarWidget;
class QListWidget;
class QMenu;
class QAction;
class QScrollArea;
class GPACalculator;
class StudyPlanner;
class NoteOrganizer;
class StreakSystem;

class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
     void saveData(QSettings *settings);
    QPushButton* getGpaButton() const { return gpaButton.get(); }
    QPushButton* getStudyPlannerButton() const { return studyPlannerButton.get(); }
    QPushButton* getNotesButton() const { return notesButton.get(); }
    QPushButton* getStreakButton() const { return streakButton.get(); }
    void updateGpaDisplay(double gpa);
    void updateUpcomingTasks(const QStringList& tasks);
    void updateStreakDisplay(int streak);

    ~Dashboard();


private slots:
    void showHomeDashboard();
    void showGpaCalculator();
    void showStudyPlanner();
    void showNoteOrganizer();
    void showStreakSystem();


    void loadData(QSettings *settings);

    void backupData();
    void restoreFromBackup();
    void showSettings();
    void showUserGuide();
    void showAbout();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initializeUI();
    void initializeModules();
    void validateModules();
    void setupConnections();
    void createMenuBar();
    void createStatusBar();
    void createMainLayout();
    void createDashboardHome();
    void updateRecentFilesMenu();
    void saveWindowState(QSettings *settings);
    void loadWindowState(QSettings *settings);

    QScopedPointer<QSettings> settings;
    QString dataPath;
    QString currentTheme;


    QScopedPointer<QWidget> centralWidget;
    QScopedPointer<QScrollArea> scrollArea;
    QScopedPointer<QStackedWidget> stackedWidget;
    QScopedPointer<QWidget> dashboardHome;

    QScopedPointer<QLabel> gpaDisplay;
    QScopedPointer<QLabel> streakDisplay;
    QScopedPointer<QLabel> tasksDisplay;
    QScopedPointer<QLabel> notesDisplay;

    QScopedPointer<QCalendarWidget> calendarWidget;
    QScopedPointer<QListWidget> todayTasksList;
    QScopedPointer<QListWidget> upcomingExamsList;

    QScopedPointer<QPushButton> homeButton;
    QScopedPointer<QPushButton> gpaButton;
    QScopedPointer<QPushButton> studyPlannerButton;
    QScopedPointer<QPushButton> notesButton;
    QScopedPointer<QPushButton> streakButton;

    QScopedPointer<QMenu> recentFilesMenu;
    QScopedPointer<QAction> darkThemeAction;

    QScopedPointer<QVBoxLayout> mainLayout;
    QScopedPointer<QHBoxLayout> buttonLayout;

    QScopedPointer<GPACalculator> gpaCalculator;
    QScopedPointer<StudyPlanner> studyPlanner;
    QScopedPointer<NoteOrganizer> noteOrganizer;
    QScopedPointer<StreakSystem> streakSystem;

};

#endif
