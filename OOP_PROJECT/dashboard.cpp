#include "dashboard.h"
#include "gpacalculator.h"
#include "studyplanner.h"
#include "noteorganizer.h"
#include "streaksystem.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QCalendarWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPalette>
#include <QApplication>
#include <QScrollArea>
#include <QDir>
#include <QFileInfo>

Dashboard::Dashboard(QWidget *parent) : QMainWindow(parent),settings(new QSettings("AcademicPro", "Dashboard", this)),dataPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
{
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    initializeUI();
    initializeModules();
    validateModules();
    setupConnections();

    if (settings) {
        loadWindowState(settings.data());
        loadData(settings.data());
    }

    setWindowTitle(tr("Academic Pro"));
    resize(1200, 800);
    setMinimumSize(1000, 700);
}

Dashboard::~Dashboard()
{
    if (settings) {
        saveWindowState(settings.data());
        saveData(settings.data());
    }
}

void Dashboard::initializeUI()
{
    centralWidget.reset(new QWidget(this));
    scrollArea.reset(new QScrollArea(this));
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(centralWidget.data());
    setCentralWidget(scrollArea.data());

    stackedWidget.reset(new QStackedWidget(centralWidget.data()));
    dashboardHome.reset(new QWidget(centralWidget.data()));

    createDashboardHome();
    createMenuBar();
    createStatusBar();
    createMainLayout();
}

