#!/bin/bash
mkdir -p mp3/{01,02}
find ./mp3 -name "*.mp3" -delete
if ! (command -v lame &> /dev/null && command -v sox &> /dev/null && command -v lame &> /dev/null && command -v mp3gain &>/dev/null) ; then
  sudo apt-get install -y libttspico-utils lame sox mp3gain
fi
for i in {0..9}; do
  pico2wave --lang=de-DE -w=mp3/02/00${i}_ziffer.wav "$i"
done
pico2wave --lang=de-DE -w=mp3/01/001_start.wav "Hallo, Herzlich willkommen, bitte gib Deine Zahlen von der letzten Station ein!"
sox -n -r 8000 -b 16 mp3/01/002_freizeichen.wav synth 30 sine 425
pico2wave --lang=de-DE -w=mp3/01/003_ok.wav "Herzlichen Glückwunsch, Deine Geheimzahl für die nächste Station lautet:"
pico2wave --lang=de-DE -w=mp3/01/004_fehler.wav "Das war leider falsch! Wenn Du es nochmal probieren willst, mußt Du den Hörer auflegen und dann wieder abnehmen."
pico2wave --lang=de-DE -w=mp3/01/005_ende.wav "Vielen Dank, ich hoffe Du hattest Spaß."
pico2wave --lang=de-DE -w=mp3/01/006_deine_Zahlen_waren.wav "Deine Zahlen waren:"
pico2wave --lang=de-DE -w=mp3/01/007_bitte_auflegen.wav "Bitte lege den Hörer auf!"
find ./mp3 -name "*.wav" -exec bash -c 'lame --resample 22.05 -m m -b 64 "$1" "${1%.wav}.mp3"' _ {} \;
find ./mp3 -name "*.mp3" -exec mp3gain -r -k {} +
find ./mp3 -name "*.wav" -delete
