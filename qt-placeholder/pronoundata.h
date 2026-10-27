#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

struct PronounCatalog {
    QStringList pronouns;
    QStringList cases;
    QHash<QString, QString> kind;
    QHash<QString, QString> lemma;
    QHash<QString, QString> english;
    QHash<QString, QHash<QString, QStringList>> forms;

    QStringList answers(const QString &pronoun, const QString &caseKey,
                        const QString &use = QString(),
                        const QString &referent = QString()) const;
    QString displayName(const QString &pronoun) const;
};

bool loadGermanPronounCatalog(PronounCatalog &catalog, QString &error);
QString pronounUseName(const QString &use);
QString pronounReferentName(const QString &referent);