void Dashboard::createDashboardHome()
{
    QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboardHome.data());
    dashboardLayout->setSpacing(25);
    dashboardLayout->setContentsMargins(25, 25, 25, 25);
    QLabel *welcomeHeader = new QLabel(tr("Welcome back, Student! 👋"), dashboardHome.data());
    welcomeHeader->setStyleSheet("font-size: 28px; font-weight: bold;");
    dashboardLayout->addWidget(welcomeHeader);
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);
    auto createStatCard = [this](const QString& title, QLabel* valueLabel, QLabel* subtext, const QString& color) {
        QGroupBox *card = new QGroupBox(title, dashboardHome.data());
        QVBoxLayout *layout = new QVBoxLayout();

        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet(QString("font-size: 36px; font-weight: bold; color: %1;").arg(color));

        subtext->setAlignment(Qt::AlignCenter);
        subtext->setStyleSheet("color: palette(mid); font-size: 14px;");

        layout->addWidget(valueLabel);
        layout->addWidget(subtext);
        layout->setSpacing(5);
        card->setLayout(layout);
        card->setStyleSheet("QGroupBox ""{"" border: 1px solid palette(mid);""   border-radius: 10px;" "   padding: 15px;""   margin-top: 20px;""}"
               "QGroupBox::title {""   subcontrol-origin: margin;""   left: 10px;" "   padding: 0 5px;""}"
            );
        return card;
    };
    gpaDisplay.reset(new QLabel("3.72", dashboardHome.data()));
    QLabel *gpaSubtext = new QLabel(tr("Up 0.15 from last semester"), dashboardHome.data());
    QGroupBox *gpaCard = createStatCard(tr("📊 GPA"), gpaDisplay.data(), gpaSubtext, "#4361ee");

    streakDisplay.reset(new QLabel("14", dashboardHome.data()));
    QLabel *streakSubtext = new QLabel(tr("Current streak"), dashboardHome.data());
    QGroupBox *streakCard = createStatCard(tr("🔥 Streak"), streakDisplay.data(), streakSubtext, "#f72585");

    tasksDisplay.reset(new QLabel("5", dashboardHome.data()));
    QLabel *tasksSubtext = new QLabel(tr("2 due today"), dashboardHome.data());
    QGroupBox *tasksCard = createStatCard(tr("📝 Tasks"), tasksDisplay.data(), tasksSubtext, "#4895ef");

    notesDisplay.reset(new QLabel("27", dashboardHome.data()));
    QLabel *notesSubtext = new QLabel(tr("1 new update"), dashboardHome.data());
    QGroupBox *notesCard = createStatCard(tr("📚 Notes"), notesDisplay.data(), notesSubtext, "#4cc9f0");

    statsLayout->addWidget(gpaCard);
    statsLayout->addWidget(streakCard);
    statsLayout->addWidget(tasksCard);
    statsLayout->addWidget(notesCard);
    dashboardLayout->addLayout(statsLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(25);

    QGroupBox *calendarGroup = new QGroupBox(tr("📅 Calendar"), dashboardHome.data());
    QVBoxLayout *calendarLayout = new QVBoxLayout();
    calendarWidget.reset(new QCalendarWidget(calendarGroup));
    calendarWidget->setGridVisible(true);
    calendarWidget->setStyleSheet(
        "QCalendarWidget { border: 1px solid palette(mid); border-radius: 8px; }"
        "QCalendarWidget QToolButton { height: 30px; font-size: 14px; }"
        "QCalendarWidget QMenu { width: 150px; }");
    calendarLayout->addWidget(calendarWidget.data());
    calendarGroup->setLayout(calendarLayout);
    calendarGroup->setStyleSheet(
        "QGroupBox { border: 1px solid palette(mid); border-radius: 10px; padding: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");
    QVBoxLayout *rightColumn = new QVBoxLayout();
    rightColumn->setSpacing(20);

    QGroupBox *tasksGroup = new QGroupBox(tr("✅ Today's Tasks"), dashboardHome.data());
    QVBoxLayout *tasksGroupLayout = new QVBoxLayout();
    todayTasksList.reset(new QListWidget(tasksGroup));
    todayTasksList->addItems(QStringList()<< "Complete OOP Assignment (Due 11:59 PM)"<< "Read Database Chapter 5 (Due 6:10 PM)" << "Prepare for Calculus Quiz (Tomorrow 5:00 AM)"); todayTasksList->setStyleSheet("QListWidget { border: 1px solid palette(mid); border-radius: 8px; padding: 5px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid palette(mid); }");
    tasksGroupLayout->addWidget(todayTasksList.data());
    tasksGroup->setLayout(tasksGroupLayout);
    tasksGroup->setStyleSheet(
        "QGroupBox { border: 1px solid palette(mid); border-radius: 10px; padding: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }" );
    QGroupBox *examsGroup = new QGroupBox(tr("📝 Upcoming Exams"), dashboardHome.data());
    QVBoxLayout *examsLayout = new QVBoxLayout();
    upcomingExamsList.reset(new QListWidget(examsGroup));
    upcomingExamsList->addItems(QStringList()
                                << "Data Structures Midterm - May 12 (10:00 AM)" << "Algorithms Quiz - May 14 (2:00 PM)" << "Database Systems Final - May 20 (9:00 AM)");
    upcomingExamsList->setStyleSheet("QListWidget { border: 1px solid palette(mid); border-radius: 8px; padding: 5px; }" "QListWidget::item { padding: 8px; border-bottom: 1px solid palette(mid); }"
        );
    examsLayout->addWidget(upcomingExamsList.data());
    examsGroup->setLayout(examsLayout);
    examsGroup->setStyleSheet(
        "QGroupBox { border: 1px solid palette(mid); border-radius: 10px; padding: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }");

    rightColumn->addWidget(tasksGroup);
    rightColumn->addWidget(examsGroup);
    contentLayout->addWidget(calendarGroup, 2);
    contentLayout->addLayout(rightColumn, 1);
    dashboardLayout->addLayout(contentLayout, 1);

    stackedWidget->addWidget(dashboardHome.data());
}

