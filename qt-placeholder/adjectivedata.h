#pragma once

#include "noundata.h"

#include <QHash>
#include <QStringList>

struct AdjectiveCatalog {
    QStringList adjectives;
    QHash<QString, QString> english;
    QHash<QString, QString> stems;
    NounCatalog nouns;

    QString form(const QString &adjective, const QString &pattern,
                 const QString &caseKey, const QString &referent) const;
};

bool loadGermanAdjectiveCatalog(AdjectiveCatalog &catalog, QString &error);
QStringList adjectivePatterns();
QStringList adjectiveReferents();
QString adjectivePatternName(const QString &pattern);
QString adjectiveReferentName(const QString &referent);
QString adjectiveDeterminer(const QString &pattern, const QString &caseKey,
                            const QString &referent, int variant = 0);
