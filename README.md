# LangTrainer

_Disclaimer: I'm reviving this project with Codex, a lot of code and UI work are AI generated_

## Overview
This program is a C++/QT app that can be used to drill verb conjugations, noun declensions, prepositions... basically anything that you may want to outright practice in a tight loop in order to improve at your target language. For example, if you're learning Spanish, you may want to test how well you know your verb conjugations for the most common verbs across various tenses. LangTrainer allows you to do this with a custom selection of verbs, which over time you'll eventually master. 

Note: this is not a spaced repetition program, as soon as anything tries to do spaced repetition, it just becomes a worse Anki. The only reason this program exists is because it *isn't* spaced repetition. Simply drill your verb conjugations to your heart's content.

### Develepment Thoughts
- Originally, I was planning on encoding the generic grammar structure of a sentence across the supported languages, however the value proposition of this has now declined given I can just Codex-generate arbitrarily many sentences, either with/without the user stored vocab. It just seems like extra effort to generate sentences using the existing vocab, constrained to a rigid precoded grammar template... *Need to think more about this*
  - I'm tempted to just enter in a couple dozen/hundred example sentences at various difficulty levels and just drill those instead.
- TTS usecase will probably be super easy to implement now, give this some more thought. Maybe Pimsleur-like question/command prompting like "say that blah blah", "ask whether they want to blah blah blah"
- Would also be nice to include some form of session tracking, being able to analyse the accuracy/response rate for specific verbs would be very nice. And maybe even tracking the accuracy for specific *tenses*. This would make it obvious if there was an area that needed focusing on.
