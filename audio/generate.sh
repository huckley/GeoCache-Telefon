#!/bin/bash
mkdir -p mp3/{01,02}
if ! (command -v lame &> /dev/null && command -v sox &> /dev/null) ; then
  sudo apt-get install -y libttspico-utils lame
fi
for i in {0..9}; do
  pico2wave --lang=de-DE -w=mp3/02/00${i}_ziffer.wav "$i"
done
pico2wave --lang=de-DE -w=mp3/01/001_start.wav "Hallo, Herzlich willkommen, bitte gib Deine Geheimzahlen ein!"
pico2wave --lang=de-DE -w=mp3/01/002_ok.wav "Herzlichen Glückwunsch, Deine Geheimzahl für diese Station lautet:"
pico2wave --lang=de-DE -w=mp3/01/003_fehler.wav "Das war leider falsch, bitte probiere es noch einmal, oder gehe zurück zur lezten Station!"
pico2wave --lang=de-DE -w=mp3/01/004_auflegen.wav "Bitte lege den Höhrer auf!"
find ./mp3 -name "*.wav" -exec bash -c 'lame -b 128 "$1" "${1%.wav}.mp3"' _ {} \;
find ./mp3 -name "*.wav" -delete