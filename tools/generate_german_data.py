"""Regenerate the German block of language.yaml from reviewed paradigms.

Run from the repository root with: python3 tools/generate_german_data.py
Each six-form list is ordered ich, du, er/sie/es, wir, ihr, sie/Sie.
"""

from pathlib import Path


def six(forms):
    result = forms.split()
    if len(result) != 6:
        raise ValueError(f"Expected six forms: {forms}")
    return result


# meaning, present, simple past, past participle, perfect auxiliary
verbs = {
    "sein": ("be", six("bin bist ist sind seid sind"), six("war warst war waren wart waren"), "gewesen", "sein"),
    "haben": ("have", six("habe hast hat haben habt haben"), six("hatte hattest hatte hatten hattet hatten"), "gehabt", "haben"),
    "werden": ("become", six("werde wirst wird werden werdet werden"), six("wurde wurdest wurde wurden wurdet wurden"), "geworden", "sein"),
    "machen": ("do / make", six("mache machst macht machen macht machen"), six("machte machtest machte machten machtet machten"), "gemacht", "haben"),
    "gehen": ("go", six("gehe gehst geht gehen geht gehen"), six("ging gingst ging gingen gingt gingen"), "gegangen", "sein"),
    "kommen": ("come", six("komme kommst kommt kommen kommt kommen"), six("kam kamst kam kamen kamt kamen"), "gekommen", "sein"),
    "sagen": ("say", six("sage sagst sagt sagen sagt sagen"), six("sagte sagtest sagte sagten sagtet sagten"), "gesagt", "haben"),
    "sehen": ("see", six("sehe siehst sieht sehen seht sehen"), six("sah sahst sah sahen saht sahen"), "gesehen", "haben"),
    "geben": ("give", six("gebe gibst gibt geben gebt geben"), six("gab gabst gab gaben gabt gaben"), "gegeben", "haben"),
    "nehmen": ("take", six("nehme nimmst nimmt nehmen nehmt nehmen"), six("nahm nahmst nahm nahmen nahmt nahmen"), "genommen", "haben"),
    "wissen": ("know (a fact)", six("weiß weißt weiß wissen wisst wissen"), six("wusste wusstest wusste wussten wusstet wussten"), "gewusst", "haben"),
    "denken": ("think", six("denke denkst denkt denken denkt denken"), six("dachte dachtest dachte dachten dachtet dachten"), "gedacht", "haben"),
    "finden": ("find", six("finde findest findet finden findet finden"), six("fand fandest fand fanden fandet fanden"), "gefunden", "haben"),
    "sprechen": ("speak", six("spreche sprichst spricht sprechen sprecht sprechen"), six("sprach sprachst sprach sprachen spracht sprachen"), "gesprochen", "haben"),
    "essen": ("eat", six("esse isst isst essen esst essen"), six("aß aßest aß aßen aßt aßen"), "gegessen", "haben"),
    "trinken": ("drink", six("trinke trinkst trinkt trinken trinkt trinken"), six("trank trankst trank tranken trankt tranken"), "getrunken", "haben"),
    "schlafen": ("sleep", six("schlafe schläfst schläft schlafen schlaft schlafen"), six("schlief schliefst schlief schliefen schlieft schliefen"), "geschlafen", "haben"),
    "fahren": ("travel / drive", six("fahre fährst fährt fahren fahrt fahren"), six("fuhr fuhrst fuhr fuhren fuhrt fuhren"), "gefahren", "sein"),
    "lesen": ("read", six("lese liest liest lesen lest lesen"), six("las lasest las lasen last lasen"), "gelesen", "haben"),
    "schreiben": ("write", six("schreibe schreibst schreibt schreiben schreibt schreiben"), six("schrieb schriebst schrieb schrieben schriebt schrieben"), "geschrieben", "haben"),
    "bleiben": ("stay", six("bleibe bleibst bleibt bleiben bleibt bleiben"), six("blieb bliebst blieb blieben bliebt blieben"), "geblieben", "sein"),
    "laufen": ("run / walk", six("laufe läufst läuft laufen lauft laufen"), six("lief liefst lief liefen lieft liefen"), "gelaufen", "sein"),
    "arbeiten": ("work", six("arbeite arbeitest arbeitet arbeiten arbeitet arbeiten"), six("arbeitete arbeitetest arbeitete arbeiteten arbeitetet arbeiteten"), "gearbeitet", "haben"),
    "kaufen": ("buy", six("kaufe kaufst kauft kaufen kauft kaufen"), six("kaufte kauftest kaufte kauften kauftet kauften"), "gekauft", "haben"),
    "spielen": ("play", six("spiele spielst spielt spielen spielt spielen"), six("spielte spieltest spielte spielten spieltet spielten"), "gespielt", "haben"),
    "fragen": ("ask", six("frage fragst fragt fragen fragt fragen"), six("fragte fragtest fragte fragten fragtet fragten"), "gefragt", "haben"),
    "lernen": ("learn", six("lerne lernst lernt lernen lernt lernen"), six("lernte lerntest lernte lernten lerntet lernten"), "gelernt", "haben"),
    "wohnen": ("live / reside", six("wohne wohnst wohnt wohnen wohnt wohnen"), six("wohnte wohntest wohnte wohnten wohntet wohnten"), "gewohnt", "haben"),
    "brauchen": ("need", six("brauche brauchst braucht brauchen braucht brauchen"), six("brauchte brauchtest brauchte brauchten brauchtet brauchten"), "gebraucht", "haben"),
    "hören": ("hear", six("höre hörst hört hören hört hören"), six("hörte hörtest hörte hörten hörtet hörten"), "gehört", "haben"),
    "holen": ("fetch", six("hole holst holt holen holt holen"), six("holte holtest holte holten holtet holten"), "geholt", "haben"),
    "rufen": ("call", six("rufe rufst ruft rufen ruft rufen"), six("rief riefst rief riefen rieft riefen"), "gerufen", "haben"),
    "bringen": ("bring", six("bringe bringst bringt bringen bringt bringen"), six("brachte brachtest brachte brachten brachtet brachten"), "gebracht", "haben"),
    "stehen": ("stand", six("stehe stehst steht stehen steht stehen"), six("stand standest stand standen standet standen"), "gestanden", "haben"),
    "steigen": ("climb / rise", six("steige steigst steigt steigen steigt steigen"), six("stieg stiegst stieg stiegen stiegt stiegen"), "gestiegen", "sein"),
    "fangen": ("catch", six("fange fängst fängt fangen fangt fangen"), six("fing fingst fing fingen fingt fingen"), "gefangen", "haben"),
    "laden": ("load", six("lade lädst lädt laden ladet laden"), six("lud ludest lud luden ludet luden"), "geladen", "haben"),
    "räumen": ("clear / tidy", six("räume räumst räumt räumen räumt räumen"), six("räumte räumtest räumte räumten räumtet räumten"), "geräumt", "haben"),
    "können": ("be able to", six("kann kannst kann können könnt können"), six("konnte konntest konnte konnten konntet konnten"), "gekonnt", "haben"),
    "müssen": ("have to", six("muss musst muss müssen müsst müssen"), six("musste musstest musste mussten musstet mussten"), "gemusst", "haben"),
    "wollen": ("want to", six("will willst will wollen wollt wollen"), six("wollte wolltest wollte wollten wolltet wollten"), "gewollt", "haben"),
}