void Dashboard::createMenuBar()
{
    menuBar()->clear();

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *saveAction = fileMenu->addAction(tr("&Save Data"));
    QAction *loadAction = fileMenu->addAction(tr("&Load Data"));
    recentFilesMenu.reset(fileMenu->addMenu(tr("Recent Files")));
    updateRecentFilesMenu();

    QAction *backupAction = fileMenu->addAction(tr("&Backup Data"));
    QAction *restoreAction = fileMenu->addAction(tr("&Restore Backup"));
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("E&xit"));

    connect(saveAction, &QAction::triggered, this, [this]() { if (settings) saveData(settings.data()); });
    connect(loadAction, &QAction::triggered, this, [this]() { if (settings) loadData(settings.data()); });
    connect(backupAction, &QAction::triggered, this, &Dashboard::backupData);
    connect(restoreAction, &QAction::triggered, this, &Dashboard::restoreFromBackup);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QAction *settingsAction = toolsMenu->addAction(tr("&Settings"));
    connect(settingsAction, &QAction::triggered, this, &Dashboard::showSettings);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *guideAction = helpMenu->addAction(tr("&User Guide"));
    QAction *aboutAction = helpMenu->addAction(tr("&About"));

    connect(guideAction, &QAction::triggered, this, &Dashboard::showUserGuide);
    connect(aboutAction, &QAction::triggered, this, &Dashboard::showAbout);
}

void Dashboard::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void Dashboard::createMainLayout()
{
    mainLayout.reset(new QVBoxLayout(centralWidget.data()));
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    homeButton.reset(new QPushButton(tr("🏠 Dashboard"), centralWidget.data()));
    gpaButton.reset(new QPushButton(tr("📊 GPA Calculator"), centralWidget.data()));
    studyPlannerButton.reset(new QPushButton(tr("📅 Study Planner"), centralWidget.data()));
    notesButton.reset(new QPushButton(tr("📝 Notes"), centralWidget.data()));
    streakButton.reset(new QPushButton(tr("🔥 Streak"), centralWidget.data()));

    QString buttonStyle = "QPushButton { padding: 10px 15px; font-size: 14px; border-radius: 6px; }";
    homeButton->setStyleSheet(buttonStyle);
    gpaButton->setStyleSheet(buttonStyle);
    studyPlannerButton->setStyleSheet(buttonStyle);
    notesButton->setStyleSheet(buttonStyle);
    streakButton->setStyleSheet(buttonStyle);

    buttonLayout.reset(new QHBoxLayout());
    buttonLayout->addWidget(homeButton.data());
    buttonLayout->addWidget(gpaButton.data());
    buttonLayout->addWidget(studyPlannerButton.data());
    buttonLayout->addWidget(notesButton.data());
    buttonLayout->addWidget(streakButton.data());
    buttonLayout->setSpacing(10);

    mainLayout->addLayout(buttonLayout.data());
    mainLayout->addWidget(stackedWidget.data(), 1);
}

void Dashboard::initializeModules()
{
    gpaCalculator.reset(new GPACalculator(this));
    studyPlanner.reset(new StudyPlanner(this));
    noteOrganizer.reset(new NoteOrganizer(this));
    streakSystem.reset(new StreakSystem(this));

    if (!gpaCalculator.isNull()) stackedWidget->addWidget(gpaCalculator.data());
    if (!studyPlanner.isNull()) stackedWidget->addWidget(studyPlanner.data());
    if (!noteOrganizer.isNull()) stackedWidget->addWidget(noteOrganizer.data());
    if (!streakSystem.isNull()) stackedWidget->addWidget(streakSystem.data());
}

void Dashboard::validateModules()
{
    if (gpaCalculator.isNull()) throw std::runtime_error("GPA Calculator initialization failed");
    if (studyPlanner.isNull()) throw std::runtime_error("Study Planner initialization failed");
    if (noteOrganizer.isNull()) throw std::runtime_error("Note Organizer initialization failed");
    if (streakSystem.isNull()) throw std::runtime_error("Streak System initialization failed");
}

void Dashboard::setupConnections()
{
    connect(homeButton.data(), &QPushButton::clicked, this, &Dashboard::showHomeDashboard);
    connect(gpaButton.data(), &QPushButton::clicked, this, &Dashboard::showGpaCalculator);
    connect(studyPlannerButton.data(), &QPushButton::clicked, this, &Dashboard::showStudyPlanner);
    connect(notesButton.data(), &QPushButton::clicked, this, &Dashboard::showNoteOrganizer);
    connect(streakButton.data(), &QPushButton::clicked, this, &Dashboard::showStreakSystem);

    connect(streakSystem.data(), &StreakSystem::streakUpdated, this, &Dashboard::updateStreakDisplay);
    connect(studyPlanner.data(), &StudyPlanner::tasksUpdated, this, &Dashboard::updateUpcomingTasks);
    connect(gpaCalculator.data(), &GPACalculator::gpaCalculated, this, &Dashboard::updateGpaDisplay);
}

