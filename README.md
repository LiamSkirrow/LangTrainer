# LangTrainer

A C++/QT app to train various aspects related to declension and conjugation in German... or any language for that matter:

- Generate sentences (from a user-supplied set of verbs+nouns+adjectives in all forms, with all endings) with a specific word removed (or multiple words removed?) with the intention of the user having to fill in the right word form. The missing words from the sentence are indicated somewhere on screen in their infinitive/nominative form and the user has to fill in the correctly conjugated/declined verb/noun/adjective, at which point the app will display whether this is correct or not. 
- The general idea is that you do this repeatedly over and over, allowing you to drill which endings apply in a given sentence. The possible list of words that can be drilled should be entered in manually (supplying the correct endings manually) or scraped from some source (wiktionary maybe?).
- Might be nice to have the ability to generate anki flash card decks from the supplied words, with the different endings on the other side of the card
- The app shouldn’t be German specific, you should be able to create separate decks for any language, nice and generic 
- Have a setting to control the complexity of the sentences (is it just SVO, or is it multiple clauses connected together etc)
- Have a setting to include multiple tenses 
- It might also be fun to track the correct/incorrect answers in a given session/day, the average time taken per card etc. Have a window that displays these statistics over time.
- In order to include functionality for multiple languages, it should be possible to specify the form of sentences within the language via some serialised format -> yaml, json etc or maybe some custom (and non-technical) format. Simply specify the SVO order and other relevant parts of the sentence structure for that language so that sentences can then be generated once a user-supplied set of vocabulary is given.

The ultimate goal is to have a tool that can be used to generate random (but also grammatically correct) sentences that exercise all different conjugation and case endings for a particular set of supplied verbs/adjectives/nouns. The user can open up the app and keep attempting the exercises over and over until they master the different endings.

# TODO
- Figure out whether this should be a type of spaced repetition app. Should the user simply open up the app and start doing the exercises until they feel they understand things, or rather should the app have a set number of cards per day that the user needs to complete ala Anki? I think SRS is probably the way to go.
- Figure out the algorithm first. How many cards will appear for each entered word? Simply produce new pseduo-randomly generated card sentences until all user-supplied word forms are covered?
  - Should the pseduo-randomly generated sentences actually be saved? So that the *same* card pops up everytime? (I think this is probably the best way to do it)

## Features
- Have a 'free practice' mode alongside the SRS mode, in which no previously covered cards pop up -> every single card is pseduo-randomly generated at the point where the user progresses to this next card. There should be no memory of previous answers or spaced repetition debt, simply generate a new card every time. This allows for the user to continually drill grammar exercises for their supplied input words as many times as they like. The user should be able to select which grammar types they want to be included in the free practice cards -> which tenses, which cases, which conjugations etc... Totally configurable
- Have the ability to backtrack by one card, if the user hits enter and gets it wrong but accidentally hits enter again then they falsely advance to the next card and don't get to see the solution.
- Advanced feature: On top of the ability to pseudo-randomly generate sentences, have the ability to use actual text (Harry Potter, news articles, other well-known novels etc...) as these sentence sources, somehow profiling the sentences from the text and splicing out specific words which must then be filled in by the user. It's hard to think how I could do this automatically without having to do enter in these sentences manually though...
- SRS implementation: https://github.com/open-spaced-repetition

## Definition of First Milestone
The first 'checkpoint' shall be as follows:
The app is opened and a main screen is presented. The main screen shows the available decks, the user clicks on one and a new screen pops up with some buttons: 'Settings', 'Begin', 'Add word'.
  - The settings button presents some configuration options (TBD)
  - The 'add word' button allows for a new word to be added to the deck. The user selects whether it's a verb/noun/adjective/preposition etc and must fill in the different endings themselves. (It might be a nice feature to auto-fill the endings by making some API call to a dictionary/Wiktionary but this is a future nice-to-have feature)
  - The 'Begin' button starts the revision/drills for that day. For the first milestone, only free practice mode is implemented. SRS shall be included in a future feature release. Additionally, only one difficulty setting is to be implemented, simply producing sentences of SVO order with not much variability or complexity.
Upon clicking 'Begin', the user-supplied words are used to pseudo-randomly generate sentences, with one of the words (randomly) chosen to be suspended, and the infinitive verb/nominative noun/unmodified adjective is shown somewhere onscreen. The user must type in a box the modified version of the word, hit enter, and see whether this is the correct answer. If it's correct, move on to the next card, if it's wrong stay on this card and show the correct answer, moving onto the next card only when the user hits enter again.
This shall demonstrate the basic working order of the app, with additional feature releases making their way in as I progress forward and start to scope out the app.

## Definition of v1.0.0 Release
I shall continue creating minor release versions (v0.x.x) until eventually declaring v1.0.0, which shall represent the first release that has all intended functionality and is 'complete' enough to be used by others. Perhaps at this point I'll start trying to get people to use it.

### Note on Donation and Funding
The reason why I have a donation button isn't just a cash-grab, there really are running costs associated with this project, which are:
- Plans for developing an iOS app with the same functionality as the desktop app, which requires an iOS developers license (USD$99/year)
- Server to sync user data between desktop and mobile apps, so that both can be used together and also stay in sync -> annual running costs unknown
- Website hosting fees -> annual running costs unknown

<a href="https://www.buymeacoffee.com/liamskirrow" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>