# Infinitive, English meaning, base verb, prefix, perfect auxiliary override.
separable = [
    ("aufstehen", "get up", "stehen", "auf", "sein"),
    ("anrufen", "call (by phone)", "rufen", "an", None),
    ("einkaufen", "shop", "kaufen", "ein", None),
    ("mitkommen", "come along", "kommen", "mit", None),
    ("zurückkommen", "come back", "kommen", "zurück", None),
    ("anfangen", "begin", "fangen", "an", None),
    ("aufhören", "stop", "hören", "auf", None),
    ("fernsehen", "watch television", "sehen", "fern", None),
    ("mitbringen", "bring along", "bringen", "mit", None),
    ("abholen", "pick up", "holen", "ab", None),
    ("einladen", "invite", "laden", "ein", None),
    ("aufmachen", "open", "machen", "auf", None),
    ("zumachen", "close", "machen", "zu", None),
    ("ausgehen", "go out", "gehen", "aus", None),
    ("einsteigen", "get in / board", "steigen", "ein", None),
    ("aussteigen", "get out / alight", "steigen", "aus", None),
    ("mitnehmen", "take along", "nehmen", "mit", None),
    ("aufräumen", "tidy up", "räumen", "auf", None),
    ("zuhören", "listen", "hören", "zu", None),
    ("vorbeikommen", "come by", "kommen", "vorbei", None),
]
for infinitive, meaning, base, prefix, auxiliary in separable:
    _, present, past, participle, base_auxiliary = verbs[base]
    if not participle.startswith("ge"):
        raise ValueError(f"No regular separable participle for {base}")
    verbs[infinitive] = (
        meaning,
        [f"{form} {prefix}" for form in present],
        [f"{form} {prefix}" for form in past],
        prefix + participle,
        auxiliary or base_auxiliary,
    )