void Dashboard::updateStreakDisplay(int streak)
{
    if (!streakDisplay) return;

    QString color;
    if (streak == 0) color = "#6c757d";
    else if (streak < 3) color = "#f72585";
    else if (streak < 7) color = "#4895ef";
    else color = "#4cc9f0";

    streakDisplay->setText(QString::number(streak));
    streakDisplay->setStyleSheet(QString("font-size: 36px; font-weight: bold; color: %1;").arg(color));
}

void Dashboard::updateGpaDisplay(double gpa)
{
    if (!gpaDisplay) return;

    QString color;
    if (gpa < 2.0) color = "#f72585";
    else if (gpa < 3.0) color = "#f8961e";
    else color = "#43aa8b";

    gpaDisplay->setText(QString::number(gpa, 'f', 2));
    gpaDisplay->setStyleSheet(QString("font-size: 36px; font-weight: bold; color: %1;").arg(color));
}

void Dashboard::updateUpcomingTasks(const QStringList& tasks)
{
    if (todayTasksList) {
        todayTasksList->clear();
        todayTasksList->addItems(tasks);
    }
}

void Dashboard::showHomeDashboard()
{
    if (stackedWidget && dashboardHome) {
        stackedWidget->setCurrentWidget(dashboardHome.data());
        statusBar()->showMessage(tr("Home Dashboard"));
    }
}

void Dashboard::showGpaCalculator()
{
    if (stackedWidget && gpaCalculator) {
        stackedWidget->setCurrentWidget(gpaCalculator.data());
        statusBar()->showMessage(tr("GPA Calculator - Enter your course grades"));
    }
}

void Dashboard::showStudyPlanner()
{
    if (stackedWidget && studyPlanner) {
        stackedWidget->setCurrentWidget(studyPlanner.data());
        statusBar()->showMessage(tr("Study Planner - Schedule your study sessions"));
    }
}

void Dashboard::showNoteOrganizer()
{
    if (stackedWidget && noteOrganizer) {
        stackedWidget->setCurrentWidget(noteOrganizer.data());
        statusBar()->showMessage(tr("Note Organizer - Manage your lecture notes"));
    }
}

void Dashboard::showStreakSystem()
{
    if (stackedWidget && streakSystem) {
        stackedWidget->setCurrentWidget(streakSystem.data());
        statusBar()->showMessage(tr("Study Streak - Track your study consistency"));
    }
}

void Dashboard::showSettings()
{
    QMessageBox::information(this, tr("Settings"),
                             tr("Application settings:\n\n"
                                "- Data location: %1").arg(dataPath));
}

void Dashboard::showUserGuide()
{
    QMessageBox::information(this, tr("User Guide"),
                             tr("Academic Pro User Guide:\n\n"
                                "1. Dashboard - Overview of your academic progress\n"
                                "2. GPA Calculator - Track and calculate your GPA\n"
                                "3. Study Planner - Schedule study sessions\n"
                                "4. Notes - Organize your lecture notes\n"
                                "5. Streak - Track your study consistency"));
}

void Dashboard::showAbout()
{
    QMessageBox::about(this, tr("About Academic Pro"),
                       tr("<b>Academic Pro</b><br><br>"
                          "<b>Version:</b> 2.0<br>"
                          "<b>Release Date:</b> 2025<br><br>"
                          "Academic Pro is a comprehensive and user-friendly software solution designed to simplify and enhance the academic experience. "
                          "With a range of powerful tools and features, it helps students, educators, and institutions manage tasks such as file handling, text editing, and project organization with ease.<br><br>"
                          "<b>Features:</b><br>"
                          "• Easy-to-use text editor for managing academic content.<br>"
                          "• Seamless file import/export with multiple file formats.<br>"
                          "• Intuitive user interface for smooth navigation.<br><br>"
                          "<b>Developers:</b> Umama Zubair, Amna Sami, Rameen Ramzan<br>"
                          "© 2025 Academic Pro Team"));

}

