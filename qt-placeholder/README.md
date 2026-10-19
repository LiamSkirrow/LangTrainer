# LangTrainer Qt app

Open `qt-placeholder/CMakeLists.txt` in Qt Creator, select a Qt 5 or Qt 6 Desktop kit, then build and run.

The home window keeps the language and practice mode choices together. Select Spanish, then choose **Verb conjugation → Choose verbs...** to open a searchable checklist. Use **Select all** or **Clear all** for the full list, including verbs hidden by search. Your selection is remembered for the next session. At least one verb must be selected to start.

The exercise asks for conjugated forms across the subjects and indicative/conditional tenses listed in `spec.yaml`. Type a form and choose **Check answer**, or choose **Show answer** to skip. It tracks correct attempts and skips until you choose **Finish**. The other practice modes remain placeholders.

Spanish verb data comes from the top-level `language.yaml` and `spec.yaml`, bundled as Qt resources during the build. Rebuild after editing either YAML file. A lightweight reader handles the current Spanish verb fields; if the YAML structure changes, update `verbdata.cpp` too. Run `LangTrainer --check-data` to validate that every listed Spanish verb has a meaning and a form for every configured tense and subject.
