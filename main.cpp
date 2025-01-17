#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/*
TODO:
- Construct a very minimal main UI. Include a scrollable list of decks.
- Each deck must become highlighted when clicked on, meaning that it's currently selected.
- At the top are three buttons: 'Settings', 'New Word' and 'Begin Training'
    - Settings should adjust config for the selected deck
    - 'New Word' brings up a new UI window that allows for the word type (verb, noun, adjective), as well as some boxes that need
      filling in for the word endings. As different word types are selected, the boxes for the word endings will change accordingly
    - Two buttons at the bottom of this new UI window are 'Cancel' and 'Add Word'.
-

- Also include an 'About' button down the bottom that brings up version info, a link to the GitHub repo and a cheeky donate button :)

*/