void Dashboard::loadData(QSettings *settings)
{
    if (!settings) return;
    if (gpaCalculator) gpaCalculator->loadData(settings);
    if (studyPlanner) studyPlanner->loadData(settings);
    if (noteOrganizer) noteOrganizer->loadData(settings);
    if (streakSystem) streakSystem->loadData();
    if (streakSystem) updateStreakDisplay(streakSystem->getCurrentStreak());
    if (gpaCalculator) updateGpaDisplay(gpaCalculator->getCurrentGpa());
    if (studyPlanner) updateUpcomingTasks(studyPlanner->getUpcomingTasks());

    statusBar()->showMessage(tr("Data loaded successfully"), 3000);
}

void Dashboard::saveData(QSettings *settings)
{
    if (!settings) return;

    if (gpaCalculator) gpaCalculator->saveData(settings);
    if (studyPlanner) studyPlanner->saveData(settings);
    if (noteOrganizer) noteOrganizer->saveData(settings);
    if (streakSystem) streakSystem->saveData();

    updateRecentFilesMenu();
    statusBar()->showMessage(tr("Data saved successfully"), 3000);
}

void Dashboard::backupData()
{
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/AcademicProBackup.ini";
    QString backupPath = QFileDialog::getSaveFileName(this, tr("Save Backup"), defaultPath, tr("INI Files (*.ini)"));

    if (!backupPath.isEmpty() && settings) {
        if (QFile::copy(settings->fileName(), backupPath)) {
            statusBar()->showMessage(tr("Backup created successfully"), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to create backup at: %1").arg(backupPath));
        }
    }
}

void Dashboard::restoreFromBackup()
{
    QString backupPath = QFileDialog::getOpenFileName(this, tr("Restore Backup"),
                                                      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),tr("INI Files (*.ini)"));

    if (!backupPath.isEmpty() && settings) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Confirm"),
 tr("This will overwrite all current data. Continue?"), QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (QFile::copy(backupPath, settings->fileName())) {
                settings->sync();
                loadWindowState(settings.data());
                loadData(settings.data());
                statusBar()->showMessage(tr("Backup restored successfully"), 3000);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to restore backup from: %1").arg(backupPath));
            }
        }
    }
}

void Dashboard::updateRecentFilesMenu()
{
    if (!recentFilesMenu || !settings) return;

    QStringList recentFiles = settings->value("recentFiles").toStringList();
    recentFiles.removeAll(settings->fileName());
    recentFiles.prepend(settings->fileName());

    while (recentFiles.size() > 5) {
        recentFiles.removeLast();
    }

    settings->setValue("recentFiles", recentFiles);
    recentFilesMenu->clear();

    for (const QString &file : recentFiles) {
        QAction *action = recentFilesMenu->addAction(QFileInfo(file).fileName());
        connect(action, &QAction::triggered, this, [this, file]() {
            if (QFile::exists(file) && settings) {
                settings->sync();
                loadData(settings.data());
            } else {
                QMessageBox::warning(this, tr("Error"), tr("File not found: %1").arg(file));
            }
        });
    }
}

void Dashboard::saveWindowState(QSettings *settings)
{
    if (!settings) return;

    settings->beginGroup("WindowState");
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->setValue("maximized", isMaximized());
    settings->endGroup();
}

void Dashboard::loadWindowState(QSettings *settings)
{
    if (!settings) return;

    settings->beginGroup("WindowState");
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());
    if (settings->value("maximized", false).toBool()) {
        showMaximized();
    }
    settings->endGroup();
}

void Dashboard::closeEvent(QCloseEvent *event)
{
    if (settings) {
        saveWindowState(settings.data());
        saveData(settings.data());
    }
    event->accept();
}
