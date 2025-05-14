#include "noteorganizer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QCloseEvent>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QSignalBlocker>

NoteOrganizer::NoteOrganizer(QWidget *parent): QWidget(parent),
    hasUnsavedChanges(false)
{
    setupUI();
    setupConnections();
    refreshSubjectsList();
}

NoteOrganizer::~NoteOrganizer()
{
    saveCurrentNote();
}

void NoteOrganizer::setupUI()
{
    subjectsList = std::make_unique<QListWidget>(this);
    notesList = std::make_unique<QListWidget>(this);
    titleEdit = std::make_unique<QLineEdit>(this);
    noteEdit = std::make_unique<QTextEdit>(this);

     newSubjectButton = std::make_unique<QPushButton>(tr("New Subject"), this);
    deleteSubjectButton = std::make_unique<QPushButton>(tr("Delete Subject"), this);
    newNoteButton = std::make_unique<QPushButton>(tr("New Note"), this);
    saveNoteButton = std::make_unique<QPushButton>(tr("Save Note"), this);

     subjectsLabel = std::make_unique<QLabel>(tr("Subjects:"), this);
    notesLabel = std::make_unique<QLabel>(tr("Notes:"), this);
    titleLabel = std::make_unique<QLabel>(tr("Title:"), this);
    contentLabel = std::make_unique<QLabel>(tr("Content:"), this);

    auto leftLayout = new QVBoxLayout;
    leftLayout->addWidget(subjectsLabel.get());
    leftLayout->addWidget(subjectsList.get());
    leftLayout->addWidget(newSubjectButton.get());
    leftLayout->addWidget(deleteSubjectButton.get());

    auto middleLayout = new QVBoxLayout;
    middleLayout->addWidget(notesLabel.get());
    middleLayout->addWidget(notesList.get());
    middleLayout->addWidget(newNoteButton.get());
    middleLayout->addWidget(saveNoteButton.get());

    auto rightLayout = new QVBoxLayout;
    rightLayout->addWidget(titleLabel.get());
    rightLayout->addWidget(titleEdit.get());
    rightLayout->addWidget(contentLabel.get());
    rightLayout->addWidget(noteEdit.get());

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(middleLayout);
    mainLayout->addLayout(rightLayout);

    setWindowTitle(tr("Note Organizer"));
    setMinimumSize(800, 600);
}

void NoteOrganizer::setupConnections()
{
    connect(newSubjectButton.get(), &QPushButton::clicked,
            this, &NoteOrganizer::newSubject);
    connect(deleteSubjectButton.get(), &QPushButton::clicked,
            this, &NoteOrganizer::deleteSubject);
    connect(newNoteButton.get(), &QPushButton::clicked,
            this, &NoteOrganizer::newNote);
    connect(saveNoteButton.get(), &QPushButton::clicked,
            this, &NoteOrganizer::saveNote);
    connect(subjectsList.get(), &QListWidget::itemClicked,
            this, &NoteOrganizer::subjectSelected);
    connect(notesList.get(), &QListWidget::itemClicked,
            this, &NoteOrganizer::noteSelected);
    connect(noteEdit.get(), &QTextEdit::textChanged,
            this, &NoteOrganizer::noteContentChanged);
}

QString NoteOrganizer::getNotesDir() const
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/NoteOrganizer");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.absolutePath();
}

bool NoteOrganizer::validateSubjectName(const QString &name) const
{
    static QRegularExpression regex("^[a-zA-Z0-9 _-]+$");
    return regex.match(name).hasMatch();
}

bool NoteOrganizer::validateNoteTitle(const QString &title) const
{
    return !title.isEmpty() && !title.contains(QRegularExpression("[\\\\/:*?\"<>|]"));
}