# Noun, English meaning, gender, genitive singular, accusative singular,
# dative singular, nominative plural, dative plural.
nouns = [
    ("Mann", "man", "m", "Mannes", "Mann", "Mann", "Männer", "Männern"),
    ("Vater", "father", "m", "Vaters", "Vater", "Vater", "Väter", "Vätern"),
    ("Bruder", "brother", "m", "Bruders", "Bruder", "Bruder", "Brüder", "Brüdern"),
    ("Freund", "friend (male)", "m", "Freundes", "Freund", "Freund", "Freunde", "Freunden"),
    ("Student", "student (male)", "m", "Studenten", "Studenten", "Studenten", "Studenten", "Studenten"),
    ("Junge", "boy", "m", "Jungen", "Jungen", "Jungen", "Jungen", "Jungen"),
    ("Hund", "dog", "m", "Hundes", "Hund", "Hund", "Hunde", "Hunden"),
    ("Tisch", "table", "m", "Tisches", "Tisch", "Tisch", "Tische", "Tischen"),
    ("Stuhl", "chair", "m", "Stuhls", "Stuhl", "Stuhl", "Stühle", "Stühlen"),
    ("Baum", "tree", "m", "Baumes", "Baum", "Baum", "Bäume", "Bäumen"),
    ("Park", "park", "m", "Parks", "Park", "Park", "Parks", "Parks"),
    ("Zug", "train", "m", "Zuges", "Zug", "Zug", "Züge", "Zügen"),
    ("Tag", "day", "m", "Tages", "Tag", "Tag", "Tage", "Tagen"),
    ("Schlüssel", "key", "m", "Schlüssels", "Schlüssel", "Schlüssel", "Schlüssel", "Schlüsseln"),
    ("Apfel", "apple", "m", "Apfels", "Apfel", "Apfel", "Äpfel", "Äpfeln"),
    ("Frau", "woman", "f", "Frau", "Frau", "Frau", "Frauen", "Frauen"),
    ("Mutter", "mother", "f", "Mutter", "Mutter", "Mutter", "Mütter", "Müttern"),
    ("Schwester", "sister", "f", "Schwester", "Schwester", "Schwester", "Schwestern", "Schwestern"),
    ("Freundin", "friend (female)", "f", "Freundin", "Freundin", "Freundin", "Freundinnen", "Freundinnen"),
    ("Stadt", "city", "f", "Stadt", "Stadt", "Stadt", "Städte", "Städten"),
    ("Straße", "street", "f", "Straße", "Straße", "Straße", "Straßen", "Straßen"),
    ("Schule", "school", "f", "Schule", "Schule", "Schule", "Schulen", "Schulen"),
    ("Tür", "door", "f", "Tür", "Tür", "Tür", "Türen", "Türen"),
    ("Hand", "hand", "f", "Hand", "Hand", "Hand", "Hände", "Händen"),
    ("Zeit", "time", "f", "Zeit", "Zeit", "Zeit", "Zeiten", "Zeiten"),
    ("Nacht", "night", "f", "Nacht", "Nacht", "Nacht", "Nächte", "Nächten"),
    ("Wohnung", "flat / apartment", "f", "Wohnung", "Wohnung", "Wohnung", "Wohnungen", "Wohnungen"),
    ("Tasche", "bag", "f", "Tasche", "Tasche", "Tasche", "Taschen", "Taschen"),
    ("Kind", "child", "n", "Kindes", "Kind", "Kind", "Kinder", "Kindern"),
    ("Haus", "house", "n", "Hauses", "Haus", "Haus", "Häuser", "Häusern"),
    ("Buch", "book", "n", "Buches", "Buch", "Buch", "Bücher", "Büchern"),
    ("Auto", "car", "n", "Autos", "Auto", "Auto", "Autos", "Autos"),
    ("Zimmer", "room", "n", "Zimmers", "Zimmer", "Zimmer", "Zimmer", "Zimmern"),
    ("Fenster", "window", "n", "Fensters", "Fenster", "Fenster", "Fenster", "Fenstern"),
    ("Bett", "bed", "n", "Bettes", "Bett", "Bett", "Betten", "Betten"),
    ("Bild", "picture", "n", "Bildes", "Bild", "Bild", "Bilder", "Bildern"),
    ("Jahr", "year", "n", "Jahres", "Jahr", "Jahr", "Jahre", "Jahren"),
    ("Wort", "word", "n", "Wortes", "Wort", "Wort", "Wörter", "Wörtern"),
    ("Mädchen", "girl", "n", "Mädchens", "Mädchen", "Mädchen", "Mädchen", "Mädchen"),
    ("Telefon", "telephone", "n", "Telefons", "Telefon", "Telefon", "Telefone", "Telefonen"),
]

