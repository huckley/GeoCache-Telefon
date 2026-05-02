#!/bin/bash
mkdir -p mp3/{01,02}
if [ ! -x /usr/bin/pico2wave ]; then
  sudo apt-get install libttspico-utils
fi
for i in {0..9}; do
  pico2wave --lang=de-DE -w=mp3/00${i}_ziffer.wav "$i"
done
pico2wave --lang=de-DE -w=mp3/01/001_start.wav "Hallo, Herzlich willkommen, bitte gib Deine Geheimzahlen ein!"
pico2wave --lang=de-DE -w=mp3/01/002_ok.wav "Herzlichen Glückwunsch, Deine Geheimzahl für diese Station lautet:"
pico2wave --lang=de-DE -w=mp3/01/003_fehler.wav "Das war leider falsch, bitte probiere es noch einmal, oder gehe zurück zur lezten Station!"
pico2wave --lang=de-DE -w=mp3/01/004_auflegen.wav "Bitte lege den Höhrer auf!"