bool NoteOrganizer::confirmUnsavedChanges()
{
    if (hasUnsavedChanges) {
        auto reply = QMessageBox::question(this, tr("Unsaved Changes"),
                                           tr("You have unsaved changes. Save them now?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            return false;
        }
        if (reply == QMessageBox::Yes) {
            saveNote();
        }
    }
    return true;
}

void NoteOrganizer::refreshSubjectsList()
{
    QSignalBlocker blocker(subjectsList.get());
    subjectsList->clear();

    QDir dir(getNotesDir());
    QStringList subjects = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    subjectsList->addItems(subjects);
}

void NoteOrganizer::newSubject()
{
    bool ok;
    QString subject = QInputDialog::getText(this, tr("New Subject"),
                                            tr("Enter subject name:"),QLineEdit::Normal, "", &ok);

    if (ok && !subject.isEmpty()) {
        if (!validateSubjectName(subject)) {
            QMessageBox::warning(this, tr("Invalid Name"),
                                 tr("Subject name contains invalid characters"));
            return;
        }

        QDir dir(getNotesDir());
        if (!dir.mkdir(subject)) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Failed to create subject directory"));
        } else {
            refreshSubjectsList();
        }
    }
}

void NoteOrganizer::deleteSubject()
{
    auto item = subjectsList->currentItem();
    if (!item) return;

    auto reply = QMessageBox::question(this, tr("Delete Subject"),
                                       tr("Are you sure you want to delete '%1' and all its notes?").arg(item->text()),
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QString path = getNotesDir() + "/" + item->text();
        QDir dir(path);
        if (!dir.removeRecursively()) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Failed to delete subject directory"));
        } else {
            refreshSubjectsList();
            notesList->clear();
            noteEdit->clear();
            titleEdit->clear();
            currentNotePath.clear();
        }
    }
}

QString NoteOrganizer::currentSubject() const
{
    auto item = subjectsList->currentItem();
    return item ? item->text() : QString();
}

void NoteOrganizer::loadNotesForSubject(const QString &subject)
{
    QSignalBlocker blocker(notesList.get());
    notesList->clear();

    if (subject.isEmpty()) return;

    QDir dir(getNotesDir() + "/" + subject);
    QStringList notes = dir.entryList(QStringList() << "*.txt", QDir::Files);
    for (const QString &note : notes) {
        notesList->addItem(QFileInfo(note).completeBaseName());
    }
}

void NoteOrganizer::subjectSelected(QListWidgetItem *item)
{
    if (!item) return;

    if (!confirmUnsavedChanges()) {
        // Restore previous selection if user cancels
        QSignalBlocker blocker(subjectsList.get());
        if (!currentSubject().isEmpty()) {
            QList<QListWidgetItem*> items = subjectsList->findItems(currentSubject(), Qt::MatchExactly);
            if (!items.isEmpty()) {
                subjectsList->setCurrentItem(items.first());
            }
        }
        return;
    }

    loadNotesForSubject(item->text());
    noteEdit->clear();
    titleEdit->clear();
    currentNotePath.clear();
    hasUnsavedChanges = false;
}

void NoteOrganizer::noteSelected(QListWidgetItem *item)
{
    if (!item) return;

    if (!confirmUnsavedChanges()) {
        // Restore previous selection if user cancels
        QSignalBlocker blocker(notesList.get());
        if (!currentNotePath.isEmpty()) {
            QString currentNote = QFileInfo(currentNotePath).completeBaseName();
            QList<QListWidgetItem*> items = notesList->findItems(currentNote, Qt::MatchExactly);
            if (!items.isEmpty()) {
                notesList->setCurrentItem(items.first());
            }
        }
        return;
    }

    QString subject = currentSubject();
    if (subject.isEmpty()) return;

    QString filePath = getNotesDir() + "/" + subject + "/" + item->text() + ".txt";

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        noteEdit->setPlainText(QTextStream(&file).readAll());
        titleEdit->setText(item->text());
        currentNotePath = filePath;
        hasUnsavedChanges = false;
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Could not open note file"));
    }
}

void NoteOrganizer::saveCurrentNote()
{
    if (hasUnsavedChanges && !currentNotePath.isEmpty()) {
        QFile file(currentNotePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << noteEdit->toPlainText();
            hasUnsavedChanges = false;
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Could not save note file"));
        }
    }
}