# Base adjective, English meaning, optional stem before an ending. The shared
# weak/mixed/strong ending tables are implemented in adjectivedata.cpp.
adjectives = [
    ("gut", "good", None),
    ("schlecht", "bad", None),
    ("klein", "small", None),
    ("groß", "big / tall", None),
    ("alt", "old", None),
    ("neu", "new", None),
    ("jung", "young", None),
    ("schön", "beautiful", None),
    ("schnell", "fast", None),
    ("langsam", "slow", None),
    ("lang", "long", None),
    ("kurz", "short", None),
    ("warm", "warm", None),
    ("kalt", "cold", None),
    ("heiß", "hot", None),
    ("leicht", "easy / light", None),
    ("schwer", "difficult / heavy", None),
    ("stark", "strong", None),
    ("schwach", "weak", None),
    ("hell", "bright / light", None),
    ("dunkel", "dark", "dunkl"),
    ("sauber", "clean", None),
    ("schmutzig", "dirty", None),
    ("interessant", "interesting", None),
    ("wichtig", "important", None),
    ("richtig", "correct", None),
    ("falsch", "wrong", None),
    ("freundlich", "friendly", None),
    ("ruhig", "calm / quiet", None),
    ("teuer", "expensive", "teur"),
    ("billig", "cheap", None),
    ("hoch", "high", "hoh"),
    ("tief", "deep / low", None),
    ("rot", "red", None),
    ("blau", "blue", None),
    ("grün", "green", None),
    ("weiß", "white", None),
    ("schwarz", "black", None),
]

# Key, spelling, meaning/context, governed case. Two-way senses are distinct.
prepositions = [
    ("aus", "aus", "out of / from", "dative"),
    ("bei", "bei", "at / near", "dative"),
    ("mit", "mit", "with", "dative"),
    ("nach", "nach", "after", "dative"),
    ("seit", "seit", "since / for (time)", "dative"),
    ("von", "von", "from / of", "dative"),
    ("zu", "zu", "to", "dative"),
    ("gegenüber", "gegenüber", "opposite", "dative"),
    ("außer", "außer", "except for", "dative"),
    ("durch", "durch", "through", "accusative"),
    ("für", "für", "for", "accusative"),
    ("gegen", "gegen", "against", "accusative"),
    ("ohne", "ohne", "without", "accusative"),
    ("um", "um", "around", "accusative"),
    ("wegen", "wegen", "because of (standard written German)", "genitive"),
    ("während", "während", "during", "genitive"),
    ("trotz", "trotz", "despite", "genitive"),
    ("statt", "statt", "instead of", "genitive"),
    ("innerhalb", "innerhalb", "inside / within", "genitive"),
    ("außerhalb", "außerhalb", "outside", "genitive"),
]
for spelling, location, destination in [
    ("an", "at / against (location)", "to / against (destination)"),
    ("auf", "on (location)", "onto (destination)"),
    ("hinter", "behind (location)", "to behind (destination)"),
    ("in", "in (location)", "into (destination)"),
    ("neben", "beside (location)", "to beside (destination)"),
    ("über", "above (location)", "to above (destination)"),
    ("unter", "under (location)", "to under (destination)"),
    ("vor", "in front of (location)", "to in front of (destination)"),
    ("zwischen", "between (location)", "to between (destination)"),
]:
    prepositions.append((spelling + "-place", spelling, location, "dative"))
    prepositions.append((spelling + "-to", spelling, destination, "accusative"))

