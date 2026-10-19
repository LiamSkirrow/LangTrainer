#include "verbdata.h"

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
    const QStringList parts = value.mid(first + 1, last - first - 1)
                                  .split(QLatin1Char(','), Qt::SkipEmptyParts);
    for (const QString &part : parts)
        result.append(unquote(part));
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

bool loadSpanishCatalog(VerbCatalog &catalog, QString &error)
{
    QStringList lines;
    if (!readResource(QStringLiteral(":/data/language.yaml"), lines, error))
        return false;

    VerbCatalog loaded;
    bool inSpanish = false;
    QString field;
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#')))
            continue;

        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inSpanish = content == QLatin1String("Spanish:");
            field.clear();
            continue;
        }
        if (!inSpanish)
            continue;
        if (depth == 4 && content.endsWith(QLatin1Char(':'))) {
            field = content.left(content.size() - 1);
            continue;
        }
        if (depth != 6)
            continue;

        if (field == QLatin1String("Verbs") && content.startsWith(QLatin1String("- "))) {
            loaded.verbs.append(unquote(content.mid(2)));
            continue;
        }
        if (!field.startsWith(QLatin1String("Verbs-")))
            continue;
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator <= 0)
            continue;
        const QString verb = content.left(separator).trimmed();
        loaded.forms[field].insert(verb, unquote(content.mid(separator + 1)));
    }

    if (!readResource(QStringLiteral(":/data/spec.yaml"), lines, error))
        return false;
    inSpanish = false;
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#')))
            continue;
        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inSpanish = content == QLatin1String("Spanish:");
            continue;
        }
        if (!inSpanish || depth != 4)
            continue;
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator <= 0)
            continue;
        const QString key = content.left(separator);
        if (key == QLatin1String("tenses"))
            loaded.tenses = inlineList(content.mid(separator + 1));
        else if (key == QLatin1String("verbConjugations"))
            loaded.subjects = inlineList(content.mid(separator + 1));
    }

    if (loaded.verbs.isEmpty() || loaded.tenses.isEmpty() || loaded.subjects.isEmpty()) {
        error = QStringLiteral("The Spanish verb list or practice spec is empty.");
        return false;
    }

    QSet<QString> seen;
    for (const QString &verb : loaded.verbs) {
        if (seen.contains(verb)) {
            error = QStringLiteral("Duplicate verb in language.yaml: %1").arg(verb);
            return false;
        }
        seen.insert(verb);
        const QString english = loaded.forms.value(QStringLiteral("Verbs-en-inf")).value(verb);
        if (english.isEmpty()) {
            error = QStringLiteral("Missing English meaning for %1.").arg(verb);
            return false;
        }
        loaded.english.insert(verb, english);
        for (const QString &tense : loaded.tenses) {
            for (const QString &subject : loaded.subjects) {
                const QString key = QStringLiteral("Verbs-%1-%2").arg(tense, subject);
                if (loaded.forms.value(key).value(verb).isEmpty()) {
                    error = QStringLiteral("Missing %1 for %2 in language.yaml.").arg(key, verb);
                    return false;
                }
            }
        }
    }

    catalog = loaded;
    error.clear();
    return true;
}

QString spanishTenseName(const QString &tense)
{
    if (tense == QLatin1String("present")) return QStringLiteral("Present");
    if (tense == QLatin1String("present_cont")) return QStringLiteral("Present progressive");
    if (tense == QLatin1String("simple_past")) return QStringLiteral("Preterite");
    if (tense == QLatin1String("imperfect_past")) return QStringLiteral("Imperfect");
    if (tense == QLatin1String("present_perf")) return QStringLiteral("Present perfect");
    if (tense == QLatin1String("pluperfect")) return QStringLiteral("Past perfect");
    if (tense == QLatin1String("future")) return QStringLiteral("Future");
    if (tense == QLatin1String("future_perf")) return QStringLiteral("Future perfect");
    if (tense == QLatin1String("conditional")) return QStringLiteral("Conditional");
    if (tense == QLatin1String("conditional_perf")) return QStringLiteral("Conditional perfect");
    QString label = tense;
    return label.replace(QLatin1Char('_'), QLatin1Char(' '));
}

QString spanishSubjectName(const QString &subject)
{
    if (subject == QLatin1String("1psing")) return QStringLiteral("yo");
    if (subject == QLatin1String("1ppl")) return QStringLiteral("nosotros / nosotras");
    if (subject == QLatin1String("2pfam")) return QStringLiteral("tú");
    if (subject == QLatin1String("2ppol")) return QStringLiteral("usted");
    if (subject == QLatin1String("2ppl")) return QStringLiteral("ustedes");
    if (subject == QLatin1String("3psing")) return QStringLiteral("él / ella");
    if (subject == QLatin1String("3ppl")) return QStringLiteral("ellos / ellas");
    return subject;
}
