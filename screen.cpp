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
#include "screen.h"


// Screen Object class *********************************************************************************
// スクリーンオブジェクトクラス関数

// boxObjectInfo構造体中のTextAlignによりStringの座標を変換して&tx,&tyに出力する
void convTextAlignType2pos(int16_t& tx, int16_t& ty, boxObjectInfo& info,
                            const int16_t& width, const int16_t& height, const String& label){
  switch(info.textAlignType){
  case  ALIGN_LEFT:
    tx = ty = (height - 8 * info.textSize) / 2;     // set margin
    tx += info.boxPos[0];
    ty += info.boxPos[1];
    break;
  case  ALIGN_RIGHT:
    tx = ty = (height - 8 * info.textSize) / 2;     // set margin
    tx = info.boxPos[0] + width - (label.length() * 6 * info.textSize + tx);
    ty += info.boxPos[1];
    break;
  case  ALIGN_CENTER:
  default:
    tx = info.boxPos[0] + (width - label.length() * 6 * info.textSize) / 2 + 1;
    ty = info.boxPos[1] + (height - 8 * info.textSize) / 2;
    break;
  }
}

// Text描画関数
void drawText(Adafruit_ILI9341* tft, int16_t& tx, int16_t& ty, uint16_t& textColor, uint8_t& textSize, const String& label){
  tft->setCursor(tx, ty);
  tft->setTextColor(textColor);
  tft->setTextSize(textSize);
  tft->println(label);
}

// スクリーンオブジェクトクラスの定義
// Text Object **********
TextObject::TextObject(Adafruit_ILI9341* tft, const textObjectInfo& info, const String& label) 
  : tft_(tft), info_(info), label_(label){};

void TextObject::draw(bool flag_drawErase){
  uint16_t textColor;
  // setting draw/erase color
  if(flag_drawErase) textColor = info_.backgroundColor;
  else textColor = info_.textColor;
  // draw text
  drawText(tft_, info_.textPos[0], info_.textPos[1], textColor, info_.textSize, label_);
}

// Box Object **********
BoxObject::BoxObject(Adafruit_ILI9341* tft, const boxObjectInfo& info, const boxObjectColor& color, const String& label) 
  : tft_(tft), info_(info), color_(color), label_(label), width(info_.boxPos[2]-info_.boxPos[0]), height(info_.boxPos[3]-info_.boxPos[1]){
};

void BoxObject::draw(bool flag_drawErase){
  uint16_t lineColor, fillColor, textColor;
  // setting draw/erase color
  if(flag_drawErase) lineColor = fillColor = textColor = color_.backgroundColor;
  else{
    lineColor = color_.lineColor, fillColor = color_.fillColor, textColor = color_.textColor;
  }
  // draw box shape
  tft_->drawRect(info_.boxPos[0], info_.boxPos[1], width, height, lineColor);
  if(color_.fillColor != color_.backgroundColor){
    tft_->fillRect(info_.boxPos[0] + 1, info_.boxPos[1] + 1, width - 2, height - 2, fillColor);
  }
  // draw text
  convTextAlignType2pos(tx, ty, info_, width, height, label_);
  drawText(tft_, tx, ty, textColor, info_.textSize, label_);
}

void BoxObject::changeText(const String& label){
  // delete previous text
  drawText(tft_, tx, ty, color_.backgroundColor, info_.textSize, label_);
  // modifying text position
  if(label.length() != label_.length()){                // check for text length changes
    label_ = label;                                                       // update text
    convTextAlignType2pos(tx, ty, info_, width, height, label_);          // modifying tx, ty pos
  }
  else label_ = label;                                                    // update text
  // draw new text
  drawText(tft_, tx, ty, color_.textColor, info_.textSize, label_);
}

// List Object **********
ListObject::ListObject(Adafruit_ILI9341* tft, const boxObjectInfo& info, const boxObjectColor& color, const String& label) 
  : tft_(tft), info_(info), color_(color), label_(label), width(info_.boxPos[2]-info_.boxPos[0]), height(info_.boxPos[3]-info_.boxPos[1]){
};