# Pronoun rows are stored alongside the other German vocabulary. The four
# personal genitives are uncommon in conversation, but belong to the full
# paradigm. Each possessive has both before-noun and standalone forms.
# id, lemma, English meaning, nominative, accusative, dative, genitive
personal_pronouns = [
    ("personal-ich", "ich", "I (first person singular)", "ich", "mich", "mir", "meiner"),
    ("personal-du", "du", "you (informal singular)", "du", "dich", "dir", "deiner"),
    ("personal-er", "er", "he / it (masculine)", "er", "ihn", "ihm", "seiner"),
    ("personal-sie-f", "sie", "she / it (feminine)", "sie", "sie", "ihr", "ihrer"),
    ("personal-es", "es", "it (neuter)", "es", "es", "ihm", "seiner"),
    ("personal-wir", "wir", "we", "wir", "uns", "uns", "unser"),
    ("personal-ihr", "ihr", "you (informal plural)", "ihr", "euch", "euch", "euer"),
    ("personal-sie-pl", "sie", "they", "sie", "sie", "ihnen", "ihrer"),
    ("personal-Sie", "Sie", "you (formal singular or plural)", "Sie", "Sie", "Ihnen", "Ihrer"),
]

# id, lemma, English meaning, suffixed stem, unsuffixed form.
possessive_pronouns = [
    ("possessive-ich", "mein", "my / mine (owner: I)", "mein", "mein"),
    ("possessive-du", "dein", "your / yours (owner: informal singular you)", "dein", "dein"),
    ("possessive-er", "sein", "his (owner: he)", "sein", "sein"),
    ("possessive-sie-f", "ihr", "her / hers (owner: she)", "ihr", "ihr"),
    ("possessive-es", "sein", "its (owner: it)", "sein", "sein"),
    ("possessive-wir", "unser", "our / ours (owner: we)", "unser", "unser"),
    ("possessive-ihr", "euer", "your / yours (owner: informal plural you)", "eur", "euer"),
    ("possessive-sie-pl", "ihr", "their / theirs (owner: they)", "ihr", "ihr"),
    ("possessive-Sie", "Ihr", "your / yours (owner: formal you)", "Ihr", "Ihr"),
]

pronoun_cases = ("nominative", "accusative", "dative", "genitive")
referents = ("m", "f", "n", "pl")
possessive_endings = {
    "m": {"nominative": "er", "accusative": "en", "dative": "em", "genitive": "es"},
    "f": {"nominative": "e", "accusative": "e", "dative": "er", "genitive": "er"},
    "n": {"nominative": "s", "accusative": "s", "dative": "em", "genitive": "es"},
    "pl": {"nominative": "e", "accusative": "e", "dative": "en", "genitive": "er"},
}


def possessive_form(stem, unsuffixed, use, referent, case):
    if use == "before" and referent in ("m", "n") and case == "nominative":
        return unsuffixed
    if use == "before" and referent == "n" and case == "accusative":
        return unsuffixed
    if use == "alone" and referent == "n" and case in ("nominative", "accusative"):
        # Short and full forms are both accepted where both are established.
        if stem in ("mein", "dein", "sein"):
            return stem + "s|" + stem + "es"
        if stem in ("ihr", "Ihr"):
            return stem + "s|" + stem + "es"
        if stem == "unser":
            return "unseres"
        if stem == "eur":
            return "eures"
    return stem + possessive_endings[referent][case]


def quote(value):
    return '"' + value.replace('"', '\\"') + '"'


def mapping(lines, field, pairs):
    lines.append(f"    {field}:")
    for key, value in pairs:
        lines.append(f"      {key}: {quote(value)}")


