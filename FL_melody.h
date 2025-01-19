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

#ifndef _FL_MELODY_H_
#define _FL_MELODY_H_

#include <Arduino.h>

// **************************************************************************************************************
// NOTE frequencies *********************************************************************************************
// **************************************************************************************************************
#define NOTE_REST 0
#define NOTE_B0   31
#define NOTE_C1   33
#define NOTE_CS1  35
#define NOTE_D1   37
#define NOTE_DS1  39
#define NOTE_E1   41
#define NOTE_F1   44
#define NOTE_FS1  46
#define NOTE_G1   49
#define NOTE_GS1  52
#define NOTE_A1   55
#define NOTE_AS1  58
#define NOTE_B1   62
#define NOTE_C2   65
#define NOTE_CS2  69
#define NOTE_D2   73
#define NOTE_DS2  78
#define NOTE_E2   82
#define NOTE_F2   87
#define NOTE_FS2  93
#define NOTE_G2   98
#define NOTE_GS2  104
#define NOTE_A2   110
#define NOTE_AS2  117
#define NOTE_B2   123
#define NOTE_C3   131
#define NOTE_CS3  139
#define NOTE_D3   147
#define NOTE_DS3  156
#define NOTE_E3   165
#define NOTE_F3   175
#define NOTE_FS3  185
#define NOTE_G3   196
#define NOTE_GS3  208
#define NOTE_A3   220
#define NOTE_AS3  233
#define NOTE_B3   247
#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_CS5  554
#define NOTE_D5   587
#define NOTE_DS5  622
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_GS5  831
#define NOTE_A5   880
#define NOTE_AS5  932
#define NOTE_B5   988
#define NOTE_C6   1047
#define NOTE_CS6  1109
#define NOTE_D6   1175
#define NOTE_DS6  1245
#define NOTE_E6   1319
#define NOTE_F6   1397
#define NOTE_FS6  1480
#define NOTE_G6   1568
#define NOTE_GS6  1661
#define NOTE_A6   1760
#define NOTE_AS6  1865
#define NOTE_B6   1976
#define NOTE_C7   2093
#define NOTE_CS7  2217
#define NOTE_D7   2349
#define NOTE_DS7  2489
#define NOTE_E7   2637
#define NOTE_F7   2794
#define NOTE_FS7  2960
#define NOTE_G7   3136
#define NOTE_GS7  3322
#define NOTE_A7   3520
#define NOTE_AS7  3729
#define NOTE_B7   3951
#define NOTE_C8   4186
#define NOTE_CS8  4435
#define NOTE_D8   4699
#define NOTE_DS8  4978

// Melody Tempo(BPM) whole note = 60 / BPM * 4 * 1000 [ms]
#define BPM120    2000
#define BPM240    1000
#define BPM360    667
#define BPM480    500
#define BPM600    400
#define BPM720    333


// **************************************************************************************************************
// Melody Data **************************************************************************************************
// **************************************************************************************************************
// メロディ構造体
struct Melody {
  const int (*notes)[2];  // 音符データへのポインタ
  int noteCount;          // 音符の総数
  int baseDuration;       // 1分音符(BPM*4)の基本時間[ms]
};

// メロディデータ
// {{note, duration}... } duration: 4 = quarter note, 8 = eighth note,,,
// Melody1
const int melody1Notes[][2] = {
  {NOTE_C4, 4}, {NOTE_G3, 8}, {NOTE_G3, 8}, {NOTE_A3, 4},
  {NOTE_G3, 4}, {NOTE_REST, 4}, {NOTE_B3, 4}, {NOTE_C4, 4}
};
const Melody melody1 = {melody1Notes, sizeof(melody1Notes) / sizeof(melody1Notes[0]), BPM360};

// Melody2
const int melody2Notes[][2] = {
  {NOTE_C4, 16}
};
const Melody melody2 = {melody2Notes, sizeof(melody2Notes) / sizeof(melody2Notes[0]), BPM480};

// Melody3
const int melody3Notes[][2] = {
  {NOTE_C5, 16}
};
const Melody melody3 = {melody3Notes, sizeof(melody3Notes) / sizeof(melody3Notes[0]), BPM480};

// Melody4
const int melody4Notes[][2] = {
  {NOTE_C6, 16}, {NOTE_E6, 16}, {NOTE_G6, 16}
};
const Melody melody4 = {melody4Notes, sizeof(melody4Notes) / sizeof(melody4Notes[0]), BPM480};

// **************************************************************************************************************
// Class MelodyPlayer *******************************************************************************************
// **************************************************************************************************************
class MelodyPlayer {
private:
  const int pin;           // ピエゾのピン番号
  const Melody* melody;    // 現在のメロディへのポインタ
  int currentNote;         // 現在再生中の音符インデックス
  unsigned long noteStartTime; // 現在の音符が再生開始された時刻
  unsigned long noteDuration;  // 現在の音符の長さ
  bool isPlaying;          // 再生中かどうか
  const Melody* newMelody; // 次の曲用メロディ

public:
  MelodyPlayer(int pin);
  void start(const Melody* melody);  // 再生開始
  void stop();                       // 再生停止
  void update();                     // メインループ内で繰り返し呼び出し
  void setMelody(const Melody* newMelody); // 曲の変更
  bool isFinished();                 // 再生終了を確認
};

#endif