void ListObject::draw(bool flag_drawErase){
  uint16_t lineColor, fillColor, textColor;
  // setting draw/erase color
  if(flag_drawErase) lineColor = fillColor = textColor = color_.backgroundColor;
  else{
    lineColor = color_.lineColor, fillColor = color_.fillColor, textColor = color_.textColor;
  }
  // draw list shape
  tft_->drawFastHLine(info_.boxPos[0], info_.boxPos[3], width, lineColor);
  if(color_.fillColor != color_.backgroundColor){
    tft_->fillRect(info_.boxPos[0] + 1, info_.boxPos[1] + 1, width - 2, height - 2, fillColor);
  }
  // draw text
  convTextAlignType2pos(tx, ty, info_, width, height, label_);
  drawText(tft_, tx, ty, textColor, info_.textSize, label_);
}


// ButtonList Object **********
ButtonListObject::ButtonListObject(Adafruit_ILI9341* tft, const boxObjectInfo& info, const boxObjectColor& color,
                                    const buttonObjectInfo& buttonInfo, const buttonObjectColor& buttonColor, const String& label) 
  : tft_(tft), info_(info), color_(color), buttonInfo_(buttonInfo), buttonColor_(buttonColor), label_(label),
    width(info_.boxPos[2]-info_.boxPos[0]), height(info_.boxPos[3]-info_.boxPos[1]){
};

void ButtonListObject::draw(bool flag_drawErase){
  uint16_t lineColor, fillColor, textColor, buttonColor;
  // setting draw/erase color
  if(flag_drawErase){
    // Erase
    lineColor = fillColor = textColor = buttonColor = color_.backgroundColor;
    reset();
  }
  else{ // draw
    lineColor = color_.lineColor, fillColor = color_.fillColor, textColor = color_.textColor, buttonColor = buttonColor_.lineColor;
  }
  // draw list shape
  tft_->drawFastHLine(info_.boxPos[0], info_.boxPos[3], width, lineColor);
  if(color_.fillColor != color_.backgroundColor){
    tft_->fillRect(info_.boxPos[0] + 1, info_.boxPos[1] + 1, width - 2, height - 2, fillColor);
  }
  // draw button shape
  tft_->drawCircle(info_.boxPos[0] + buttonInfo_.buttonPos[0], info_.boxPos[1] + buttonInfo_.buttonPos[1],
                    buttonInfo_.r_line, lineColor);
  // draw text
  tx = info_.boxPos[0] + 24;
  ty = info_.boxPos[1] + (height - 8 * info_.textSize) / 2;
  drawText(tft_, tx, ty, textColor, info_.textSize, label_);
}

void ButtonListObject::set(){
  // draw button shape
  tft_->fillCircle(info_.boxPos[0] + buttonInfo_.buttonPos[0], info_.boxPos[1] + buttonInfo_.buttonPos[1],
                    buttonInfo_.r_fill, buttonColor_.fillColor);
}
void ButtonListObject::reset(){
  uint16_t buttonFillColor;
  // setting reset color
  if(color_.fillColor != color_.backgroundColor) buttonFillColor = color_.fillColor;
  else buttonFillColor = color_.backgroundColor;
  // draw button shape
  tft_->fillCircle(info_.boxPos[0] + buttonInfo_.buttonPos[0], info_.boxPos[1] + buttonInfo_.buttonPos[1],
                    buttonInfo_.r_fill, buttonFillColor);
}

// Triangle Object **********
TriangleObject::TriangleObject(Adafruit_ILI9341* tft, const boxObjectInfo& info, const boxObjectColor& color, 
  const triangleObjectInfo& triangleInfo)
  : tft_(tft), info_(info), color_(color), triangleInfo_(triangleInfo) {
    width = (info_.boxPos[2]-info_.boxPos[0]);
    height = (info_.boxPos[3]-info_.boxPos[1]);
};