def generate():
    lines = ["  German:", "    # Complete stored forms; answers omit the subject pronoun.", "    Verbs:"]
    lines.extend(f"      - {verb}" for verb in verbs)
    mapping(lines, "Verbs-en-inf", ((verb, data[0]) for verb, data in verbs.items()))
    subject_index = {
        "1psing": 0, "1ppl": 3, "2pfam": 1, "2pfampl": 4,
        "2ppol": 5, "2ppolpl": 5, "3psing": 2, "3ppl": 5,
    }
    auxiliary_forms = {
        "haben": six("habe hast hat haben habt haben"),
        "sein": six("bin bist ist sind seid sind"),
    }
    future_auxiliary = six("werde wirst wird werden werdet werden")
    for tense in ("present", "future", "simple_past", "pres_perf"):
        for subject, index in subject_index.items():
            forms = []
            for verb, (_, present, past, participle, auxiliary) in verbs.items():
                form = {
                    "present": present[index],
                    "future": f"{future_auxiliary[index]} {verb}",
                    "simple_past": past[index],
                    "pres_perf": f"{auxiliary_forms[auxiliary][index]} {participle}",
                }[tense]
                forms.append((verb, form))
            mapping(lines, f"Verbs-{tense}-{subject}", forms)

    lines.extend(["    # German answers contain the definite article and noun.", "    Nouns:"])
    lines.extend(f"      - {noun[0]}" for noun in nouns)
    mapping(lines, "Nouns-en", ((noun, english) for noun, english, *_ in nouns))
    mapping(lines, "Nouns-gender", ((noun, gender) for noun, _, gender, *_ in nouns))
    articles = {
        "singular": {
            "nominative": {"m": "der", "f": "die", "n": "das"},
            "accusative": {"m": "den", "f": "die", "n": "das"},
            "dative": {"m": "dem", "f": "der", "n": "dem"},
            "genitive": {"m": "des", "f": "der", "n": "des"},
        },
        "plural": {"nominative": "die", "accusative": "die", "dative": "den", "genitive": "der"},
    }
    for number in ("singular", "plural"):
        for case in ("nominative", "accusative", "dative", "genitive"):
            pairs = []
            for noun, _, gender, gen_sg, acc_sg, dat_sg, plural, dat_pl in nouns:
                if number == "singular":
                    stem = {"nominative": noun, "accusative": acc_sg,
                            "dative": dat_sg, "genitive": gen_sg}[case]
                    article = articles[number][case][gender]
                else:
                    stem = dat_pl if case == "dative" else plural
                    article = articles[number][case]
                pairs.append((noun, f"{article} {stem}"))
            mapping(lines, f"Nouns-{number}-{case}", pairs)

    lines.extend(["    # Attributive adjective bases; uncommon stem contractions are explicit.",
                  "    Adjectives:"])
    lines.extend(f"      - {adjective}" for adjective, _, _ in adjectives)
    mapping(lines, "Adjectives-en", ((adjective, meaning)
                                      for adjective, meaning, _ in adjectives))
    mapping(lines, "Adjectives-stem", ((adjective, stem)
                                        for adjective, _, stem in adjectives if stem))

    lines.extend(["    # Two-way prepositions have separate location and destination cards.", "    Prepositions:"])
    lines.extend(f"      - {key}" for key, *_ in prepositions)
    mapping(lines, "Prepositions-de", ((key, spelling) for key, spelling, *_ in prepositions))
    mapping(lines, "Prepositions-en", ((key, meaning) for key, _, meaning, _ in prepositions))
    mapping(lines, "Prepositions-case", ((key, case) for key, _, _, case in prepositions))

    lines.extend(["    # Personal and possessive paradigms. A pipe separates accepted variants.",
                  "    Pronouns:"])
    lines.extend(f"      - {entry[0]}" for entry in personal_pronouns + possessive_pronouns)
    mapping(lines, "Pronouns-kind", ((entry[0], "personal") for entry in personal_pronouns))
    lines.extend(f"      {entry[0]}: {quote('possessive')}" for entry in possessive_pronouns)
    mapping(lines, "Pronouns-lemma", ((entry[0], entry[1]) for entry in personal_pronouns + possessive_pronouns))
    mapping(lines, "Pronouns-en", ((entry[0], entry[2]) for entry in personal_pronouns + possessive_pronouns))
    for case_index, case in enumerate(pronoun_cases, start=3):
        mapping(lines, f"Pronouns-personal-{case}",
                ((entry[0], entry[case_index]) for entry in personal_pronouns))
    for use in ("before", "alone"):
        for referent in referents:
            for case in pronoun_cases:
                mapping(lines, f"Pronouns-{use}-{referent}-{case}",
                        ((entry[0], possessive_form(entry[3], entry[4], use, referent, case))
                         for entry in possessive_pronouns))
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    path = Path(__file__).resolve().parents[1] / "language.yaml"
    text = path.read_text(encoding="utf-8")
    start = text.index("  German:\n")
    end = text.index("  Russian:\n", start)
    path.write_text(text[:start] + generate() + text[end:], encoding="utf-8")
    print(f"Wrote {len(verbs)} verbs ({len(separable)} separable), "
          f"{len(nouns)} nouns, {len(adjectives)} adjectives, "
          f"{len(prepositions)} preposition meanings, and "
          f"{len(personal_pronouns) + len(possessive_pronouns)} pronoun bases.")
