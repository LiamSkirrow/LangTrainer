#include "noundata.h"

#include <QFile>
#include <QSet>

namespace {

int indentation(const QString &line)
{
    int spaces = 0;
    while (spaces < line.size() && line.at(spaces) == QLatin1Char(' '))
        ++spaces;
    return spaces;
}

QString unquote(QString value)
{
    value = value.trimmed();
    if (value.size() >= 2 &&
        ((value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"'))) ||
         (value.startsWith(QLatin1Char('\'')) && value.endsWith(QLatin1Char('\''))))) {
        value = value.mid(1, value.size() - 2);
    }
    return value;
}

QStringList inlineList(const QString &value)
{
    const int first = value.indexOf(QLatin1Char('['));
    const int last = value.indexOf(QLatin1Char(']'));
    if (first < 0 || last <= first)
        return {};
    QStringList result;
    for (const QString &part : value.mid(first + 1, last - first - 1)
                                   .split(QLatin1Char(','), Qt::SkipEmptyParts)) {
        result.append(unquote(part));
    }
    return result;
}

bool readResource(const QString &path, QStringList &lines, QString &error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = QStringLiteral("Could not open %1. Rebuild the project to include the YAML files.").arg(path);
        return false;
    }
    lines = QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'));
    return true;
}

} // namespace

QString NounCatalog::form(const QString &noun, const QString &caseKey) const
{
    return forms.value(caseKey).value(noun);
}

bool loadRussianNounCatalog(NounCatalog &catalog, QString &error)
{
    QStringList lines;
    if (!readResource(QStringLiteral(":/data/language.yaml"), lines, error))
        return false;

    NounCatalog loaded;
    bool inRussian = false;
    QString field;
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#')))
            continue;
        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inRussian = content == QLatin1String("Russian:");
            field.clear();
            continue;
        }
        if (!inRussian)
            continue;
        if (depth == 4 && content.endsWith(QLatin1Char(':'))) {
            field = content.left(content.size() - 1);
            continue;
        }
        if (depth != 6)
            continue;

        if (content.startsWith(QLatin1String("- "))) {
            if (field == QLatin1String("Nouns"))
                loaded.nouns.append(unquote(content.mid(2)));
            else if (field == QLatin1String("Prepositions"))
                loaded.prepositions.append(unquote(content.mid(2)));
            continue;
        }
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator <= 0)
            continue;
        const QString key = content.left(separator).trimmed();
        const QString value = unquote(content.mid(separator + 1));
        if (field == QLatin1String("Nouns-en"))
            loaded.english.insert(key, value);
        else if (field == QLatin1String("Nouns-gender"))
            loaded.gender.insert(key, value);
        else if (field.startsWith(QLatin1String("Nouns-singular-")))
            loaded.forms[field.mid(15)].insert(key, value);
        else if (field == QLatin1String("Prepositions-ru"))
            loaded.prepositionRussian.insert(key, value);
        else if (field == QLatin1String("Prepositions-en"))
            loaded.prepositionEnglish.insert(key, value);
        else if (field == QLatin1String("Prepositions-case"))
            loaded.prepositionCase.insert(key, value);
    }

    if (!readResource(QStringLiteral(":/data/spec.yaml"), lines, error))
        return false;
    inRussian = false;
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#')))
            continue;
        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inRussian = content == QLatin1String("Russian:");
            continue;
        }
        if (!inRussian || depth != 4)
            continue;
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator > 0 && content.left(separator) == QLatin1String("cases"))
            loaded.cases = inlineList(content.mid(separator + 1));
    }

    if (loaded.nouns.isEmpty() || loaded.cases.isEmpty() || loaded.prepositions.isEmpty()) {
        error = QStringLiteral("The Russian noun, case, or preposition list is empty.");
        return false;
    }
    QSet<QString> seen;
    for (const QString &noun : loaded.nouns) {
        if (seen.contains(noun)) {
            error = QStringLiteral("Duplicate Russian noun: %1.").arg(noun);
            return false;
        }
        seen.insert(noun);
        if (loaded.english.value(noun).isEmpty() || loaded.gender.value(noun).isEmpty()) {
            error = QStringLiteral("Missing English meaning or gender for Russian noun %1.").arg(noun);
            return false;
        }
        for (const QString &caseKey : loaded.cases) {
            if (loaded.form(noun, caseKey).isEmpty()) {
                error = QStringLiteral("Missing Nouns-singular-%1 for %2.").arg(caseKey, noun);
                return false;
            }
        }
    }
    seen.clear();
    for (const QString &preposition : loaded.prepositions) {
        if (seen.contains(preposition)) {
            error = QStringLiteral("Duplicate Russian preposition: %1.").arg(preposition);
            return false;
        }
        seen.insert(preposition);
        const QString caseKey = loaded.prepositionCase.value(preposition);
        if (loaded.prepositionRussian.value(preposition).isEmpty() ||
            loaded.prepositionEnglish.value(preposition).isEmpty() ||
            !loaded.cases.contains(caseKey) || caseKey == QLatin1String("nominative")) {
            error = QStringLiteral("Missing spelling, meaning, or valid governed case for %1.").arg(preposition);
            return false;
        }
    }
    catalog = loaded;
    error.clear();
    return true;
}

QString russianCaseName(const QString &caseKey)
{
    if (caseKey.isEmpty()) return {};
    return caseKey.left(1).toUpper() + caseKey.mid(1);
}
