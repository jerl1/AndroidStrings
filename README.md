AndroidStrings
==============

This project is for exporting (in CSV format) all strings used in an Android source repository.
It look for all strings into ressources folder, it does not include the string inside the source code.

This project uses Qt 5.

The usage are very simple, three text boxes:
- Sources: for the root folder of your Android repository
- Exclude: for excluding all ressources from this start point
- Overlay: for adding the overlay of your device ressources files (will replace the one found in other ressources files)

Exclude and overlay are very usefull if you have multiple device into your sources codes repository