void NoteOrganizer::newNote()
{
    if (currentSubject().isEmpty()) {
        QMessageBox::warning(this, tr("No Subject Selected"),
                             tr("Please select a subject first"));
        return;
    }

    bool ok;
    QString title = QInputDialog::getText(this, tr("New Note"),
                                          tr("Enter note title:"),
                                          QLineEdit::Normal, "", &ok);

    if (ok && !title.isEmpty()) {
        if (!validateNoteTitle(title)) {
            QMessageBox::warning(this, tr("Invalid Title"),
                                 tr("Note title contains invalid characters"));
            return;
        }

        QString subject = currentSubject();
        QString filePath = getNotesDir() + "/" + subject + "/" + title + ".txt";

        if (QFile::exists(filePath)) {
            QMessageBox::warning(this, tr("Note Exists"),
                                 tr("A note with this title already exists"));
            return;
        }

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "";

            notesList->addItem(title);
            notesList->setCurrentRow(notesList->count() - 1);
            noteEdit->clear();
            titleEdit->setText(title);
            currentNotePath = filePath;
            hasUnsavedChanges = false;
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Could not create note file"));
        }
    }
}

void NoteOrganizer::saveNote()
{
    QString title = titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Note title cannot be empty"));
        return;
    }

    if (!validateNoteTitle(title)) {
        QMessageBox::warning(this, tr("Invalid Title"),
                             tr("Note title contains invalid characters"));
        return;
    }

    QString subject = currentSubject();
    if (subject.isEmpty()) {
        QMessageBox::warning(this, tr("No Subject Selected"),
                             tr("Please select a subject first"));
        return;
    }

    QString newFilePath = getNotesDir() + "/" + subject + "/" + title + ".txt";

    if (currentNotePath != newFilePath) {
        if (QFile::exists(newFilePath)) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("A note with this title already exists"));
            return;
        }

        if (!currentNotePath.isEmpty() && QFile::exists(currentNotePath)) {
            if (!QFile::rename(currentNotePath, newFilePath)) {
                QMessageBox::warning(this, tr("Error"),
                                     tr("Could not rename note file"));
                return;
            }
        }
    }

    QFile file(newFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << noteEdit->toPlainText();
        hasUnsavedChanges = false;
        currentNotePath = newFilePath;

        // Update note list if title changed
        if (auto item = notesList->currentItem()) {
            if (item->text() != title) {
                item->setText(title);
            }
        }
    } else {
        QMessageBox::warning(this, tr("Error"),
                             tr("Could not save note file"));
    }
}

void NoteOrganizer::noteContentChanged()
{
    hasUnsavedChanges = true;
}

void NoteOrganizer::loadData(QSettings *settings)
{
    if (!settings) return;

    settings->beginGroup("NoteOrganizer");
    QString lastSubject = settings->value("lastSubject").toString();
    QString lastNote = settings->value("lastNote").toString();
    settings->endGroup();

    if (!lastSubject.isEmpty()) {
        for (int i = 0; i < subjectsList->count(); ++i) {
            if (subjectsList->item(i)->text() == lastSubject) {
                subjectsList->setCurrentRow(i);
                loadNotesForSubject(lastSubject);

                if (!lastNote.isEmpty()) {
                    for (int j = 0; j < notesList->count(); ++j) {
                        if (notesList->item(j)->text() == lastNote) {
                            notesList->setCurrentRow(j);
                            noteSelected(notesList->currentItem());
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
}

void NoteOrganizer::saveData(QSettings *settings)
{
    if (!settings) return;

    settings->beginGroup("NoteOrganizer");
    settings->setValue("lastSubject", currentSubject());
    settings->setValue("lastNote", titleEdit->text());
    settings->endGroup();
}

void NoteOrganizer::closeEvent(QCloseEvent *event)
{
    if (confirmUnsavedChanges()) {
        event->accept();
    } else {
        event->ignore();
    }
}
