# LangTrainer Qt app

Open `qt-placeholder/CMakeLists.txt` in Qt Creator, select a Qt 5 or Qt 6 Desktop kit, then build and run.

The home window keeps language and practice mode choices together. Spanish has Verb conjugation, Sentence practice, and Listening and speaking. Russian has those same modes plus Noun declension. Verb conjugation is available for both languages; the other buttons remain disabled placeholders.

Choose **Verb conjugation → Choose verbs...** to open a searchable checklist. Highlight a verb and use **View forms...** at the bottom to see every stored conjugation. Choose one or more verbs and select **Next: tenses** to open a matching checklist for tenses. Both windows have search, **Select all**, **Clear all**, and a selected count. Selections are remembered separately for each language.

The exercise asks for forms across the selected verbs and tenses, using the subjects listed in `spec.yaml`. Russian currently has ten imperfective verbs with present, past, and compound future forms; past tense distinguishes the speaker's or subject's gender where needed. The Russian exercise includes an on-screen Cyrillic keyboard and a Space key. Spanish keeps its accent buttons. Type a form and choose **Check answer**, or choose **Show answer** to skip. Spelling must match the stored form, including Spanish accents and Russian `ё`. **Finish**, or **View summary** after the last card, opens a summary of incorrect answers, skipped cards, and unseen cards.

Verb data comes from the top-level `language.yaml` and `spec.yaml`, bundled as Qt resources during the build. Rebuild after editing either YAML file. A lightweight reader handles the current verb fields; if the YAML structure changes, update `verbdata.cpp` too. Run `LangTrainer --check-data` to validate that every listed Spanish and Russian verb has an English meaning and a form for every configured tense and subject.
