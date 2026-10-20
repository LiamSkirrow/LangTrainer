# LangTrainer Qt app

Open `qt-placeholder/CMakeLists.txt` in Qt Creator, select a Qt 5 or Qt 6 Desktop kit, then build and run.

The home window keeps language and practice mode choices together. Spanish has Verb conjugation, Sentence practice, and Listening and speaking. Russian has those modes plus Noun declension and Prepositions. Russian verb, noun, and preposition practice are available; Sentence practice and Listening and speaking remain disabled placeholders.

For verb practice, choose verbs, then tenses. Highlight a verb in the first checklist and use **View forms...** to see its stored conjugations. Russian currently has ten imperfective verbs with present, past, and compound future forms; past tense distinguishes gender where needed.

For Russian noun practice, choose nouns, then cases. Highlight a noun and use **View forms...** to see its six singular forms. Each card gives a noun and a case; type the declined noun. The starter deck contains ten nouns. For Prepositions, choose nouns, then preposition meanings. Each card shows the Russian preposition and an English meaning or context cue, but leaves the governed case for you to recall; answer with the declined noun only. The case is revealed with the answer. The deck has 36 everyday preposition meanings, including separate movement and location entries for **в**, **на**, **за**, and **под**, plus the different meanings of **с/со**. The selected nouns all begin with consonants, so **о** does not need the **об** variant. Preposition phrases are short morphology prompts; some noun combinations are unusual outside a full sentence.

The checklists have search, **Select all**, **Clear all**, and a selected count. Selections are remembered separately for each language and mode. Exercises have on-screen character buttons, a card queue count, answer feedback, and a session summary of incorrect, skipped, and unseen cards. Answers must use the stored spelling, including Spanish accents and Russian `ё`.

Data comes from the top-level `language.yaml` and `spec.yaml`, bundled as Qt resources during the build. Rebuild after editing either YAML file. A lightweight reader handles the current fields; if the YAML structure changes, update the data readers too. Run `LangTrainer --check-data` to validate the bundled Spanish and Russian verb data and the Russian noun and preposition data.
