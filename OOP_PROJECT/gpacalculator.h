#ifndef GPACALCULATOR_H
#define GPACALCULATOR_H

#include <QWidget>
#include <QMap>
#include <memory>

class QLineEdit;
class QComboBox;
class QSpinBox;
class QPushButton;
class QLCDNumber;
class QTableWidget;
class QTableWidgetItem;
class QSettings;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QGroupBox;

class GPACalculator : public QWidget
{
    Q_OBJECT

public:
    explicit GPACalculator(QWidget *parent = nullptr);
    ~GPACalculator();

    double getCurrentGpa() const;
    void loadData(QSettings *settings);
    void saveData(QSettings *settings);

signals:
    void gpaCalculated(double gpa);

private:
    void setupUI();
    void initializeGradeValues();
    void addCourse();
    void calculateGPA();
    void removeSelectedCourse();
    void updateGpaDisplay(double gpa);

    double currentGpa;
    QMap<QString, double> gradeValues;

    std::unique_ptr<QLineEdit> courseNameEdit;
    std::unique_ptr<QComboBox> gradeCombo;
    std::unique_ptr<QSpinBox> creditsSpin;
    std::unique_ptr<QPushButton> addButton;
    std::unique_ptr<QLCDNumber> gpaDisplay;
    std::unique_ptr<QPushButton> calcButton;
    std::unique_ptr<QPushButton> removeButton;
    std::unique_ptr<QTableWidget> coursesTable;
};

#endif
