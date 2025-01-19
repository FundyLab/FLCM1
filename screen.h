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

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <Arduino.h>
#include <vector>
#include <algorithm>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "debug.h"

// ***** Screen definitions
#define FLAG_DRAW       0
#define FLAG_ERASE      1
#define VALUEDIGITMAX   8
#define MUTABLEOBJMAX   8

// Screen Objects *********************************************************************************
// 画面に表示するオブジェクトの種類
enum eTextAlignType {
  ALIGN_LEFT,
  ALIGN_CENTER,
  ALIGN_RIGHT,
  TYPEMAX
};
// BOXオブジェクト情報を格納する構造体の定義
struct boxObjectInfo {
  int16_t boxPos[4];//x0,y0,x1,y1
  uint8_t textSize;
  eTextAlignType textAlignType;
};
struct boxObjectColor {
  uint16_t lineColor;
  uint16_t fillColor;
  uint16_t textColor;
  uint16_t backgroundColor;
};

// Buttonオブジェクト情報を格納する構造体の定義
struct buttonObjectInfo {
  int16_t buttonPos[2];  // dx,dy from origin x0,y0 of box object
  uint16_t r_line;        // radius for outside circle line
  uint16_t r_fill;        // radius for inside fill
};
struct buttonObjectColor {
  uint16_t lineColor;
  uint16_t fillColor;
  uint16_t backgroundColor;
};

// triangleオブジェクト情報を格納
struct triangleObjectInfo {
  int16_t trianglePos[6]; // x0,y0,x1,y1,x2,y2
  uint16_t lineColor;
  uint16_t fillColor;
  uint16_t backgroundColor;
};

// カーソルオブジェクト情報を格納
struct cursorObjectInfo{
  int16_t offsetPos[6];   // cursor triangle offset value x0,y0,x1,y1,x2,y2 from digit top left ref 
  int16_t valueDigitPosList[VALUEDIGITMAX * 2]; // digit top left refs x0,y0,x1,y1,,,,x7,y7
};
struct cursorObjectColor {
  uint16_t fillColor;
  uint16_t backgroundColor;
};

// textオブジェクト情報を格納する構造体の定義
struct textObjectInfo {
  int16_t textPos[2]; // x0,y0
  uint8_t textSize;
  uint16_t textColor;
  uint16_t backgroundColor;
};

// screen object class ********************************************************************************
// スクリーンオブジェクトクラス群の定義
class ScreenObject { // 基底クラス
public:
  virtual ~ScreenObject() {}
  virtual void draw(bool flag_drawErase) = 0;
  virtual bool isTextObject() const { return false; }  // 型確認メソッド
  virtual bool isBoxObject() const { return false; }  // 型確認用仮想メソッド
  virtual bool isListObject() const { return false; }  // 型確認用仮想メソッド
  virtual bool isButtonListObject() const { return false; }  // 型確認用仮想メソッド
  virtual bool isTriangleObject() const { return false; }  // 型確認用仮想メソッド
  virtual bool isCursorObject() const { return false; }  // 型確認用仮想メソッド
};

class TextObject : public ScreenObject {
private:
  Adafruit_ILI9341* tft_; // ILI9341 TFTディスプレイ
  textObjectInfo info_;   // 配置＆色情報
  String label_;

public:
  TextObject(Adafruit_ILI9341* tft, const textObjectInfo &info, const String& label);
  void draw(bool flag_drawErase) override;
  bool isTextObject() const override { return true; }  // 型確認メソッドをオーバーライド
};

class BoxObject : public ScreenObject {
private:
  Adafruit_ILI9341* tft_; // ILI9341 TFTディスプレイ
  boxObjectInfo info_;        // 配置情報
  boxObjectColor color_;
  String label_;
  const int16_t width, height; // box width,height
  int16_t tx = 0, ty = 0; // text origin

public:
  BoxObject(Adafruit_ILI9341* tft, const boxObjectInfo &info, const boxObjectColor &color, const String& label);
  void draw(bool flag_drawErase) override;
  void changeText(const String& label);
  bool isBoxObject() const override { return true; }  // 型確認メソッドをオーバーライド
};

class ListObject : public ScreenObject {
private:
  Adafruit_ILI9341* tft_; // ILI9341 TFTディスプレイ
  boxObjectInfo info_;        // 配置情報
  boxObjectColor color_;
  String label_;
  const int16_t width, height; // box width,height
  int16_t tx = 0, ty = 0; // text origin

public:
  ListObject(Adafruit_ILI9341* tft, const boxObjectInfo &info, const boxObjectColor &color, const String& label);
  void draw(bool flag_drawErase) override;
  bool isListObject() const override { return true; }  // 型確認メソッドをオーバーライド
};

class ButtonListObject : public ScreenObject {
private:
  Adafruit_ILI9341* tft_; // ILI9341 TFTディスプレイ
  boxObjectInfo info_;        // 配置情報
  boxObjectColor color_;
  buttonObjectInfo buttonInfo_; 
  buttonObjectColor buttonColor_;
  String label_;
  const int16_t width, height; // box width,height
  int16_t tx = 0, ty = 0; // text origin

public:
  ButtonListObject(Adafruit_ILI9341* tft, const boxObjectInfo& info, const boxObjectColor& color, 
                    const buttonObjectInfo& buttonInfo, const buttonObjectColor& buttonColor, const String& label);
  void draw(bool flag_drawErase) override;
  void set();
  void reset();
  bool isButtonListObject() const override { return true; }  // 型確認メソッドをオーバーライド
};

class TriangleObject : public ScreenObject {
private:
  Adafruit_ILI9341* tft_;   // ILI9341 TFTディスプレイ
  boxObjectInfo info_;      // 配置情報
  boxObjectColor color_;
  triangleObjectInfo triangleInfo_;
  int16_t width, height;    // box width,height

public:
  TriangleObject(Adafruit_ILI9341* tft, const boxObjectInfo& info, const boxObjectColor& color,
                  const triangleObjectInfo& triangleInfo);
  void draw(bool flag_drawErase) override;
  bool isTriangleObject() const override { return true; }  // 型確認メソッドをオーバーライド
};

class CursorObject : public ScreenObject {
private:
  Adafruit_ILI9341* tft_;   // ILI9341 TFTディスプレイ
  cursorObjectInfo info_;      // 配置情報
  cursorObjectColor color_;
  int cursorPos_;

public:
  CursorObject(Adafruit_ILI9341* tft, const cursorObjectInfo& info, const cursorObjectColor& color, int cursorPos);
  void draw(bool flag_drawErase) override;
  void movePos(int cursorPos);
  bool isCursorObject() const override { return true; }  // 型確認メソッドをオーバーライド
};


// Screen Page class ********************************************************************************
// ページオブジェクト動作タイプの定義
enum ePageObjActionType{
  POAT_FIXED, POAT_MUTABLE, POAT_CURSOR, POAT_MAX
};

// ページオブジェクトクラスの定義
class Page {
private:
  std::vector<ScreenObject*> objects;
  ScreenObject *mutableObj[8];      // for change value later
  ScreenObject *cursorObj_;          // for change value later
  int scObjCount;

public:
  Page();
  ~Page();
  void addObject(ePageObjActionType pageObjActionType, ScreenObject* obj);
  void removeObject(ScreenObject* obj);
  void draw();
  void erase();
  ScreenObject* getMutableObj(int cnt);
  ScreenObject* getCursorObj();
};
// page object pointer
extern Page* page;


#endif