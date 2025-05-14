#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

// Forward declarations
class QTabWidget;
class QStatusBar;
class Dashboard;
class GPACalculator;
class StudyPlanner;
class NoteOrganizer;
class StreakSystem;
class QCloseEvent;
class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initializeUI();
    void setupConnections();
    void saveWindowState();
    void loadWindowState();

    std::unique_ptr<QTabWidget> tabWidget;
    std::unique_ptr<QStatusBar> statusBar;
    std::unique_ptr<Dashboard> dashboard;

};

#endif
