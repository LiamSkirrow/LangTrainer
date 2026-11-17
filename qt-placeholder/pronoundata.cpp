#include "pronoundata.h"

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

QString formKey(const QString &kind, const QString &caseKey,
                const QString &use, const QString &referent)
{
    if (kind == QLatin1String("personal"))
        return QStringLiteral("personal-%1").arg(caseKey);
    return QStringLiteral("%1-%2-%3").arg(use, referent, caseKey);
}

} // namespace

QStringList PronounCatalog::answers(const QString &pronoun, const QString &caseKey,
                                    const QString &use, const QString &referent) const
{
    return forms.value(formKey(kind.value(pronoun), caseKey, use, referent)).value(pronoun);
}

QString PronounCatalog::displayName(const QString &pronoun) const
{
    return QStringLiteral("%1 — %2").arg(lemma.value(pronoun), english.value(pronoun));
}

bool loadGermanPronounCatalog(PronounCatalog &catalog, QString &error)
{
    QStringList lines;
    if (!readResource(QStringLiteral(":/data/language.yaml"), lines, error))
        return false;

    PronounCatalog loaded;
    bool inGerman = false;
    QString field;
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#')))
            continue;
        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inGerman = content == QLatin1String("German:");
            field.clear();
            continue;
        }
        if (!inGerman)
            continue;
        if (depth == 4 && content.endsWith(QLatin1Char(':'))) {
            field = content.left(content.size() - 1);
            continue;
        }
        if (depth != 6)
            continue;
        if (field == QLatin1String("Pronouns") && content.startsWith(QLatin1String("- "))) {
            loaded.pronouns.append(unquote(content.mid(2)));
            continue;
        }
        if (!field.startsWith(QLatin1String("Pronouns-")))
            continue;
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator <= 0)
            continue;
        const QString key = content.left(separator).trimmed();
        const QString value = unquote(content.mid(separator + 1));
        if (field == QLatin1String("Pronouns-kind"))
            loaded.kind.insert(key, value);
        else if (field == QLatin1String("Pronouns-lemma"))
            loaded.lemma.insert(key, value);
        else if (field == QLatin1String("Pronouns-en"))
            loaded.english.insert(key, value);
        else {
            QStringList variants;
            for (const QString &form : value.split(QLatin1Char('|'), Qt::SkipEmptyParts))
                variants.append(form.trimmed());
            loaded.forms[field.mid(9)].insert(key, variants);
        }
    }

    if (!readResource(QStringLiteral(":/data/spec.yaml"), lines, error))
        return false;
    inGerman = false;
    for (const QString &line : lines) {
        const QString content = line.trimmed();
        if (content.isEmpty() || content.startsWith(QLatin1Char('#')))
            continue;
        const int depth = indentation(line);
        if (depth == 2 && content.endsWith(QLatin1Char(':'))) {
            inGerman = content == QLatin1String("German:");
            continue;
        }
        if (!inGerman || depth != 4)
            continue;
        const int separator = content.indexOf(QLatin1Char(':'));
        if (separator > 0 && content.left(separator) == QLatin1String("cases"))
            loaded.cases = inlineList(content.mid(separator + 1));
    }

    if (loaded.pronouns.isEmpty() || loaded.cases.isEmpty()) {
        error = QStringLiteral("The German pronoun list or case spec is empty.");
        return false;
    }
    QSet<QString> seen;
    for (const QString &pronoun : loaded.pronouns) {
        const QString kind = loaded.kind.value(pronoun);
        if (seen.contains(pronoun) || loaded.lemma.value(pronoun).isEmpty() ||
            loaded.english.value(pronoun).isEmpty() ||
            (kind != QLatin1String("personal") && kind != QLatin1String("possessive"))) {
            error = QStringLiteral("Invalid or duplicate German pronoun entry: %1.").arg(pronoun);
            return false;
        }
        seen.insert(pronoun);
        for (const QString &caseKey : loaded.cases) {
            if (kind == QLatin1String("personal")) {
                if (loaded.answers(pronoun, caseKey).isEmpty()) {
                    error = QStringLiteral("Missing personal %1 form for %2.").arg(caseKey, pronoun);
                    return false;
                }
            } else {
                for (const QString &use : {QStringLiteral("before"), QStringLiteral("alone")}) {
                    for (const QString &referent : {QStringLiteral("m"), QStringLiteral("f"),
                                                    QStringLiteral("n"), QStringLiteral("pl")}) {
                        if (loaded.answers(pronoun, caseKey, use, referent).isEmpty()) {
                            error = QStringLiteral("Missing %1 %2 %3 form for %4.")
                                        .arg(use, referent, caseKey, pronoun);
                            return false;
                        }
                    }
                }
            }
        }
    }
    catalog = loaded;
    error.clear();
    return true;
}

QString pronounUseName(const QString &use)
{
    return use == QLatin1String("before")
        ? QStringLiteral("Before a noun") : QStringLiteral("Standalone (replaces the noun)");
}

QString pronounReferentName(const QString &referent)
{
    if (referent == QLatin1String("m")) return QStringLiteral("Masculine singular (der Mann)");
    if (referent == QLatin1String("f")) return QStringLiteral("Feminine singular (die Frau)");
    if (referent == QLatin1String("n")) return QStringLiteral("Neuter singular (das Kind)");
    if (referent == QLatin1String("pl")) return QStringLiteral("Plural (die Kinder)");
    return referent;
}
