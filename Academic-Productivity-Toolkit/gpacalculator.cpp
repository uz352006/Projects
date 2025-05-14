#include "gpacalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLCDNumber>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSettings>

GPACalculator::GPACalculator(QWidget *parent)
    : QWidget(parent),
    currentGpa(0.0)
{
    setupUI();
    initializeGradeValues();
}

GPACalculator::~GPACalculator() = default;

void GPACalculator::setupUI()
{
    // Main layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // Input group
    auto inputGroup = new QGroupBox("Add New Course", this);
    auto formLayout = new QFormLayout();

    courseNameEdit = std::make_unique<QLineEdit>();
    courseNameEdit->setPlaceholderText("Enter course name");
    formLayout->addRow("Course Name:", courseNameEdit.get());

    gradeCombo = std::make_unique<QComboBox>();
    gradeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    formLayout->addRow("Grade:", gradeCombo.get());

    creditsSpin = std::make_unique<QSpinBox>();
    creditsSpin->setRange(1, 5);
    creditsSpin->setValue(3);
    formLayout->addRow("Credits:", creditsSpin.get());

    auto buttonLayout = new QHBoxLayout();
    addButton = std::make_unique<QPushButton>("Add Course");
    addButton->setStyleSheet("QPushButton { padding: 5px; background-color: #4CAF50; color: white; }");
    connect(addButton.get(), &QPushButton::clicked, this, &GPACalculator::addCourse);
    buttonLayout->addWidget(addButton.get());

    formLayout->addRow(buttonLayout);
    inputGroup->setLayout(formLayout);
    auto displayGroup = new QGroupBox("GPA Information", this);
    auto displayLayout = new QVBoxLayout();

    gpaDisplay = std::make_unique<QLCDNumber>();
    gpaDisplay->setDigitCount(4);
    gpaDisplay->setSegmentStyle(QLCDNumber::Flat);
    gpaDisplay->setMinimumHeight(60);
    updateGpaDisplay(0.0);
    displayLayout->addWidget(gpaDisplay.get());

    auto actionLayout = new QHBoxLayout();
    calcButton = std::make_unique<QPushButton>("Calculate GPA");
    calcButton->setStyleSheet("QPushButton { padding: 5px; background-color: #2196F3; color: white; }");
    connect(calcButton.get(), &QPushButton::clicked, this, &GPACalculator::calculateGPA);

    removeButton = std::make_unique<QPushButton>("Remove Selected");
    removeButton->setStyleSheet("QPushButton { padding: 5px; background-color: #f44336; color: white; }");
    connect(removeButton.get(), &QPushButton::clicked, this, &GPACalculator::removeSelectedCourse);

    actionLayout->addWidget(calcButton.get());
    actionLayout->addWidget(removeButton.get());
    displayLayout->addLayout(actionLayout);

    coursesTable = std::make_unique<QTableWidget>(0, 3);
    coursesTable->setHorizontalHeaderLabels({"Course", "Grade", "Credits"});
    coursesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    coursesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    coursesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    coursesTable->verticalHeader()->setVisible(false);
    coursesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    coursesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    displayLayout->addWidget(coursesTable.get());

    displayGroup->setLayout(displayLayout);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(displayGroup);
}

double GPACalculator::getCurrentGpa() const
{
    return currentGpa;
}

void GPACalculator::loadData(QSettings *settings)
{
    if (!settings) {
        qWarning() << "Invalid settings object provided";
        return;
    }

    settings->beginGroup("GPACalculator");
    currentGpa = settings->value("currentGpa", 0.0).toDouble();

    const int size = settings->beginReadArray("courses");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        const QString course = settings->value("name").toString();
        const QString grade = settings->value("grade").toString();
        const int credits = settings->value("credits").toInt();

        if (coursesTable) {
            const int row = coursesTable->rowCount();
            coursesTable->insertRow(row);
            coursesTable->setItem(row, 0, new QTableWidgetItem(course));
            coursesTable->setItem(row, 1, new QTableWidgetItem(grade));
            coursesTable->setItem(row, 2, new QTableWidgetItem(QString::number(credits)));
        }
    }
    settings->endArray();
    settings->endGroup();

    updateGpaDisplay(currentGpa);
}

