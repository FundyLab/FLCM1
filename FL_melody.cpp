/*
  The MIT License (MIT)

  Copyright (c) 2025 FundyLab

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "FL_melody.h"

// **************************************************************************************************************
// Class MelodyPlayer *******************************************************************************************
// **************************************************************************************************************
MelodyPlayer::MelodyPlayer(int pin)
    : pin(pin), melody(nullptr), currentNote(0), 
      isPlaying(false), newMelody(nullptr) {}

void MelodyPlayer::start(const Melody* melody) {
  this->melody = melody;

  currentNote = 0;
  isPlaying = true;
  noteStartTime = millis();
  noteDuration = melody->baseDuration / melody->notes[currentNote][1]; // 音符の長さを計算
  tone(pin, melody->notes[currentNote][0], noteDuration);
}

void MelodyPlayer::stop() {
  noTone(pin);
  isPlaying = false;
}

void MelodyPlayer::update() {
  if (!isPlaying) return;

  // 新しい曲が設定されている場合は切り替え
  if (newMelody) {
    start(newMelody);
    newMelody = nullptr;
    return;
  }

  unsigned long currentTime = millis();
  if (currentTime - noteStartTime >= noteDuration * 1.25) {
    // 次の音符へ進む
    currentNote++;
    if (currentNote < melody->noteCount) {
      noteStartTime = currentTime;
      noteDuration = melody->baseDuration / melody->notes[currentNote][1];
      tone(pin, melody->notes[currentNote][0], noteDuration);
    } else {
      // 再生終了
      stop();
    }
  }
}

void MelodyPlayer::setMelody(const Melody* newMelody) {
  this->newMelody = newMelody;
}

bool MelodyPlayer::isFinished() {
  return !isPlaying;
}