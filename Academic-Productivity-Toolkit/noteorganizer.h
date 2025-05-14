#ifndef NOTEORGANIZER_H
#define NOTEORGANIZER_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include <QSettings>

class NoteOrganizer : public QWidget
{
    Q_OBJECT

public:
    explicit NoteOrganizer(QWidget *parent = nullptr);
    ~NoteOrganizer();

    void loadData(QSettings *settings);
    void saveData(QSettings *settings);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newSubject();
    void deleteSubject();
    void newNote();
    void saveNote();
    void subjectSelected(QListWidgetItem *item);
    void noteSelected(QListWidgetItem *item);
    void noteContentChanged();

private:
    void setupUI();
    void setupConnections();
    void refreshSubjectsList();
    void loadNotesForSubject(const QString &subject);
    void saveCurrentNote();
    QString getNotesDir() const;
    bool validateSubjectName(const QString &name) const;
    bool validateNoteTitle(const QString &title) const;
    bool confirmUnsavedChanges();
    QString currentSubject() const;

    std::unique_ptr<QListWidget> subjectsList;
    std::unique_ptr<QListWidget> notesList;
    std::unique_ptr<QLineEdit> titleEdit;
    std::unique_ptr<QTextEdit> noteEdit;
    std::unique_ptr<QPushButton> newSubjectButton;
    std::unique_ptr<QPushButton> deleteSubjectButton;
    std::unique_ptr<QPushButton> newNoteButton;
    std::unique_ptr<QPushButton> saveNoteButton;
    std::unique_ptr<QLabel> subjectsLabel;
    std::unique_ptr<QLabel> notesLabel;
    std::unique_ptr<QLabel> titleLabel;
    std::unique_ptr<QLabel> contentLabel;

    QString currentNotePath;
    bool hasUnsavedChanges;
};

#endif