void GPACalculator::saveData(QSettings *settings)
{
    if (!settings) {
        qWarning() << "Invalid settings object provided";
        return;
    }

    settings->beginGroup("GPACalculator");
    settings->setValue("currentGpa", currentGpa);

    if (coursesTable) {
        settings->beginWriteArray("courses");
        for (int i = 0; i < coursesTable->rowCount(); ++i) {
            settings->setArrayIndex(i);
            settings->setValue("name", coursesTable->item(i, 0)->text());
            settings->setValue("grade", coursesTable->item(i, 1)->text());
            settings->setValue("credits", coursesTable->item(i, 2)->text().toInt());
        }
        settings->endArray();
    }
    settings->endGroup();
}

void GPACalculator::initializeGradeValues()
{
    gradeValues = {
        {"A+", 4.0}, {"A", 4.0}, {"A-", 3.7},
        {"B+", 3.3}, {"B", 3.0}, {"B-", 2.7},
        {"C+", 2.3}, {"C", 2.0}, {"C-", 1.7},
        {"D+", 1.3}, {"D", 1.0}, {"F", 0.0}
    };

    if (gradeCombo) {
        gradeCombo->clear();
        for (const auto &grade : gradeValues.keys()) {
            gradeCombo->addItem(grade);
        }
    }
}

void GPACalculator::addCourse()
{
    if (!courseNameEdit || !gradeCombo || !creditsSpin || !coursesTable) {
        qWarning() << "UI components not properly initialized";
        return;
    }

    const QString course = courseNameEdit->text().trimmed();
    const QString grade = gradeCombo->currentText();
    const int credits = creditsSpin->value();

    if (course.isEmpty()) {
        QMessageBox::warning(this, "Empty Course Name", "Please enter a course name.");
        return;
    }

    const int row = coursesTable->rowCount();
    coursesTable->insertRow(row);
    coursesTable->setItem(row, 0, new QTableWidgetItem(course));
    coursesTable->setItem(row, 1, new QTableWidgetItem(grade));
    coursesTable->setItem(row, 2, new QTableWidgetItem(QString::number(credits)));

    courseNameEdit->clear();
    creditsSpin->setValue(3);
    courseNameEdit->setFocus();
}

void GPACalculator::calculateGPA()
{
    if (!coursesTable) {
        qWarning() << "Courses table not initialized";
        return;
    }

    double totalPoints = 0.0;
    int totalCredits = 0;

    for (int row = 0; row < coursesTable->rowCount(); ++row) {
        const QString grade = coursesTable->item(row, 1)->text();
        const int credits = coursesTable->item(row, 2)->text().toInt();
        totalPoints += gradeValues.value(grade, 0.0) * credits;
        totalCredits += credits;
    }

    currentGpa = totalCredits > 0 ? totalPoints / totalCredits : 0.0;
    updateGpaDisplay(currentGpa);
    emit gpaCalculated(currentGpa);
}

void GPACalculator::removeSelectedCourse()
{
    if (!coursesTable) {
        qWarning() << "Courses table not initialized";
        return;
    }

    const QModelIndexList selected = coursesTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "No Selection", "Please select a course to remove.");
        return;
    }

    for (int i = selected.count() - 1; i >= 0; --i) {
        coursesTable->removeRow(selected.at(i).row());
    }

    calculateGPA(); //recalculate gpa after removing course
}

void GPACalculator::updateGpaDisplay(double gpa)
{
    if (!gpaDisplay) {
        qWarning() << "GPA display not initialized";
        return;
    }

    gpaDisplay->display(gpa);

    // Color coding based on GPA
    QString style;
    if (gpa >= 3.5) {
        style = "background-color: #4CAF50; color: white;";
    } else if (gpa >= 2.5) {
        style = "background-color: #FFC107; color: black;";
    } else {
        style = "background-color: #F44336; color: white;";
    }

    gpaDisplay->setStyleSheet(style);
}
