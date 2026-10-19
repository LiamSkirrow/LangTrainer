#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

struct VerbCatalog {
    QStringList verbs;
    QHash<QString, QString> english;
    QStringList tenses;
    QStringList subjects;
    QHash<QString, QHash<QString, QString>> forms;
};

// Reads the Spanish verb fields from the bundled language.yaml and spec.yaml.
bool loadSpanishCatalog(VerbCatalog &catalog, QString &error);

// Display name used by the tense chooser and exercise prompt.
QString spanishTenseName(const QString &tense);