void TriangleObject::draw(bool flag_drawErase){
  uint16_t lineColor, fillColor, triLineColor, triFillColor;
  // setting draw/erase color
  if(flag_drawErase){
    lineColor = fillColor = color_.backgroundColor;
    triLineColor = triFillColor = triangleInfo_.backgroundColor;
  } 
  else{
    lineColor = color_.lineColor; fillColor = color_.fillColor;
    triLineColor = triangleInfo_.lineColor; triFillColor = triangleInfo_.fillColor;
  }
  // draw box shape
  tft_->drawRect(info_.boxPos[0], info_.boxPos[1], width, height, lineColor);
  if(color_.fillColor != color_.backgroundColor){
    tft_->fillRect(info_.boxPos[0] + 1, info_.boxPos[1] + 1, width - 2, height - 2, fillColor);
  }
  // draw triangle
  tft_->drawTriangle(triangleInfo_.trianglePos[0], triangleInfo_.trianglePos[1], triangleInfo_.trianglePos[2],
                      triangleInfo_.trianglePos[3], triangleInfo_.trianglePos[4], triangleInfo_.trianglePos[5], triLineColor);
  if(triangleInfo_.fillColor != triangleInfo_.backgroundColor){
    tft_->fillTriangle(triangleInfo_.trianglePos[0], triangleInfo_.trianglePos[1], triangleInfo_.trianglePos[2],
                      triangleInfo_.trianglePos[3], triangleInfo_.trianglePos[4], triangleInfo_.trianglePos[5], triFillColor);
  }
}

// Cursor Object **********
CursorObject::CursorObject(Adafruit_ILI9341* tft, const cursorObjectInfo& info, const cursorObjectColor& color, int cursorPos) 
  : tft_(tft), info_(info), color_(color), cursorPos_(cursorPos){
};

void CursorObject::draw(bool flag_drawErase){
  uint16_t fillColor;
  // limit cursorPos_ range
  if(cursorPos_ >= VALUEDIGITMAX) cursorPos_ = VALUEDIGITMAX - 1;
  else if(cursorPos_ < 0) cursorPos_ = 0;
  // setting draw/erase color
  if(flag_drawErase) fillColor = color_.backgroundColor;
  else fillColor = color_.fillColor;
  // draw cursor
  int16_t x0 = info_.valueDigitPosList[2 * cursorPos_];
  int16_t y0 = info_.valueDigitPosList[2 * cursorPos_ + 1];
  tft_->fillTriangle(x0 + info_.offsetPos[0], y0 + info_.offsetPos[1],
                      x0 + info_.offsetPos[2], y0 + info_.offsetPos[3],
                      x0 + info_.offsetPos[4], y0 + info_.offsetPos[5], fillColor);
}

void CursorObject::movePos(int cursorPos){
  draw(FLAG_ERASE);
  cursorPos_ = cursorPos;
  draw(FLAG_DRAW);
}


// Screen Page class *********************************************************************************
// ページオブジェクトクラスの定義
// ページ管理
Page::Page() : scObjCount(0) {
  for(int i = 0; i < MUTABLEOBJMAX; i++){
    mutableObj[i] = nullptr;
  }
}
Page::~Page() {
  for (auto obj : objects) {
    obj->draw(FLAG_ERASE);  // addした全オブジェクトをbackgroundColorで消す
    delete obj;
  }
}

// newしたオブジェクトアドレスobjをaddする
void Page::addObject(ePageObjActionType pageObjActionType, ScreenObject* obj) {
  objects.push_back(obj);
  // 表示内容が変化するオブジェクトは外部から表示を書き換えられるようにリストに登録
  if(pageObjActionType == POAT_MUTABLE && scObjCount < MUTABLEOBJMAX){
    mutableObj[scObjCount] = obj;
    scObjCount++;
  }
  else if(pageObjActionType == POAT_CURSOR){
    cursorObj_ = obj;
  }
}

void Page::removeObject(ScreenObject* obj) {
  objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
  delete obj;
}

// addした全オブジェクトを画面に描画する
void Page::draw() {
  for (auto obj : objects) {
    obj->draw(FLAG_DRAW);
  }
}

// addした全オブジェクトをbackgroundColorで消す
void Page::erase() {
  for (auto obj : objects) {
    obj->draw(FLAG_ERASE);
  }
}

// あとから表示変更可能なオブジェクトのアドレス取得I/F
ScreenObject* Page::getMutableObj(int cnt){
  return mutableObj[cnt];
}
ScreenObject* Page::getCursorObj(){
  return cursorObj_;
}

// page object pointerの実態
Page* page = nullptr;




