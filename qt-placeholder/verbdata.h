#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

struct VerbCatalog {
    QString language;
    QStringList verbs;
    QHash<QString, QString> english;
    QStringList tenses;
    QStringList subjects;
    QHash<QString, QStringList> tenseSubjects;
    QHash<QString, QHash<QString, QString>> forms;

    QStringList subjectsForTense(const QString &tense) const;
};

// Reads one language's verb fields from the bundled language.yaml and spec.yaml.
bool loadVerbCatalog(const QString &language, VerbCatalog &catalog, QString &error);

// Display names used by the chooser, reference window, and exercise.
QString tenseName(const QString &language, const QString &tense);
QString subjectName(const QString &language, const QString &subject);
