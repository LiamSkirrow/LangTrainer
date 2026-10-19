# LangTrainer Qt app

Open `qt-placeholder/CMakeLists.txt` in Qt Creator, select a Qt 5 or Qt 6 Desktop kit, then build and run.

The home window keeps the language and practice mode choices together. Spanish currently has Verb conjugation, Sentence practice, and Listening and speaking rows; its modes are built in a separate function so other languages can define their own rows. Select Spanish, then choose **Verb conjugation → Choose verbs...** to open a searchable checklist. Highlight a verb and use **View forms...** at the bottom to see every stored conjugation for reference. Choose one or more verbs and select **Next: tenses** to open a matching checklist for tenses. Both windows have search, **Select all**, **Clear all**, and a selected count. Selections are remembered for later sessions. Select at least one verb and one tense to start.

The exercise asks for conjugated forms across the selected verbs and tenses, using the subjects listed in `spec.yaml`. Type a form and choose **Check answer**, or choose **Show answer** to skip. Answers must include the correct accents: `hablaría` is accepted, while `hablaria` is not. The small buttons beneath the answer field insert á, é, í, ó, ú, ü, and ñ at the cursor. Feedback shows the correct spelling. It tracks correct attempts and skips until you choose **Finish**. The other practice modes remain placeholders.

Spanish verb data comes from the top-level `language.yaml` and `spec.yaml`, bundled as Qt resources during the build. Rebuild after editing either YAML file. A lightweight reader handles the current Spanish verb fields; if the YAML structure changes, update `verbdata.cpp` too. Run `LangTrainer --check-data` to validate that every listed Spanish verb has a meaning and a form for every configured tense and subject.
