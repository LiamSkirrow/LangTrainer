#include "adjectivedata.h"

#include <QFile>
#include <QSet>

namespace {

const QStringList cases = {QStringLiteral("nominative"), QStringLiteral("accusative"),
                           QStringLiteral("dative"), QStringLiteral("genitive")};
const QStringList referents = {QStringLiteral("m"), QStringLiteral("f"),
                               QStringLiteral("n"), QStringLiteral("pl")};
const QStringList patterns = {QStringLiteral("definite"), QStringLiteral("mixed"),
                              QStringLiteral("none")};

// Indices: definite/mixed/no article, then case, then masculine/feminine/neuter/plural.
const char *const endings[3][4][4] = {
    {{"e", "e", "e", "en"}, {"en", "e", "e", "en"},
     {"en", "en", "en", "en"}, {"en", "en", "en", "en"}},
    {{"er", "e", "es", "en"}, {"en", "e", "es", "en"},
     {"en", "en", "en", "en"}, {"en", "en", "en", "en"}},
    {{"er", "e", "es", "e"}, {"en", "e", "es", "e"},
     {"em", "er", "em", "en"}, {"en", "er", "en", "er"}}
};

const char *const definiteArticles[4][4] = {
    {"der", "die", "das", "die"}, {"den", "die", "das", "die"},
    {"dem", "der", "dem", "den"}, {"des", "der", "des", "der"}
};
const char *const mixedEndings[4][4] = {
    {"", "e", "", "e"}, {"en", "e", "", "e"},
    {"em", "er", "em", "en"}, {"es", "er", "es", "er"}
};

int indentation(const QString &line)
{
    int count = 0;
    while (count < line.size() && line.at(count) == QLatin1Char(' ')) ++count;
    return count;
}

QString unquote(QString value)
{
    value = value.trimmed();
    if (value.size() >= 2 && value.startsWith(QLatin1Char('"')) &&
        value.endsWith(QLatin1Char('"')))
        return value.mid(1, value.size() - 2);
    return value;
}

} // namespace

QStringList adjectivePatterns() { return patterns; }
QStringList adjectiveReferents() { return referents; }

QString adjectivePatternName(const QString &pattern)
{
    if (pattern == QLatin1String("definite")) return QStringLiteral("Definite article (weak)");
    if (pattern == QLatin1String("mixed")) return QStringLiteral("Ein / kein / possessive (mixed)");
    if (pattern == QLatin1String("none")) return QStringLiteral("No article (strong)");
    return pattern;
}

QString adjectiveReferentName(const QString &referent)
{
    if (referent == QLatin1String("m")) return QStringLiteral("Masculine singular");
    if (referent == QLatin1String("f")) return QStringLiteral("Feminine singular");
    if (referent == QLatin1String("n")) return QStringLiteral("Neuter singular");
    if (referent == QLatin1String("pl")) return QStringLiteral("Plural");
    return referent;
}

QString AdjectiveCatalog::form(const QString &adjective, const QString &pattern,
                               const QString &caseKey, const QString &referent) const
{
    const int p = patterns.indexOf(pattern);
    const int c = cases.indexOf(caseKey);
    const int r = referents.indexOf(referent);
    if (!adjectives.contains(adjective) || p < 0 || c < 0 || r < 0) return {};
    return stems.value(adjective, adjective) + QString::fromLatin1(endings[p][c][r]);
}

QString adjectiveDeterminer(const QString &pattern, const QString &caseKey,
                            const QString &referent, int variant)
{
    const int c = cases.indexOf(caseKey);
    const int r = referents.indexOf(referent);
    if (c < 0 || r < 0) return {};
    if (pattern == QLatin1String("definite"))
        return QString::fromLatin1(definiteArticles[c][r]);
    if (pattern != QLatin1String("mixed")) return {};
    // Ein has no plural; use kein or mein for those cards.
    const int choice = variant % 3;
    const QString stem = choice == 2 ? QStringLiteral("mein")
        : choice == 1 || r == 3 ? QStringLiteral("kein") : QStringLiteral("ein");
    return stem + QString::fromLatin1(mixedEndings[c][r]);
}

bool loadGermanAdjectiveCatalog(AdjectiveCatalog &catalog, QString &error)
{
    AdjectiveCatalog loaded;
    if (!loadNounCatalog(QStringLiteral("German"), loaded.nouns, error)) return false;
    QFile file(QStringLiteral(":/data/language.yaml"));
    if (!file.open(QIODevice::ReadOnly)) {
        error = QStringLiteral("Could not open bundled language.yaml.");
        return false;
    }
    bool inGerman = false;
    QString field;
    const QStringList lines = QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#'))) continue;
        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inGerman = content == QLatin1String("German:");
            field.clear();
            continue;
        }
        if (!inGerman) continue;
        if (depth == 4 && content.endsWith(QLatin1Char(':'))) {
            field = content.chopped(1);
            continue;
        }
        if (depth != 6) continue;
        if (field == QLatin1String("Adjectives") && content.startsWith(QLatin1String("- "))) {
            loaded.adjectives.append(unquote(content.mid(2)));
            continue;
        }
        if (field != QLatin1String("Adjectives-en") &&
            field != QLatin1String("Adjectives-stem")) continue;
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator <= 0) continue;
        const QString adjective = content.left(separator).trimmed();
        const QString value = unquote(content.mid(separator + 1));
        if (field == QLatin1String("Adjectives-en"))
            loaded.english.insert(adjective, value);
        else
            loaded.stems.insert(adjective, value);
    }
    if (loaded.adjectives.isEmpty() || loaded.nouns.cases != cases) {
        error = QStringLiteral("The German adjective list or four-case specification is invalid.");
        return false;
    }
    QSet<QString> seen;
    for (const QString &adjective : loaded.adjectives) {
        if (seen.contains(adjective) || loaded.english.value(adjective).isEmpty() ||
            (loaded.stems.contains(adjective) && loaded.stems.value(adjective).isEmpty())) {
            error = QStringLiteral("Invalid or duplicate German adjective: %1.").arg(adjective);
            return false;
        }
        seen.insert(adjective);
        for (const QString &pattern : patterns)
            for (const QString &caseKey : cases)
                for (const QString &referent : referents)
                    if (loaded.form(adjective, pattern, caseKey, referent).isEmpty()) {
                        error = QStringLiteral("Missing adjective form for %1.").arg(adjective);
                        return false;
                    }
    }
    for (const QString &noun : loaded.nouns.nouns) {
        if (loaded.nouns.gender.value(noun) == QLatin1String("m") ||
            loaded.nouns.gender.value(noun) == QLatin1String("f") ||
            loaded.nouns.gender.value(noun) == QLatin1String("n")) continue;
        error = QStringLiteral("Invalid German noun gender for adjective context: %1.").arg(noun);
        return false;
    }
    for (const QString &gender : {QStringLiteral("m"), QStringLiteral("f"), QStringLiteral("n")}) {
        bool found = false;
        for (const QString &noun : loaded.nouns.nouns)
            found |= loaded.nouns.gender.value(noun) == gender;
        if (!found) {
            error = QStringLiteral("No %1 noun for German adjective cards.").arg(gender);
            return false;
        }
    }
    catalog = loaded;
    error.clear();
    return true;
}
