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

#include "FLCM1_IO.h"

// **************************************************************************************************************
// Flash Storage area for Device Settings ***********************************************************************
// **************************************************************************************************************

//FLASHに読み書き格納する 要FlashStorageライブラリ
FlashStorage(device_settings_flash_0, DeviceSettings);
FlashStorage(device_settings_flash_1, DeviceSettings);
FlashStorage(device_settings_flash_2, DeviceSettings);
FlashStorage(device_settings_flash_3, DeviceSettings);
FlashStorage(device_settings_flash_4, DeviceSettings);
FlashStorage(device_settings_flash_5, DeviceSettings);
FlashStorage(device_settings_flash_6, DeviceSettings);
FlashStorage(device_settings_flash_7, DeviceSettings);
FlashStorage(device_settings_flash_temp, DeviceSettings);


// **************************************************************************************************************
// Device Settings Manager class ********************************************************************************
// **************************************************************************************************************
// 以下eDeviceSettingRegTypeとDeviceSettingsとセットで変更すること
// 設定記憶域操作クラスの定義
SettingsManager::SettingsManager(){};

// 設定値の呼び出し
int32_t SettingsManager::getSettingValue(eDeviceSettingRegType regType, int regIndex){
  switch(regType){
    case CANSPEED:  return currentDeviceSetting_.canSpeed;
    case HWFFL:     return currentDeviceSetting_.hwffl[regIndex];
    case SWFSW:     return currentDeviceSetting_.swf[regIndex].onoff;
    case SWFSU:     return currentDeviceSetting_.swf[regIndex].sign;
    case COSW:      return currentDeviceSetting_.co[regIndex].onoff;
    case COPOL:     return currentDeviceSetting_.co[regIndex].pol;
    case AOSET:     return currentDeviceSetting_.ao[regIndex];
    case SLSV: case SLLD: return 0;
    case DS_OPSM:   return currentDeviceSetting_.op[regIndex];
    case DS_HWF:    return currentDeviceSetting_.hwf[regIndex];
    case SWFID:     return currentDeviceSetting_.swf[regIndex].canID;
    case SWFSB:     return currentDeviceSetting_.swf[regIndex].startByte;
    case SWFSI:     return currentDeviceSetting_.swf[regIndex].startBit;
    case SWFEB:     return currentDeviceSetting_.swf[regIndex].endByte;
    case SWFEI:     return currentDeviceSetting_.swf[regIndex].endBit;
    case COUSF:     return currentDeviceSetting_.co[regIndex].usingSwf;
    //case COTRS:     return currentDeviceSetting_.co[regIndex].threshould;// move to the different return method
    default: DEBUG_PRINT("Error: getSettingValue regType=");DEBUG_PRINTLN(regType); return ERROR_GENERAL; // エラー
  }
}
uint64_t SettingsManager::getSettingAnyvalue(eDeviceSettingRegType regType, int regIndex){
  switch(regType){
    case COTRS:     return currentDeviceSetting_.co[regIndex].threshould;
    default: DEBUG_PRINTLN("Error: getSettingAnyvalue regType=");DEBUG_PRINTLN(regType); return ERROR_GENERAL; // エラー
  }
}

// 設定値の範囲内チェック for used other than the any value(COTRS)
bool SettingsManager::isValidSetting(int32_t value, eDeviceSettingRegType regType, int pageIndex){
  switch(regType){
    case CANSPEED:
      if(value >= 0 && value < getButtonCount(pageIndex)) return true;
      break;
    case HWFFL: case SWFSW: case SWFSU: case COSW: case COPOL: case AOSET: case DS_OPSM:
      if(value == 0 || value == 1) return true;
      break;
    case DS_HWF: case SWFID: case SWFSB: case SWFSI: case SWFEB: case SWFEI: case COUSF: case COTRS:
      // ANY値の場合この関数を呼べないため無条件にfalse
      if(!getValueIsAny(pageIndex) && value >= getValueMin(pageIndex) && value <= getValueMax(pageIndex)) return true;
      break;
    default: DEBUG_PRINTLN("Error: SettingsManager_isValidSetting"); // エラー
      break;
  }
  return false;
}

// 設定値の書き込み
void SettingsManager::setSettingValue(int32_t value, eDeviceSettingRegType regType, int pageIndex, int regIndex){
  if(isValidSetting(value, regType, pageIndex)){    // check value
    switch(regType){
      case CANSPEED: currentDeviceSetting_.canSpeed = value;                break;
      case HWFFL:    currentDeviceSetting_.hwffl[regIndex] = value;         break;
      case SWFSW:    currentDeviceSetting_.swf[regIndex].onoff = value;     break;
      case SWFSU:    currentDeviceSetting_.swf[regIndex].sign = value;      break;
      case COSW:     currentDeviceSetting_.co[regIndex].onoff = value;      break;
      case COPOL:    currentDeviceSetting_.co[regIndex].pol = value;        break;
      case AOSET:    currentDeviceSetting_.ao[regIndex] = value;            break;
      case DS_OPSM:  currentDeviceSetting_.op[regIndex] = value;            break;
      case DS_HWF:   currentDeviceSetting_.hwf[regIndex] = value;           break;
      case SWFID:    currentDeviceSetting_.swf[regIndex].canID = value;     break;
      case SWFSB:    currentDeviceSetting_.swf[regIndex].startByte = value; break;
      case SWFSI:    currentDeviceSetting_.swf[regIndex].startBit = value;  break;
      case SWFEB:    currentDeviceSetting_.swf[regIndex].endByte = value;   break;
      case SWFEI:    currentDeviceSetting_.swf[regIndex].endBit = value;    break;
      case COUSF:    currentDeviceSetting_.co[regIndex].usingSwf = value;   break;
      //case COTRS:    currentDeviceSetting_.co[regIndex].threshould = value; break;// move to the overload method
      default: DEBUG_PRINTLN("Error: setSettingValue regType");     break;
    }
  }
  else DEBUG_PRINTLN("Error: setSettingValue range");
}
void SettingsManager::setSettingValue(uint64_t value, eDeviceSettingRegType regType, int pageIndex, int regIndex){
  switch(regType){
    case COTRS:    currentDeviceSetting_.co[regIndex].threshould = value; break;
    default: DEBUG_PRINTLN("Error: setSettingValue regType");     break;
  }
}

// 現在設定値をFlash領域にsave
void SettingsManager::saveDeviceSettings(int pos){
  switch((eSaveLoadPos)pos){
    case SLP_SL0: device_settings_flash_0.write(currentDeviceSetting_); break;
    case SLP_SL1: device_settings_flash_1.write(currentDeviceSetting_); break;
    case SLP_SL2: device_settings_flash_2.write(currentDeviceSetting_); break;
    case SLP_SL3: device_settings_flash_3.write(currentDeviceSetting_); break;
    case SLP_SL4: device_settings_flash_4.write(currentDeviceSetting_); break;
    case SLP_SL5: device_settings_flash_5.write(currentDeviceSetting_); break;
    case SLP_SL6: device_settings_flash_6.write(currentDeviceSetting_); break;
    case SLP_SL7: device_settings_flash_7.write(currentDeviceSetting_); break;
    case SLP_TEMP: device_settings_flash_temp.write(currentDeviceSetting_); break;
    default: DEBUG_PRINTLN("Error: saveDeviceSettings pos");           return;
  }
  DEBUG2_PRINTLN("DeviceSettings SAVED!!");
}

// 現在設定値をFlash領域からload
void SettingsManager::loadDeviceSettings(int pos){
  switch((eSaveLoadPos)pos){
    case SLP_SL0: currentDeviceSetting_ = device_settings_flash_0.read(); break;
    case SLP_SL1: currentDeviceSetting_ = device_settings_flash_1.read(); break;
    case SLP_SL2: currentDeviceSetting_ = device_settings_flash_2.read(); break;
    case SLP_SL3: currentDeviceSetting_ = device_settings_flash_3.read(); break;
    case SLP_SL4: currentDeviceSetting_ = device_settings_flash_4.read(); break;
    case SLP_SL5: currentDeviceSetting_ = device_settings_flash_5.read(); break;
    case SLP_SL6: currentDeviceSetting_ = device_settings_flash_6.read(); break;
    case SLP_SL7: currentDeviceSetting_ = device_settings_flash_7.read(); break;
    case SLP_TEMP: currentDeviceSetting_ = device_settings_flash_temp.read(); break;
    default: DEBUG_PRINTLN("Error: loadDeviceSettings pos");             return;
  }
  DEBUG2_PRINTLN("DeviceSettings LOADED!!");
}

// bit列抽出用設定のLengthをあらかじめ計算
// startBit of startByte must be left or same pos from endbit of endByte
void extractLen(SettingsManager *setMan, uint8_t swfIndex, uint8_t *byteLen, uint8_t *bitLen) {
  uint8_t startByte = setMan->getSettingValue(SWFSB, swfIndex);
  uint8_t startBit = setMan->getSettingValue(SWFSI, swfIndex);
  uint8_t endByte = setMan->getSettingValue(SWFEB, swfIndex);
  uint8_t endBit = setMan->getSettingValue(SWFEI, swfIndex);

  // エラーチェック
  if (startByte > 7 || endByte > 7 || startBit > 7 || endBit > 7 || (startByte > endByte) || 
      (startByte == endByte && startBit < endBit)){
    DEBUG_PRINTLN("Error: extractSettings");
    return;     // error
  }
  // calc len
  uint8_t bytl, bitl;
  if(startByte == endByte){
    bytl = 1;
    bitl = startBit - endBit + 1;
  }
  else{
    bytl = ((1 + startBit + 8 - endBit + 8 * (endByte - startByte - 1)) - 1 >> 3) + 1;
    if(bytl > 4) bytl = 8;
    else if (bytl > 2) bytl = 4;
    bitl = startBit + 1 + (endByte - startByte - 1) * 8 + 8 - endBit;
  }
  *byteLen = bytl;
  *bitLen = bitl;
}


// **************************************************************************************************************
// Interval Timer **************************************************************************************************
// **************************************************************************************************************
IntervalTimer::IntervalTimer(unsigned int duration, bool accumulateMode)
  : duration(duration), startTimestamp(0), elapsedIntervals(0), accumulateMode(accumulateMode) {};

// タイマーが期限切れかどうかを確認
bool IntervalTimer::isExpired() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTimestamp;
  if (elapsedTime >= duration) {
    if(accumulateMode){                 // 蓄積モードの場合は経過インターバルを計算
      unsigned int count = elapsedTime / duration;
      elapsedIntervals += count;
      startTimestamp += count * duration;
    }
    else startTimestamp = currentTime;  // 通常モードの場合はタイマーをリセット
    return true;
  }
  return false;
}

// 経過したインターバルの回数を取得
unsigned long IntervalTimer::getElapsedIntervals() {
  if (!accumulateMode) {
    // 通常モードでは常に0を返す
    return 0;
  }
  isExpired(); // 最新の状態に更新
  unsigned long intervals = elapsedIntervals;
  elapsedIntervals = 0; // 取得後にリセット
  return intervals;
}

// タイマーの開始時間をリセットする
void IntervalTimer::reset() {
  startTimestamp = millis();
  elapsedIntervals = 0;
}

// タイマーの持続時間を変更する
void IntervalTimer::setDuration(unsigned int newDuration) {
  duration = newDuration;
}


// **************************************************************************************************************
// Display ******************************************************************************************************
// **************************************************************************************************************
// Screen Objects *********************************************************************************
// Common buttons
const boxObjectInfo boxOI_back = {{180,16,227,47}, 2, ALIGN_CENTER};
const boxObjectColor boxOC_default = {ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, ILI9341_BLACK};
const boxObjectInfo boxOI_cancel = {{24,240,107,271}, 2, ALIGN_CENTER};
const boxObjectColor boxOC_cancel = {ILI9341_WHITE, ILI9341_RED, ILI9341_WHITE, ILI9341_BLACK};
const boxObjectInfo boxOI_enter = {{132,240,215,271}, 2, ALIGN_CENTER};
const boxObjectColor boxOC_enter = {ILI9341_WHITE, ILI9341_GREEN, ILI9341_BLACK, ILI9341_BLACK};
// Message
const boxObjectInfo boxOI_notice = {{0,240,239,271}, 4, ALIGN_CENTER};
const boxObjectColor boxOC_notice = {ILI9341_BLACK, ILI9341_BLACK, ILI9341_PINK, ILI9341_BLACK};
const boxObjectInfo boxOI_noticeSmall = {{0,304,239,319}, 2, ALIGN_CENTER};
const boxObjectColor boxOC_noticeSmall = {ILI9341_BLACK, ILI9341_BLACK, ILI9341_PINK, ILI9341_BLACK};
// CAN voltage obj
const boxObjectInfo boxOI_canVolgate = {{114,0,137,7}, 1, ALIGN_RIGHT};
const boxObjectColor boxOC_textOnly = {
  STATUSLINE_BACKGROUNDCOLOR, STATUSLINE_BACKGROUNDCOLOR, STATUSLINE_TEXTCOLOR, STATUSLINE_BACKGROUNDCOLOR};
// icons
const boxObjectInfo boxOI_iconHF = {{150,0,161,7}, 1, ALIGN_CENTER};
const boxObjectInfo boxOI_iconHD = {{162,0,173,7}, 1, ALIGN_CENTER};
const boxObjectInfo boxOI_iconHA = {{174,0,185,7}, 1, ALIGN_CENTER};
const boxObjectInfo boxOI_iconSF = {{192,0,203,7}, 1, ALIGN_CENTER};
const boxObjectInfo boxOI_iconSD = {{204,0,215,7}, 1, ALIGN_CENTER};
const boxObjectInfo boxOI_iconSA = {{216,0,227,7}, 1, ALIGN_CENTER};
const boxObjectInfo boxOI_iconCD = {{228,0,239,7}, 1, ALIGN_CENTER};
const boxObjectColor boxOC_iconDefault = {
  STATUSLINE_LINECOLOR, STATUSLINE_BACKGROUNDCOLOR, STATUSLINE_TEXTCOLOR, STATUSLINE_BACKGROUNDCOLOR};
// Round button
const buttonObjectInfo buttonOI_default = {{12, 16}, 8, 5};
const buttonObjectColor buttonOC_default = {ILI9341_WHITE, ILI9341_RED, ILI9341_BLACK};
// Value Up Down buttons  triangle:x0,y0,x1,y1,x2,y2
const boxObjectInfo boxOI_valueUp = {{192,80,215,111}, 2, ALIGN_CENTER};
const triangleObjectInfo triangleOI_valueUp = {{199,101,207,101,204,90},ILI9341_WHITE, ILI9341_RED, ILI9341_BLACK};
const boxObjectInfo boxOI_valueDown = {{192,176,215,207}, 2, ALIGN_CENTER};
const triangleObjectInfo triangleOI_valueDown = {{199,186,207,186,204,197},ILI9341_WHITE, ILI9341_RED, ILI9341_BLACK};
// Cursor
const cursorObjectInfo cursorOI_value = {{1,6,21,6,11,2},{192,160,168,160,144,160,120,160,96,160,72,160,48,160,24,160}};
const cursorObjectInfo cursorOI_list = {{0,2,0,28,4,14},{0,64,0,96,0,128,0,160,0,192,0,224,0,256,0,288}};
const cursorObjectColor cursorOC_default = {ILI9341_CYAN, ILI9341_BLACK};
// Text top,sub title
const textObjectInfo textOI_topTitle = {{12,16}, 3, ILI9341_WHITE, ILI9341_BLACK};
const textObjectInfo textOI_subTitle = {{12,48}, 2, ILI9341_WHITE, ILI9341_BLACK};

// Screen Page Objects *********************************************************************************
enum ePageObjectType {
  BUTTON_BACK,
  LIST_1LINE,
  BUTTONLIST_1LINE,
  VALUE_1DIGIT,
  TRIANGLE_UP,
  TRIANGLE_DOWN,
  BUTTON_CANCEL,
  BUTTON_ENTER,
  CURSOR,
  PAGEOBJTYPEMAX
};

// ********** List8の表示オブジェクトリスト
const ePageObjectType pageObjList_list8[] = {
  BUTTON_BACK, LIST_1LINE, LIST_1LINE, LIST_1LINE,
  LIST_1LINE, LIST_1LINE, LIST_1LINE, LIST_1LINE, 
  LIST_1LINE, CURSOR};

// リストオブジェクトの配置情報
const boxObjectInfo boxOI_list8[] = {
  {{12,64,239,95}, 2, ALIGN_LEFT},
  {{12,96,239,127}, 2, ALIGN_LEFT},
  {{12,128,239,159}, 2, ALIGN_LEFT},
  {{12,160,239,191}, 2, ALIGN_LEFT},
  {{12,192,239,223}, 2, ALIGN_LEFT},
  {{12,224,239,255}, 2, ALIGN_LEFT},
  {{12,256,239,287}, 2, ALIGN_LEFT},
  {{12,288,239,319}, 2, ALIGN_LEFT},
};

// ********** Button8の表示オブジェクトリスト
const ePageObjectType pageObjList_buttonBox8[] = {
  BUTTON_BACK, BUTTONLIST_1LINE, BUTTONLIST_1LINE, BUTTONLIST_1LINE,
  BUTTONLIST_1LINE, BUTTONLIST_1LINE, BUTTONLIST_1LINE, BUTTONLIST_1LINE,
  BUTTONLIST_1LINE, CURSOR};

// ボタンリストオブジェクトの配置情報
const boxObjectInfo boxOI_buttonBox8[] = {
  {{24,64,239,95}, 2, ALIGN_LEFT},
  {{24,96,239,127}, 2, ALIGN_LEFT},
  {{24,128,239,159}, 2, ALIGN_LEFT},
  {{24,160,239,191}, 2, ALIGN_LEFT},
  {{24,192,239,223}, 2, ALIGN_LEFT},
  {{24,224,239,255}, 2, ALIGN_LEFT},
  {{24,256,239,287}, 2, ALIGN_LEFT},
  {{24,288,239,319}, 2, ALIGN_LEFT}
};

// ********** Valueの表示オブジェクトリスト
const ePageObjectType pageObjList_value[] = {
  BUTTON_BACK, TRIANGLE_UP, VALUE_1DIGIT, VALUE_1DIGIT,
  VALUE_1DIGIT, VALUE_1DIGIT, VALUE_1DIGIT, VALUE_1DIGIT,
  VALUE_1DIGIT, VALUE_1DIGIT, CURSOR, TRIANGLE_DOWN,
  BUTTON_CANCEL, BUTTON_ENTER};

// VALUE_1DIGITオブジェクトの配置情報
const boxObjectInfo boxOI_value[] = {
  {{192,128,215,159}, 4, ALIGN_LEFT},
  {{168,128,191,159}, 4, ALIGN_LEFT},
  {{144,128,167,159}, 4, ALIGN_LEFT},
  {{120,128,143,159}, 4, ALIGN_LEFT},
  {{96,128,119,159}, 4, ALIGN_LEFT},
  {{72,128,95,159}, 4, ALIGN_LEFT},
  {{48,128,71,159}, 4, ALIGN_LEFT},
  {{24,128,47,159}, 4, ALIGN_LEFT},
};


// Page Information *******************************
// union labels
const char* LavelCanId = "CAN ID";
const char* LavelStartBytePos = "StartByte Position";
const char* LavelStartBitPos = "StartBit Position";
const char* LavelEndBytePos = "EndByte Position";
const char* LavelEndBitPos = "EndBit Position";
const char* LavelOnOff = "ON/OFF";
const char* LavelByteCount = "Byte Count";
const char* LavelByteOrder = "Byte Order";
const char* LavelUsingSwfNo = "Using SWF No. 0-7";
const char* LavelThreshould = "Threshould Value";
const char* LavelOutputPolarity = "Output Polarity";
const char* LavelSaveToMemory = "Save to Memory";
const char* LavelLoadFromMemory = "Load from Memory";
const char* LavelSaveLoad = "Save/Load";
const char* LavelOff = "OFF";
const char* LavelOn = "ON";
const char* LavelSwf0 = "SWF0";
const char* LavelSwf1 = "SWF1";
const char* LavelSwf2 = "SWF2";
const char* LavelSwf3 = "SWF3";
const char* LavelSwf4 = "SWF4";
const char* LavelSwf5 = "SWF5";
const char* LavelSwf6 = "SWF6";
const char* LavelSwf7 = "SWF7";
const char* LavelSoftwareFilter0 = "SoftwareFilter0";
const char* LavelSoftwareFilter1 = "SoftwareFilter1";
const char* LavelSoftwareFilter2 = "SoftwareFilter2";
const char* LavelSoftwareFilter3 = "SoftwareFilter3";
const char* LavelSoftwareFilter4 = "SoftwareFilter4";
const char* LavelSoftwareFilter5 = "SoftwareFilter5";
const char* LavelSoftwareFilter6 = "SoftwareFilter6";
const char* LavelSoftwareFilter7 = "SoftwareFilter7";
const char* LavelBigEndian = "BIG endian";
const char* LavelLittleEndian = "LITTLE endian";
const char* LavelCo0 = "CO0";
const char* LavelCo1 = "CO1";
const char* LavelCo2 = "CO2";
const char* LavelCo3 = "CO3";
const char* LavelCo4 = "CO4";
const char* LavelCo5 = "CO5";
const char* LavelCompareOut0 = "CompareOut0";
const char* LavelCompareOut1 = "CompareOut1";
const char* LavelCompareOut2 = "CompareOut2";
const char* LavelCompareOut3 = "CompareOut3";
const char* LavelCompareOut4 = "CompareOut4";
const char* LavelCompareOut5 = "CompareOut5";
const char* LavelActive = "Active(Low)";
const char* LavelInactive = "Inactive(HighZ)";
const char* LavelIfSwfmsg = "If SWFmsg >= TRS";
const char* LavelAuxOut = "AUX Out";
const char* LavelNo = "NO";
const char* LavelYes = "Yes";
const char* LavelSl0 = "SL0";
const char* LavelSl1 = "SL1";
const char* LavelSl2 = "SL2";
const char* LavelSl3 = "SL3";
const char* LavelSl4 = "SL4";
const char* LavelSl5 = "SL5";
const char* LavelSl6 = "SL6";
const char* LavelSl7 = "SL7";
const char* LavelSaveSettings = "Save settings";
const char* LavelLoadSettings = "Load settings";
const char* LavelFiltCanId = "Filtering CAN ID";
const char* LavelByteCounts = "ByteCounts 1-8";
const char* LavelSigned = "Signed int";
const char* LavelUnsigned = "Unsigned int";
const char* LavelSignedUnsigned = "Signed/Unsigned";
const char* LavelIDlength = "ID length";
const char* LavelFilterValue = "Filter value";
const char* LavelHwfFilter0 = "HWF F0";
const char* LavelHwfFilter1 = "HWF F1";
const char* LavelHwfFilter2 = "HWF F2";
const char* LavelHwfFilter3 = "HWF F3";
const char* LavelHwfFilter4 = "HWF F4";
const char* LavelHwfFilter5 = "HWF F5";
const char* LavelStandard = "Standard 11bits";
const char* LavelExtended = "Extended 29bits";
const char* LavelOption = "Option";
const char* LavelSwapCE = "Swap C-E SWs";


// ページの情報 enum ePageと１対１対応
// List page type *******************************
const pageInfo_list8 pageList8[] = {
  // MENU_TOP
  {7, INMONITOR, {CAN_SPEED, HWF, SWF, CO, AO, SL, OP}, 
   {"CAN speed", "HardWareFilter", "SoftWareFilter", "ComparatorOutput", "AuxOutput", LavelSaveLoad, "Option"},
    "SETTINGS","TOP MENU"},
  // HWF
  {8, MENU_TOP, {HWF0, HWFF0, HWFF1, HWF3, HWFF2, HWFF3, HWFF4, HWFF5}, 
   {"Mask0", "|-- Filter0", "|__ Filter1", "Mask1", "|-- Filter2", "|-- Filter3", "|-- Filter4", 
    "|__ Filter5"},"HWF","HardwareFilter"},
  // SWF
  {8, MENU_TOP, {SWF0, SWF1, SWF2, SWF3, SWF4, SWF5, SWF6, SWF7}, 
   {LavelSoftwareFilter0, LavelSoftwareFilter1, LavelSoftwareFilter2, LavelSoftwareFilter3,
    LavelSoftwareFilter4, LavelSoftwareFilter5, LavelSoftwareFilter6, LavelSoftwareFilter7},
    "SWF","SoftwareFilter"},
  // CO
  {6, MENU_TOP, {CO0, CO1, CO2, CO3, CO4, CO5},
   {LavelCompareOut0, LavelCompareOut1, LavelCompareOut2, LavelCompareOut3, LavelCompareOut4,
    LavelCompareOut5} ,"CO","CompareOut"},
  // AO
  {3, MENU_TOP, {AOHSW, AOSSW, AOSBO},
   {"HWFOut ON/OFF", "SWFOut ON/OFF", "SWFOut byte Order"}
   ,"AO","AuxOutput"},
  // SL
  {8, MENU_TOP, {SL0, SL1, SL2, SL3, SL4, SL5, SL6, SL7}, 
   {"Memory0", "Memory1", "Memory2", "Memory3", "Memory4", "Memory5", "Memory6", "Memory7"}
   ,"SL","Save/Load Setting"},
  // OP
  {1, MENU_TOP, {OPSMCE}, {LavelSwapCE}, LavelOption, "Option Settings"},
  // HWFF0-F5
  {2, HWF, {HWFF0L, HWF1}, {LavelIDlength, LavelFilterValue}, LavelHwfFilter0, "Hardware Filter0"},
  {2, HWF, {HWFF1L, HWF2}, {LavelIDlength, LavelFilterValue}, LavelHwfFilter1, "Hardware Filter1"},
  {2, HWF, {HWFF2L, HWF4}, {LavelIDlength, LavelFilterValue}, LavelHwfFilter2, "Hardware Filter2"},
  {2, HWF, {HWFF3L, HWF5}, {LavelIDlength, LavelFilterValue}, LavelHwfFilter3, "Hardware Filter3"},
  {2, HWF, {HWFF4L, HWF6}, {LavelIDlength, LavelFilterValue}, LavelHwfFilter4, "Hardware Filter4"},
  {2, HWF, {HWFF5L, HWF7}, {LavelIDlength, LavelFilterValue}, LavelHwfFilter5, "Hardware Filter5"},
  // SWF0
  {7, SWF, {SWF0SW, SWF0ID, SWF0SB, SWF0SI, SWF0EB, SWF0EI, SWF0SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf0,LavelSoftwareFilter0},
  // SWF1
  {7, SWF, {SWF1SW, SWF1ID, SWF1SB, SWF1SI, SWF1EB, SWF1EI, SWF1SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf1,LavelSoftwareFilter1},
  // SWF2
  {7, SWF, {SWF2SW, SWF2ID, SWF2SB, SWF2SI, SWF2EB, SWF2EI, SWF2SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf2,LavelSoftwareFilter2},
  // SWF3
  {7, SWF, {SWF3SW, SWF3ID, SWF3SB, SWF3SI, SWF3EB, SWF3EI, SWF3SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf3,LavelSoftwareFilter3},
  // SWF4
  {7, SWF, {SWF4SW, SWF4ID, SWF4SB, SWF4SI, SWF4EB, SWF4EI, SWF4SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf4,LavelSoftwareFilter4},
  // SWF5
  {7, SWF, {SWF5SW, SWF5ID, SWF5SB, SWF5SI, SWF5EB, SWF5EI, SWF5SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf5,LavelSoftwareFilter5},
  // SWF6
  {7, SWF, {SWF6SW, SWF6ID, SWF6SB, SWF6SI, SWF6EB, SWF6EI, SWF6SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf6,LavelSoftwareFilter6},
  // SWF7
  {7, SWF, {SWF7SW, SWF7ID, SWF7SB, SWF7SI, SWF7EB, SWF7EI, SWF7SU}, 
   {LavelOnOff, LavelCanId, LavelStartBytePos, LavelStartBitPos, LavelEndBytePos, LavelEndBitPos, LavelSignedUnsigned}
   ,LavelSwf7,LavelSoftwareFilter7},
  // CO0
  {4, CO, {CO0SW, CO0USF, CO0TRS, CO0POL}, 
   {LavelOnOff, LavelUsingSwfNo, LavelThreshould, LavelOutputPolarity}
   ,LavelCo0,LavelCompareOut0},
  // CO1
  {4, CO, {CO1SW, CO1USF, CO1TRS, CO1POL}, 
   {LavelOnOff, LavelUsingSwfNo, LavelThreshould, LavelOutputPolarity}
   ,LavelCo1,LavelCompareOut1},
  // CO2
  {4, CO, {CO2SW, CO2USF, CO2TRS, CO2POL}, 
   {LavelOnOff, LavelUsingSwfNo, LavelThreshould, LavelOutputPolarity}
   ,LavelCo2,LavelCompareOut2},
  // CO3
  {4, CO, {CO3SW, CO3USF, CO3TRS, CO3POL}, 
   {LavelOnOff, LavelUsingSwfNo, LavelThreshould, LavelOutputPolarity}
   ,LavelCo3,LavelCompareOut3},
  // CO4
  {4, CO, {CO4SW, CO4USF, CO4TRS, CO4POL}, 
   {LavelOnOff, LavelUsingSwfNo, LavelThreshould, LavelOutputPolarity}
   ,LavelCo4,LavelCompareOut4},
  // CO5
  {4, CO, {CO5SW, CO5USF, CO5TRS, CO5POL}, 
   {LavelOnOff, LavelUsingSwfNo, LavelThreshould, LavelOutputPolarity}
   ,LavelCo5,LavelCompareOut5},
  // SL0
  {2, SL, {SL0SV, SL0LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl0,LavelSaveLoad},
  // SL1
  {2, SL, {SL1SV, SL1LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl1,LavelSaveLoad},
  // SL2
  {2, SL, {SL2SV, SL2LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl2,LavelSaveLoad},
  // SL3
  {2, SL, {SL3SV, SL3LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl3,LavelSaveLoad},
  // SL4
  {2, SL, {SL4SV, SL4LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl4,LavelSaveLoad},
  // SL5
  {2, SL, {SL5SV, SL5LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl5,LavelSaveLoad},
  // SL6
  {2, SL, {SL6SV, SL6LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl6,LavelSaveLoad},
  // SL7
  {2, SL, {SL7SV, SL7LD}, {LavelSaveToMemory, LavelLoadFromMemory},LavelSl7,LavelSaveLoad},
};


// Button 8line page type *******************************
// ページの情報 enum ePageと１対１対応
const pageInfo_button8 pageButton8[] = {
  // CAN_SPEED
  {8, MENU_TOP, {"10kbps", "50kbps", "100kbps", "125kbps", "250kbps", "500kbps", "800kbps", "1Mbps"},
    "CANSPEED","select speed"},
  // HWFFxL
  {2, HWFF0, {LavelStandard, LavelExtended}, LavelHwfFilter0, "Filter0 ID length"},
  {2, HWFF1, {LavelStandard, LavelExtended}, LavelHwfFilter1, "Filter1 ID length"},
  {2, HWFF2, {LavelStandard, LavelExtended}, LavelHwfFilter2, "Filter2 ID length"},
  {2, HWFF3, {LavelStandard, LavelExtended}, LavelHwfFilter3, "Filter3 ID length"},
  {2, HWFF4, {LavelStandard, LavelExtended}, LavelHwfFilter4, "Filter4 ID length"},
  {2, HWFF5, {LavelStandard, LavelExtended}, LavelHwfFilter5, "Filter5 ID length"},
  // SWFxSW
  {2, SWF0, {LavelOff, LavelOn},LavelSwf0,LavelSoftwareFilter0},
  {2, SWF1, {LavelOff, LavelOn},LavelSwf1,LavelSoftwareFilter1},
  {2, SWF2, {LavelOff, LavelOn},LavelSwf2,LavelSoftwareFilter2},
  {2, SWF3, {LavelOff, LavelOn},LavelSwf3,LavelSoftwareFilter3}, 
  {2, SWF4, {LavelOff, LavelOn},LavelSwf4,LavelSoftwareFilter4},
  {2, SWF5, {LavelOff, LavelOn},LavelSwf5,LavelSoftwareFilter5},
  {2, SWF6, {LavelOff, LavelOn},LavelSwf6,LavelSoftwareFilter6},
  {2, SWF7, {LavelOff, LavelOn},LavelSwf7,LavelSoftwareFilter7}, 
  // SWFxSU
  {2, SWF0, {LavelUnsigned, LavelSigned},LavelSwf0,LavelSignedUnsigned},
  {2, SWF1, {LavelUnsigned, LavelSigned},LavelSwf1,LavelSignedUnsigned},
  {2, SWF2, {LavelUnsigned, LavelSigned},LavelSwf2,LavelSignedUnsigned},
  {2, SWF3, {LavelUnsigned, LavelSigned},LavelSwf3,LavelSignedUnsigned},
  {2, SWF4, {LavelUnsigned, LavelSigned},LavelSwf4,LavelSignedUnsigned},
  {2, SWF5, {LavelUnsigned, LavelSigned},LavelSwf5,LavelSignedUnsigned},
  {2, SWF6, {LavelUnsigned, LavelSigned},LavelSwf6,LavelSignedUnsigned},
  {2, SWF7, {LavelUnsigned, LavelSigned},LavelSwf7,LavelSignedUnsigned},
  // COxSW
  {2, CO0, {LavelOff, LavelOn},LavelCo0,LavelCompareOut0},
  {2, CO1, {LavelOff, LavelOn},LavelCo1,LavelCompareOut1},
  {2, CO2, {LavelOff, LavelOn},LavelCo2,LavelCompareOut2},
  {2, CO3, {LavelOff, LavelOn},LavelCo3,LavelCompareOut3},
  {2, CO4, {LavelOff, LavelOn},LavelCo4,LavelCompareOut4},
  {2, CO5, {LavelOff, LavelOn},LavelCo5,LavelCompareOut5},
  // COxPOL
  {2, CO0, {LavelActive, LavelInactive},LavelCo0,LavelIfSwfmsg},
  {2, CO1, {LavelActive, LavelInactive},LavelCo1,LavelIfSwfmsg},
  {2, CO2, {LavelActive, LavelInactive},LavelCo2,LavelIfSwfmsg},
  {2, CO3, {LavelActive, LavelInactive},LavelCo3,LavelIfSwfmsg},
  {2, CO4, {LavelActive, LavelInactive},LavelCo4,LavelIfSwfmsg},
  {2, CO5, {LavelActive, LavelInactive},LavelCo5,LavelIfSwfmsg},
  // AOHSW, AOSSW, AOSBO
  {2, AO, {LavelOff, LavelOn},LavelAuxOut,"HWF out to AUX"},
  {2, AO, {LavelOff, LavelOn},LavelAuxOut,"SWF out to AUX"},
  {2, AO, {LavelBigEndian, LavelLittleEndian},LavelAuxOut,LavelByteOrder},
  // SLxSV
  {2, SL0, {LavelNo, LavelYes},LavelSl0,LavelSaveSettings},
  {2, SL1, {LavelNo, LavelYes},LavelSl1,LavelSaveSettings},
  {2, SL2, {LavelNo, LavelYes},LavelSl2,LavelSaveSettings},
  {2, SL3, {LavelNo, LavelYes},LavelSl3,LavelSaveSettings},
  {2, SL4, {LavelNo, LavelYes},LavelSl4,LavelSaveSettings},
  {2, SL5, {LavelNo, LavelYes},LavelSl5,LavelSaveSettings},
  {2, SL6, {LavelNo, LavelYes},LavelSl6,LavelSaveSettings},
  {2, SL7, {LavelNo, LavelYes},LavelSl7,LavelSaveSettings},
  // SLxLD
  {2, SL0, {LavelNo, LavelYes},LavelSl0,LavelLoadSettings},
  {2, SL1, {LavelNo, LavelYes},LavelSl1,LavelLoadSettings},
  {2, SL2, {LavelNo, LavelYes},LavelSl2,LavelLoadSettings},
  {2, SL3, {LavelNo, LavelYes},LavelSl3,LavelLoadSettings},
  {2, SL4, {LavelNo, LavelYes},LavelSl4,LavelLoadSettings},
  {2, SL5, {LavelNo, LavelYes},LavelSl5,LavelLoadSettings},
  {2, SL6, {LavelNo, LavelYes},LavelSl6,LavelLoadSettings},
  {2, SL7, {LavelNo, LavelYes},LavelSl7,LavelLoadSettings},
  // OPxxxx
  {2, OP, {"C:cancel,E:enter", "C:enter,E:cancel"}, LavelOption, LavelSwapCE},
};

// ボタン数を返すインタフェース 
int getButtonCount(int pageIndex){
  return pageButton8[pageIndex].buttonCount;
}

// Value page type *******************************
// ページの情報 enum ePageと１対１対応
const pageInfo_value pageValue[] = {
  // HWFx
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Mask0", "Mask0 maskbit val"}, // HWF0
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Filt0", "Filter0 for Mask0"}, // HWF1
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Filt1", "Filter1 for Mask0"}, // HWF2
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Mask1", "Mask1 maskbit val"}, // HWF3
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Filt2", "Filter2 for Mask1"}, // HWF4
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Filt3", "Filter3 for Mask1"}, // HWF5
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Filt4", "Filter4 for Mask1"}, // HWF6
  {HWF, CANEXTIDDIGIT, VALUEISLIMITED, 0, CANEXTIDNUMMAX, "HWF Filt5", "Filter5 for Mask1"}, // HWF7
  // SWFxID
  {SWF0, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf0, LavelFiltCanId},
  {SWF1, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf1, LavelFiltCanId},
  {SWF2, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf2, LavelFiltCanId},
  {SWF3, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf3, LavelFiltCanId},
  {SWF4, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf4, LavelFiltCanId},
  {SWF5, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf5, LavelFiltCanId},
  {SWF6, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf6, LavelFiltCanId},
  {SWF7, 8, VALUEISLIMITED, 0, CANEXTIDNUMMAX, LavelSwf7, LavelFiltCanId},
  // SWFxSB
  {SWF0, 1, VALUEISLIMITED, 0, 7, LavelSwf0, LavelStartBytePos},
  {SWF1, 1, VALUEISLIMITED, 0, 7, LavelSwf1, LavelStartBytePos},
  {SWF2, 1, VALUEISLIMITED, 0, 7, LavelSwf2, LavelStartBytePos},
  {SWF3, 1, VALUEISLIMITED, 0, 7, LavelSwf3, LavelStartBytePos},
  {SWF4, 1, VALUEISLIMITED, 0, 7, LavelSwf4, LavelStartBytePos},
  {SWF5, 1, VALUEISLIMITED, 0, 7, LavelSwf5, LavelStartBytePos},
  {SWF6, 1, VALUEISLIMITED, 0, 7, LavelSwf6, LavelStartBytePos},
  {SWF7, 1, VALUEISLIMITED, 0, 7, LavelSwf7, LavelStartBytePos},
  // SWFxSI
  {SWF0, 1, VALUEISLIMITED, 0, 7, LavelSwf0, LavelStartBitPos},
  {SWF1, 1, VALUEISLIMITED, 0, 7, LavelSwf1, LavelStartBitPos},
  {SWF2, 1, VALUEISLIMITED, 0, 7, LavelSwf2, LavelStartBitPos},
  {SWF3, 1, VALUEISLIMITED, 0, 7, LavelSwf3, LavelStartBitPos},
  {SWF4, 1, VALUEISLIMITED, 0, 7, LavelSwf4, LavelStartBitPos},
  {SWF5, 1, VALUEISLIMITED, 0, 7, LavelSwf5, LavelStartBitPos},
  {SWF6, 1, VALUEISLIMITED, 0, 7, LavelSwf6, LavelStartBitPos},
  {SWF7, 1, VALUEISLIMITED, 0, 7, LavelSwf7, LavelStartBitPos},
  // SWFxEB
  {SWF0, 1, VALUEISLIMITED, 0, 7, LavelSwf0, LavelEndBytePos},
  {SWF1, 1, VALUEISLIMITED, 0, 7, LavelSwf1, LavelEndBytePos},
  {SWF2, 1, VALUEISLIMITED, 0, 7, LavelSwf2, LavelEndBytePos},
  {SWF3, 1, VALUEISLIMITED, 0, 7, LavelSwf3, LavelEndBytePos},
  {SWF4, 1, VALUEISLIMITED, 0, 7, LavelSwf4, LavelEndBytePos},
  {SWF5, 1, VALUEISLIMITED, 0, 7, LavelSwf5, LavelEndBytePos},
  {SWF6, 1, VALUEISLIMITED, 0, 7, LavelSwf6, LavelEndBytePos},
  {SWF7, 1, VALUEISLIMITED, 0, 7, LavelSwf7, LavelEndBytePos},
  // SWFxEI
  {SWF0, 1, VALUEISLIMITED, 0, 7, LavelSwf0, LavelEndBitPos},
  {SWF1, 1, VALUEISLIMITED, 0, 7, LavelSwf1, LavelEndBitPos},
  {SWF2, 1, VALUEISLIMITED, 0, 7, LavelSwf2, LavelEndBitPos},
  {SWF3, 1, VALUEISLIMITED, 0, 7, LavelSwf3, LavelEndBitPos},
  {SWF4, 1, VALUEISLIMITED, 0, 7, LavelSwf4, LavelEndBitPos},
  {SWF5, 1, VALUEISLIMITED, 0, 7, LavelSwf5, LavelEndBitPos},
  {SWF6, 1, VALUEISLIMITED, 0, 7, LavelSwf6, LavelEndBitPos},
  {SWF7, 1, VALUEISLIMITED, 0, 7, LavelSwf7, LavelEndBitPos},
  // COxUSF
  {CO0, 1, VALUEISLIMITED, 0, 7, LavelCo0, LavelUsingSwfNo}, 
  {CO1, 1, VALUEISLIMITED, 0, 7, LavelCo1, LavelUsingSwfNo}, 
  {CO2, 1, VALUEISLIMITED, 0, 7, LavelCo2, LavelUsingSwfNo}, 
  {CO3, 1, VALUEISLIMITED, 0, 7, LavelCo3, LavelUsingSwfNo}, 
  {CO4, 1, VALUEISLIMITED, 0, 7, LavelCo4, LavelUsingSwfNo}, 
  {CO5, 1, VALUEISLIMITED, 0, 7, LavelCo5, LavelUsingSwfNo}, 
  // COxTRS
  {CO0, 8, VALUEISANY, 0, 0, LavelCo0, LavelThreshould},
  {CO1, 8, VALUEISANY, 0, 0, LavelCo1, LavelThreshould},
  {CO2, 8, VALUEISANY, 0, 0, LavelCo2, LavelThreshould},
  {CO3, 8, VALUEISANY, 0, 0, LavelCo3, LavelThreshould},
  {CO4, 8, VALUEISANY, 0, 0, LavelCo4, LavelThreshould},
  {CO5, 8, VALUEISANY, 0, 0, LavelCo5, LavelThreshould},
};

// 値のmax,min,isSignedを返すインタフェース
int32_t getValueMax(int pageIndex){
  return pageValue[pageIndex].maxNumber;
}
int32_t getValueMin(int pageIndex){
  return pageValue[pageIndex].minNumber;
}
bool getValueIsAny(int pageIndex){
  return pageValue[pageIndex].isAnyVal;
}

// CAN Mask and Filter setting screen table
const canMaskFilterTable canMFtable[HWFMENUCOUNT] = {
  {F_MFTMASK, 0, CANMASKVAL0}, {F_MFTFILTER, 0, CANFILTERVAL0},
  {F_MFTFILTER, 1, CANFILTERVAL1}, {F_MFTMASK, 1, CANMASKVAL1},
  {F_MFTFILTER, 2, CANFILTERVAL2}, {F_MFTFILTER, 3, CANFILTERVAL3},
  {F_MFTFILTER, 4, CANFILTERVAL4}, {F_MFTFILTER, 5, CANFILTERVAL5}
};



// Screen Page *********************************************************************************

// 画面ページから画面の種類に変換する関数
ePageType page2pageType(ePage page){
  ePageType pageType = ERROR;
  if(page >= PAGEMAX); // pageType = ERROR
  else if(page > VALUE_TYPE) pageType = VALUE;
  else if(page > BUTTON_TYPE && page < VALUE_TYPE) pageType = BUTTON8;
  else if(page > LIST_TYPE && page < BUTTON_TYPE) pageType = LIST8;
  else if(page > MONITOR_TYPE && page < LIST_TYPE) pageType = MONITOR;
  // error check
  if(pageType == ERROR){  // ERROR check
    DEBUG_PRINT("page2pagetype ERROR page=");DEBUG_PRINTLN(page);
  }
  return pageType;
}

// 画面ページからPageTypeの配列番号に変換する
int page2pageIndex(ePage page){
  int retval = ERROR;
  switch(page2pageType(page)){
    case VALUE: return page - (VALUE_TYPE + 1);
    case BUTTON8: return page - (BUTTON_TYPE + 1);
    case LIST8: return page - (LIST_TYPE + 1);
    case MONITOR: return page - (MONITOR_TYPE + 1);
    case ERROR: default: ;
  }
  return retval;  // return ERROR
}

// 画面ページからPageType情報の配列番号と、設定データ格納先情報とその配列番号に変換する
void page2typeIndexes(ePage page, int &pageIndex, eDeviceSettingRegType &regType, int &regIndex){
  // making the page type index
  pageIndex = page2pageIndex(page);
  // making the device setting reg type and the reg index
  switch(page2pageType(page)){
    case VALUE:
      if(page >= CO0TRS){regType = COTRS; regIndex = page - CO0TRS;}
      else if(page >= CO0USF){regType = COUSF; regIndex = page - CO0USF;}
      else if(page >= SWF0EI){regType = SWFEI; regIndex = page - SWF0EI;}
      else if(page >= SWF0EB){regType = SWFEB; regIndex = page - SWF0EB;}
      else if(page >= SWF0SI){regType = SWFSI; regIndex = page - SWF0SI;}
      else if(page >= SWF0SB){regType = SWFSB; regIndex = page - SWF0SB;}
      else if(page >= SWF0ID){regType = SWFID; regIndex = page - SWF0ID;}
      else {regType = DS_HWF; regIndex = page - HWF0;}
      return;
    case BUTTON8:
      if(page >= OPSMCE){regType = DS_OPSM; regIndex = page - OPSMCE;}
      else if(page >= SL0LD){regType = SLLD; regIndex = page - SL0LD;}
      else if(page >= SL0SV){regType = SLSV; regIndex = page - SL0SV;}
      else if(page >= AOHSW){regType = AOSET; regIndex = page - AOHSW;}
      else if(page >= CO0POL){regType = COPOL; regIndex = page - CO0POL;}
      else if(page >= CO0SW){regType = COSW; regIndex = page - CO0SW;}
      else if(page >= SWF0SU){regType = SWFSU; regIndex = page - SWF0SU;}
      else if(page >= SWF0SW){regType = SWFSW; regIndex = page - SWF0SW;}
      else if(page >= HWFF0L){regType = HWFFL; regIndex = page - HWFF0L;}
      else {regType = CANSPEED; regIndex = 0;}
      return;
    case LIST8:
      regType = DSRT_ERROR;
      regIndex = ERROR_GENERAL;
      return;
    case MONITOR:
      regType = DSRT_ERROR;
      regIndex = ERROR_GENERAL;
      return;
    case ERROR: default:
      break;
  }
  // ERROR
  DEBUG_PRINT("page2typeIndexes ERROR page=");DEBUG_PRINTLN(page);
}

// Display class ********************************************************************************
// 画面描画クラスの定義
Display::Display(Adafruit_ILI9341* tft, SettingsManager* setMan, int width, int height,
                  int titleH, int listItemH)
  : tft_(tft), setMan_(setMan), tftWidth(width), tftHeight(height), titleHeight(titleH){
    // make CAN voltage object
    objVcan_ = new BoxObject(tft_, boxOI_canVolgate, boxOC_textOnly, " 0.0");
    // make icon objects
    iconHF_ = new BoxObject(tft_, boxOI_iconHF, boxOC_iconDefault, "HF");
    iconHD_ = new BoxObject(tft_, boxOI_iconHD, boxOC_iconDefault, "HD");
    iconHA_ = new BoxObject(tft_, boxOI_iconHA, boxOC_iconDefault, "HA");
    iconSF_ = new BoxObject(tft_, boxOI_iconSF, boxOC_iconDefault, "SF");
    iconSD_ = new BoxObject(tft_, boxOI_iconSD, boxOC_iconDefault, "SD");
    iconSA_ = new BoxObject(tft_, boxOI_iconSA, boxOC_iconDefault, "SA");
    iconCD_ = new BoxObject(tft_, boxOI_iconCD, boxOC_iconDefault, "CD");
    // clear cursor pos
    for(int i = 0; i < VALUE_TYPE; i++){
      previousCursorPos_[i] = 0;
    }
};

// set CAN Mask/Filter flag for HWF menu
void Display::setCanMFtable(const canMaskFilterTable *pCanMaskFilterTable){
  pCanMaskFilterTable_ = pCanMaskFilterTable;
}

// clear screen
void Display::clearScreen(uint16_t color){
  tft_->fillScreen(color);
  tft_->setRotation(0);
  tft_->setScrollMargins(8,0);
}

// initial bitmap
const uint8_t initBitmapSize[2] = {48, 46}; // x, y [px]
const uint8_t initBitmap[] PROGMEM = {
	// 'whiteのコピー, 48x46px
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xf8, 0x7f, 0xff, 0xff, 0xfc, 0x7f, 0xc0, 
	0x7f, 0xff, 0xff, 0xf9, 0xff, 0x00, 0x7f, 0xff, 0xff, 0xf3, 0xfc, 0x18, 0xff, 0xff, 0xff, 0xe7, 
	0xf0, 0x78, 0xff, 0xff, 0xff, 0xcf, 0xc1, 0xf1, 0xff, 0xff, 0xff, 0x9f, 0x07, 0xf3, 0xff, 0x3f, 
	0xff, 0x3c, 0x1f, 0xe3, 0xf8, 0x1f, 0xfe, 0x70, 0x7f, 0xc7, 0xe0, 0x1f, 0xfc, 0xc1, 0xff, 0xc7, 
	0x01, 0x3f, 0xf8, 0x07, 0xff, 0xc0, 0x0f, 0x3f, 0xf8, 0x3f, 0xff, 0xc0, 0x3e, 0x3f, 0xf8, 0xff, 
	0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 
	0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf0, 
	0x7f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 
	0xff, 0xf8, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x39, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff
};
void Display::showInitialScreen() {
  clearScreen(ILI9341_BLACK);
  tft_->drawBitmap(ILI9341_TFTWIDTH / 2 - initBitmapSize[0] / 2,    // calc x center position
                    ILI9341_TFTHEIGHT / 2 - initBitmapSize[1] / 2,  // calc y center position
                    initBitmap, initBitmapSize[0], initBitmapSize[1], ILI9341_CYAN);
}

// set text color
void Display::setTextColor(uint16_t color){
  if(color_ != color){
    DEBUG2_PRINTLN("changed text color");
    tft_->setTextColor(color);
    color_ = color;
  }
}

// home cursor
void Display::homeCursor(){
  tft_->setTextSize(1);
  tft_->setCursor(0, 0);
  tft_->scrollTo(0);
  cln = pcln = 0;   // reset Cursor Line Number and g-ram Pointer for write1Line()
}

// draw CAN Voltage in the status line
void Display::drawCANvoltage(String strVcan){
  if(pageType_ == MONITOR){
    setTextColor(statusColor_);           // set text color
    objVcan_->changeText(strVcan);
    tft_->setCursor(0, A_ROWSIZE * pcln); // set back GRAM row address
  }
}

// Write new Status line
void Display::writeStatusLine(uint16_t color = STATUSLINE_TEXTCOLOR){
  statusColor_ = color;
  // erace previous text
  tft_->fillRect(0, 0, ILI9341_TFTWIDTH, A_ROWSIZE, STATUSLINE_BACKGROUNDCOLOR);
  // draw status line
  homeCursor();                 // cursor go home and fontsize = 1
  setTextColor(statusColor_);   // set text color
  tft_->println(StatusLine);    // draw status line text
  drawStatusIconAll();          // draw status icons
  objVcan_->draw(FLAG_DRAW);    // draw CAN voltage
  // set cursor position
  cln = pcln = 1;
  tft_->setCursor(0, A_ROWSIZE * pcln); // set back GRAM row address
}

// convert iconType -> iconObject
BoxObject* Display::iconType2obj(eStatuLineIcon iconType){
  // アイコンを表示非表示する
  switch(iconType){
    case SLI_HF:  return iconHF_;
    case SLI_HD:  return iconHD_;
    case SLI_HA:  return iconHA_;
    case SLI_SF:  return iconSF_;
    case SLI_SD:  return iconSD_;
    case SLI_SA:  return iconSA_;
    case SLI_CP:  return iconCD_;
    default: DEBUG_PRINTLN("Error! setStatusIconSw iconType"); break;
  }
  return nullptr;
}

// Set status line icon on/off  offOn=0:OFF,1:ON
void Display::setStatusIconSw(eStatuLineIcon iconType, bool offOn){
  statuIconSw_[iconType] = offOn;
}
void Display::drawStatusIcon(eStatuLineIcon iconType, bool offOn){
  // 表示フラグが変更されたときだけ描画
  if(statuIconSw_[iconType] != offOn){
    BoxObject* obj = iconType2obj(iconType);
    obj->draw(!offOn);
    statuIconSw_[iconType] = offOn;   // write back the flag
  }
  tft_->setTextColor(color_);         // reset color setting
}

// draw status line icons all
void Display::drawStatusIconAll(){
  BoxObject* obj;
  for(int i = 0; i < SLIMAX; i++){
    obj = iconType2obj(static_cast<eStatuLineIcon>(i));
    obj->draw(!statuIconSw_[i]);
  }
  tft_->setTextColor(color_);         // reset color setting
}

// reset monitor mode
void Display::resetMonitorScrollType(){
  monitorScrollType_.fMonitorScrollSw_ = true;
  monitorScrollType_.fMonitorDispSw_.bit.hwfDisp = true;
  monitorScrollType_.fMonitorDispSw_.bit.swfDisp = true;
}
// get monitor scroll type
monitorScrollType Display::getMonitorScrollType(){
  return monitorScrollType_;
}

// Show 1 line with scrolling
void Display::write1Line(String* s, uint16_t color){
  // set text color
  setTextColor(color);
  // clear first line and scroll if written area is over
  if(cln >= CURSORROWNUM){
    cln = CURSORROWNUM - 1;
    // Come back the pcln val if GRAM pointer pcln is over
    if(pcln >= CURSORROWNUM){
      tft_->setCursor(0, A_ROWSIZE * SCROLLMARGINLN); // set GRAM row address
      pcln = SCROLLMARGINLN;
    }
    // erace previous text
    tft_->fillRect(0, pcln * A_ROWSIZE, ILI9341_TFTWIDTH, A_ROWSIZE, ILI9341_BLACK);    // 6.5ms on SAMD21
    // scrolling in advance
    tft_->scrollTo((pcln + 1) * A_ROWSIZE);
  }
  // print with linefeed for moving GRAM row address
  tft_->println(*s);     // 3-10ms on SAMD21
  // calc cursor inclement for multiple lines
  int clninc = (s->length() + CURSORCOLNUM - 1) / CURSORCOLNUM;
  cln += clninc;
  pcln += clninc;
}

// post Line if mode is run mode
// モニタモード且つCAN受信した時に呼ばれる
void Display::postLine(String& s, uint16_t color){
  // fMonitorScrollSw_がTrueの時はスクロール表示、falseの時は表示停止
  if(monitorScrollType_.fMonitorScrollSw_){
    write1Line(&s, color);           // display CAN data screenの1ライン表示処理
  }
}

// check if both previous mode and current mode are monitor mode
// 前のページも今のページもモニターモードの場合Trueを返す
bool Display::isMonitorMode(){
  if(pageType_ == MONITOR && page2pageType(newPage_) == MONITOR) return true;
  else return false;
}

// check if the display page is changed
bool Display::isPageChanged(){
  if(currentPage_ != newPage_) return true;
  else return false;
}

// check if the display page went back to monitor mode from other
bool Display::isPageBackToMonitorMode(){
  if(pageType_ != MONITOR && page2pageType(newPage_) == MONITOR) {
    return true;
  }
  else return false;
}

// change page
void Display::changePage(){
  // erase previous page
  DEBUG2_PRINTLN("CP0_erase previous page");
  switch(pageType_){
    case MONITOR:
      color_ = BACKGROUNDCOLOR;
      clearScreen(color_);
    break;
    default:
      if(page != nullptr){
        delete page;
        page = nullptr;
      }
    break;
  }
  // draw new page
  DEBUG2_PRINTLN("CP1_draw new page");
  drawPage();
  // update the page reference
  currentPage_ = newPage_;
}

// calc digit count for Value type
void Display::calcRepeatCountOfDigit(){
  // max digit is set before calc
  repeatCount_ = pageValue[pageIndex_].digitCount;
  // limit the repeatCount(digit) when newPage_ is CO threshould
  bool fext = false;
  int i;
  switch(regType_){
    case COTRS: // CO threshould value
      // set value byte length and bit length
      extractLen(setMan_, setMan_->getSettingValue(COUSF, regIndex_), &byteLen_, &bitLen_);
      // 1桁16進表示のためbit長を4bitsで割って余りを繰り上げ
      repeatCount_ = (bitLen_ + 3) / 4; // repeatCount_: digit count
      break;
    case DS_HWF:
      // Mask/FilterがFilter且つそのFilter番号におけるIDlengthがSTDのとき、桁数を制限
      if(pCanMaskFilterTable_[pageIndex_].fMaskFilter &&
        setMan_->getSettingValue(HWFFL, pCanMaskFilterTable_[pageIndex_].num) == CANFLSTD){
        // standard case: limited digits
        repeatCount_ = CANSTDIDDIGIT;
      }
    case SWFID:
      // すべてのFilterのIDlengthがstdか調べる
      for(i = 0; i < CANFILTERCOUNT; i++){
        if(setMan_->getSettingValue(HWFFL, i)) fext = true;
      }
      if(!fext){  // If all filters are standard case: limited digits
        repeatCount_ = CANSTDIDDIGIT;
      }
    default:
      break;
  }
}

// set limiting values to private regs
void Display::setLimitValue(){
  uint32_t max;
  int32_t min;

  valueIsAny_ = getValueIsAny(pageIndex_);
  if(valueIsAny_){    // COTRS
    valueIsSign_ = setMan_->getSettingValue(SWFSU, setMan_->getSettingValue(COUSF, regIndex_));
    max = powInt(2, bitLen_) - 1;
    min = 0;
    if(valueIsSign_){
      max = max / 2;
      min = -(int32_t)max - 1;
    }
    valueMax_ = max;
    valueMin_ = (uint32_t)min;
  }
  else if(regType_ == DS_HWF &&  // Filter番号におけるIDlengthがSTDのとき、数を制限
          setMan_->getSettingValue(HWFFL, pCanMaskFilterTable_[pageIndex_].num) == CANFLSTD){
    // standard case: limited digits
    valueMax_ = CANSTDIDNUMMAX;
    valueMin_ = 0;
  }
  else{
    valueMax_ = getValueMax(pageIndex_);
    valueMin_ = getValueMin(pageIndex_);
  }
  DEBUG_PRINT("valueMax_,Min_=");DEBUG_PRINT(valueMax_);DEBUG_PRINT(", ");DEBUG_PRINTLN(valueMin_);
}

// get the different type value from setman
uint32_t Display::getCurrentValue_fromSetman(){
  if(valueIsAny_) return setMan_->getSettingAnyvalue(regType_, regIndex_);
  else return (uint32_t)(setMan_->getSettingValue(regType_, regIndex_));
}

// set the different type value to setman
void Display::setNewValue_toSetman(){
  if(valueIsAny_) setMan_->setSettingValue((uint64_t)newValue_, regType_, pageIndex_, regIndex_);
  else setMan_->setSettingValue((int32_t)newValue_, regType_, pageIndex_, regIndex_);
}

// SWF0-7でStartByte/BitとStopByteBit位置がおかしいとエラー表示
void Display::noticeByteBitPositionError(int swfIndex){
  uint8_t startByte = setMan_->getSettingValue(SWFSB, swfIndex);
  uint8_t startBit = setMan_->getSettingValue(SWFSI, swfIndex);
  uint8_t endByte = setMan_->getSettingValue(SWFEB, swfIndex);
  uint8_t endBit = setMan_->getSettingValue(SWFEI, swfIndex);
  if (startByte > endByte){
    DEBUG2_PRINTLN("SWF0-7 Stt>End ByteErr!");
    showNotice(NOTICETIME3, boxOI_noticeSmall, boxOC_notice, "Stt>End ByteErr!"); // 画面にNotice表示
  }
  else if(startByte == endByte && startBit < endBit){
    DEBUG2_PRINTLN("SWF0-7 Stt<End BitErr!");
    showNotice(NOTICETIME3, boxOI_noticeSmall, boxOC_notice, "Stt<End BitErr!");  // 画面にNotice表示
  }
  else;   // no error
}

// draw page
void Display::drawPage(){
  int objectCount = 0, cnt = 0;
  // check and new page pointer
  if(page != nullptr)  delete page;
  page = new Page();
  homeCursor();
  // set page type and index
  pageType_ = page2pageType(newPage_);                                // 画面ページタイプを更新
  page2typeIndexes(newPage_, pageIndex_, regType_, regIndex_);        // その他ページ情報を更新
  DEBUG2_PRINT("DP0_drawPage currentPage_,newPage_,PageType,pageIndex_,regType_,regIndex_= ");
  DEBUG2_PRINT(currentPage_);DEBUG2_PRINT(", ");DEBUG2_PRINT(newPage_);DEBUG2_PRINT(", ");
  DEBUG2_PRINT(pageType_);DEBUG2_PRINT(", ");DEBUG2_PRINT(pageIndex_);DEBUG2_PRINT(", ");
  DEBUG2_PRINT(regType_);DEBUG2_PRINT(", ");DEBUG2_PRINTLN(regIndex_);
  // draw the page according to pageType
  switch(pageType_){
    case MONITOR:
      writeStatusLine(STATUSLINE_TEXTCOLOR);
      resetMonitorScrollType();
      break;
    case LIST8:
      objectCount = sizeof(pageObjList_list8) / sizeof(pageObjList_list8[0]);
      repeatCount_ = pageList8[pageIndex_].listCount;
      cursorPos_ = previousCursorPos_[newPage_];  // load cursor position
      if(cursorPos_ >= repeatCount_) cursorPos_ = 0;  // cursorPos_ limitation
      page->addObject(POAT_FIXED, new TextObject(tft_, textOI_topTitle, pageList8[pageIndex_].topTitle));
      page->addObject(POAT_FIXED, new TextObject(tft_, textOI_subTitle, pageList8[pageIndex_].subTitle));
      for(int i = 0; i < objectCount; i++){
        switch(pageObjList_list8[i]){
          case BUTTON_BACK:
            page->addObject(POAT_FIXED, new BoxObject(tft_, boxOI_back, boxOC_default, "BACK"));
            break;
          case LIST_1LINE:
            if(cnt < repeatCount_){
              page->addObject(POAT_FIXED, new ListObject(tft_, boxOI_list8[cnt], boxOC_default, 
                              pageList8[pageIndex_].listLabels[cnt]));
              cnt++;
            }
            break;
          case CURSOR:
            page->addObject(POAT_CURSOR, new CursorObject(tft_, cursorOI_list, cursorOC_default, cursorPos_));
            break;
        }
      }
      page->draw();
      // SWF0-7でStartByte/BitとStopByteBit位置がおかしいとエラー表示
      if(newPage_ >= SWF0 && newPage_ <= SWF7) noticeByteBitPositionError((int)(newPage_ - SWF0));
      break;
    case BUTTON8:
      objectCount = sizeof(pageObjList_buttonBox8) / sizeof(pageObjList_buttonBox8[0]);
      repeatCount_ = pageButton8[pageIndex_].buttonCount;
      cursorPos_ = previousCursorPos_[newPage_];  // cursorPos_ starts prev pos
      if(cursorPos_ >= repeatCount_) cursorPos_ = 0;  // cursorPos_ limitation
      page->addObject(POAT_FIXED, new TextObject(tft_, textOI_topTitle, pageButton8[pageIndex_].topTitle));
      page->addObject(POAT_FIXED, new TextObject(tft_, textOI_subTitle, pageButton8[pageIndex_].subTitle));
      for(int i = 0; i < objectCount; i++){
        switch(pageObjList_buttonBox8[i]){
          case BUTTON_BACK:
            page->addObject(POAT_FIXED, new BoxObject(tft_, boxOI_back, boxOC_default, "BACK"));
            break;
          case BUTTONLIST_1LINE:
            if(cnt < repeatCount_){
              page->addObject(POAT_MUTABLE, new ButtonListObject(tft_, boxOI_buttonBox8[cnt], boxOC_default, 
                              buttonOI_default, buttonOC_default, pageButton8[pageIndex_].buttonLabels[cnt]));
              cnt++;
            }
            break;
          case CURSOR:
            page->addObject(POAT_CURSOR, new CursorObject(tft_, cursorOI_list, cursorOC_default, cursorPos_));
            break;
        }
      }
      page->draw();
      setButtonToButtonListObj();
      break;
    case VALUE:
      objectCount = sizeof(pageObjList_value) / sizeof(pageObjList_value[0]);
      calcRepeatCountOfDigit();       // calc repeatCount_
      setLimitValue();                // set limiting values to private regs
      cursorPos_ = 0;
      page->addObject(POAT_FIXED, new TextObject(tft_, textOI_topTitle, pageValue[pageIndex_].topTitle));
      page->addObject(POAT_FIXED, new TextObject(tft_, textOI_subTitle, pageValue[pageIndex_].subTitle));
      for(int i = 0; i < objectCount; i++){
        switch(pageObjList_value[i]){
          case BUTTON_BACK:
            page->addObject(POAT_FIXED, new BoxObject(tft_, boxOI_back, boxOC_default, "BACK"));
            break;
          case TRIANGLE_UP:
            page->addObject(POAT_FIXED, new TriangleObject(tft_, boxOI_valueUp, boxOC_default, triangleOI_valueUp));
            break;
          case VALUE_1DIGIT:
            if(cnt < repeatCount_){
              page->addObject(POAT_MUTABLE, new BoxObject(tft_, boxOI_value[cnt], boxOC_default, ""));
              cnt++;
            }
            break;
          case CURSOR:
            page->addObject(POAT_CURSOR, new CursorObject(tft_, cursorOI_value, cursorOC_default, cursorPos_));
            break;
          case TRIANGLE_DOWN:
            page->addObject(POAT_FIXED, new TriangleObject(tft_, boxOI_valueDown, boxOC_default, triangleOI_valueDown));
            break;
          case BUTTON_CANCEL:
            page->addObject(POAT_FIXED, new BoxObject(tft_, boxOI_cancel, boxOC_cancel, "CANCEL"));
            break;
          case BUTTON_ENTER:
            page->addObject(POAT_FIXED, new BoxObject(tft_, boxOI_enter, boxOC_enter, "ENTER"));
            break;
        }
      }
      page->draw();
      // 今の設定値をval表示桁数分表示
      currentValue_ = getCurrentValue_fromSetman(); // 設定値をcurrentValue_に読み込む
      num2hexToScreenValObj(currentValue_);                         // valueをrepeatCount桁分表示更新
      newValue_ = currentValue_;                                    // 更新値を現在値で初期化
      break;
    default:
      DEBUG_PRINTLN("DP1 case:ERROR");
      break;
  }
}

// 今の設定値をボタンに表示
void Display::setButtonToButtonListObj(){
  int32_t val;
  // 設定値をvalに読み込む
  val = setMan_->getSettingValue(regType_, regIndex_);
  // ボタンを押された部分の表示を更新  // set new setting
  ButtonListObject *obj;
  if(checkObject_ButtonList(val, obj)) obj->set(); // 型確認＆ボタンセット
  else DEBUG_PRINTLN("Error: Display::touched cnt object type"); // 型エラー
}

// uint32をhexに変換し画面のValueのBoxObjを表示更新
void Display::num2hexToScreenValObj(uint32_t val){
  // 16進数に変換して文字列にする
  char hexStr[VALUEDIGITMAX + 1];   // 桁数+null文字分
  sprintf(hexStr, HEXDIGIT8, val);
  // 各文字をBoxオブジェクトに表示
  BoxObject *obj;
  for(int i = 0; i < repeatCount_; i++){   //桁数分繰り返し値を表示
    // 型確認＆Valueセット
    if(checkObject_Box(i, obj)) obj->changeText(String(hexStr[VALUEDIGITMAX - 1 - i])); // 桁順序反転
    else DEBUG_PRINTLN("Error: num2hexToScreenValObj object type"); // 型エラー
  }
}

// set newPage directly
void Display::setNewpageDirectly(ePage page){
  newPage_ = page;
  DEBUG2_PRINT("NPD0 newPage= ");DEBUG2_PRINTLN(newPage_);
}

// check touched BACK button for touch processing
bool Display::checkTouchedBox(int tx, int ty, const int16_t *boxPos){
  if(ty >= boxPos[POS_Y0] && ty <= boxPos[POS_Y1] && tx >= boxPos[POS_X0] && tx <= boxPos[POS_X1]) return true;
  else return false;
}
bool Display::checkTouchedBack(int tx, int ty){
  return checkTouchedBox(tx, ty, boxOI_back.boxPos);
}

// Mutableオブジェクトの型確認＆手動キャスト
bool Display::checkObject_ButtonList(int mutableObjPos, ButtonListObject *&obj){
  if (page->getMutableObj(mutableObjPos) && page->getMutableObj(mutableObjPos)->isButtonListObject()) {
    obj = static_cast<ButtonListObject*>(page->getMutableObj(mutableObjPos));  // 手動キャスト
    return true;
  }
  return false;
}
bool Display::checkObject_Box(int mutableObjPos, BoxObject *&obj){
  if (page->getMutableObj(mutableObjPos) && page->getMutableObj(mutableObjPos)->isBoxObject()) {
    obj = static_cast<BoxObject*>(page->getMutableObj(mutableObjPos));  // 手動キャスト
    return true;
  }
  return false;
}
bool Display::checkObject_Cursor(CursorObject *&obj){
  if (page->getCursorObj() && page->getCursorObj()->isCursorObject()) {
    obj = static_cast<CursorObject*>(page->getCursorObj());  // 手動キャスト
    return true;
  }
  return false;
}

void Display::showNotice(unsigned long time, const boxObjectInfo &info, 
                          const boxObjectColor &color, const char* str){
  BoxObject *obj = new BoxObject(tft_, info, color, str);
  obj->draw(FLAG_DRAW);
  delay(time);
  obj->draw(FLAG_ERASE);
  delete obj;
}

// ボタンリストのボタン位置を変更し設定値を変更
void Display::changeButtonPos(int newPos){
  // 画面ページから設定データ格納先情報に変換する
  int32_t val;
  // 前の設定値をvalに読み込む
  val = setMan_->getSettingValue(regType_, regIndex_);
  // ボタンを押された部分の表示を更新
  // delete prebious setting
  ButtonListObject *obj;
  if(checkObject_ButtonList(val, obj)) obj->reset(); // 型確認＆前のボタン消去
  else DEBUG_PRINTLN("Error: Display::touched val object type"); // 型エラー
  // set new setting
  if(checkObject_ButtonList(newPos, obj)) obj->set(); // 型確認＆押されたボタンセット
  else DEBUG_PRINTLN("Error: Display::touched newPos object type"); // 型エラー
  // 新しい設定値をセット
  setMan_->setSettingValue((int32_t)newPos, regType_, pageIndex_, regIndex_);
}

int powInt(int base, int exponent) {
  int result = 1;
  for (int i = 0; i < exponent; ++i) {
      result *= base;
  }
  return result;
}

// Increment digit value
void Display::incrementValue(){
  // check if cursor pos is on a value box
  if(cursorPos_ >= repeatCount_ || cursorPos_ < 0) return;
  // Increment value
  if(valueIsAny_ && valueIsSign_){    // handle if value has sign for COTRS
    int64_t val = (int32_t)newValue_;
    val += powInt(16, cursorPos_);
    // check range and limitation
    if(val <= (int32_t)valueMax_) newValue_ = (uint32_t)val;
    else  newValue_ = valueMax_;
  }
  else{
    int64_t val = newValue_;
    val += powInt(16, cursorPos_);
    // check range and limitation
    if(val <= valueMax_) newValue_ = (uint32_t)val;
    else  newValue_ = valueMax_;
  }
  DEBUG_PRINT("UP newValue_= ");DEBUG_PRINTLN(newValue_);
  // uint32をhexに変換し画面のValueのBoxObjを表示更新
  num2hexToScreenValObj(newValue_);
}

// Decriment digit value
void Display::decrementValue(){
  // check if cursor pos is on a value box
  if(cursorPos_ >= repeatCount_ || cursorPos_ < 0) return;
  // Decrement value
  if(valueIsAny_ && valueIsSign_){    // handle if value has sign for COTRS 
    int64_t val = (int32_t)newValue_;
    val -= powInt(16, cursorPos_);
    // check range and limitation
    if(val >= (int32_t)valueMin_) newValue_ = (uint32_t)val;
    else  newValue_ = valueMin_;
  }
  else{
    int64_t val = newValue_;
    val -= powInt(16, cursorPos_);
    // check range and limitation
    if(val >= valueMin_) newValue_ = (uint32_t)val;
    else  newValue_ = valueMin_;
  }
  DEBUG_PRINT("DN newValue_= ");DEBUG_PRINTLN(newValue_);
  // uint32をhexに変換し画面のValueのBoxObjを表示更新
  num2hexToScreenValObj(newValue_);
}

// move cursor position
void Display::moveCursorPosition(int pos){
  CursorObject *obj;
  if(checkObject_Cursor(obj)) obj->movePos(pos); // 型確認＆カーソルセット
  else DEBUG_PRINTLN("Error: Display::keyControl object type"); // 型エラー
}

// touch processing
// retval= fassigned= the flag if the touched pos has assigned for some function
bool Display::touched(int tx, int ty){
  int cnt;
  bool fassigned = true;
  DEBUG2_PRINTLN("DT0_Touched");

  // draw the page according to pageType
  switch(pageType_){
  case MONITOR:
    // no BACK button
    fassigned = false;  // no function
    break;
  case LIST8:
    // check if BACK button is touched
    if(checkTouchedBack(tx, ty)){  
      newPage_ = pageList8[pageIndex_].previous;
      previousCursorPos_[currentPage_] = 0;   // clear current cursor pos
    }
    else{   // check if a List box is touched
      fassigned = false;
      for(cnt = 0; cnt < repeatCount_; cnt++){
        if(checkTouchedBox(tx, ty, boxOI_list8[cnt].boxPos)){
          // page transitions
          newPage_ = pageList8[pageIndex_].destinations[cnt];
          previousCursorPos_[currentPage_] = cnt;   // save current cursor pos
          fassigned = true;
          break;
        }
      }
    }
    break;
  case BUTTON8:
    // check if BACK button is touched
    if(checkTouchedBack(tx, ty)){
      newPage_ = pageButton8[pageIndex_].previous;
    }
    else{   // check if a List box is touched
      fassigned = false;
      for(cnt = 0; cnt < repeatCount_; cnt++){
        if(checkTouchedBox(tx, ty, boxOI_list8[cnt].boxPos)){ // buttonBox8よりlist8の方がタッチ範囲が広いので流用
          changeButtonPos(cnt);                     // ボタンリストのボタン位置を変更し設定値を変更
          previousCursorPos_[currentPage_] = cnt;   // save current cursor pos
          fassigned = true;
          break;
        }
      }
      // cnt has the touched position value
      // Save/LoadのYes/Noボタンによる動作
      if(regType_ == SLSV && cnt == SAVELOAD_YESPOS){
        setMan_->saveDeviceSettings(regIndex_);       // セーブ
        showNotice(NOTICETIME1, boxOI_notice, boxOC_notice, "Saved!");     // 画面にNotice表示
        newPage_ = pageButton8[pageIndex_].previous;  // 前のページに戻る
        previousCursorPos_[currentPage_] = 0;   // clear current cursor pos
      }
      else if(regType_ == SLLD && cnt == SAVELOAD_YESPOS){
        setMan_->loadDeviceSettings(regIndex_);       // ロード
        showNotice(NOTICETIME1, boxOI_notice, boxOC_notice, "Loaded!");    // 画面にNotice表示
        newPage_ = pageButton8[pageIndex_].previous;  // 前のページに戻る
        previousCursorPos_[currentPage_] = 0;   // clear current cursor pos
      }
    }
    break;
  case VALUE:
    if(checkTouchedBack(tx, ty) || checkTouchedBox(tx, ty, boxOI_cancel.boxPos)){  // BACK or Cancel button
      num2hexToScreenValObj(currentValue_);       // revert to original value
      newPage_ = pageValue[pageIndex_].previous;  // Go back to previous page
    }
    else if(checkTouchedBox(tx, ty, boxOI_enter.boxPos)){         // Apply button
      num2hexToScreenValObj(newValue_); // uint32をhexに変換し画面のValueのBoxObjを表示更新
      setNewValue_toSetman(); // set value by setman
      newPage_ = pageValue[pageIndex_].previous;  // Go back to previous page
    }
    else if(checkTouchedBox(tx, ty, boxOI_valueUp.boxPos)){       // 上ボタンを押された時の処理
      incrementValue();
    }
    else if(checkTouchedBox(tx, ty, boxOI_valueDown.boxPos)){     // 下ボタンを押された時の処理
      decrementValue();
    }
    else{                                                         // check Value boxes
      fassigned = false;
      for(cnt = 0; cnt < repeatCount_; cnt++){
        if(checkTouchedBox(tx, ty, boxOI_value[cnt].boxPos)){
          cursorPos_ = cnt;
          moveCursorPosition(cursorPos_);
          fassigned = true;
          break;
        }
      }
    }
    break;
  default:
    DEBUG_PRINTLN("DT1 case:ERROR");
    fassigned = false;
    break;
  }
  return fassigned;
}

// process key control
// retval= fassigned= the flag if the pressed button has assigned for some function
bool Display::keyControl(uint16_t pushedSw){
  bool fassigned = true;

  // draw the page according to pageType
  DEBUG2_PRINTLN("KC0_keyControl");
  switch(pageType_){
  case MONITOR:
    if(pushedSw & bitposSwE_){
      monitorScrollType_.fMonitorScrollSw_ = !monitorScrollType_.fMonitorScrollSw_;
    }
    else if(pushedSw & bitposSwC_){
      if(monitorScrollType_.fMonitorScrollSw_) monitorScrollType_.fMonitorScrollSw_ = false;
      else newPage_ = MENU_TOP;
    }
    else if(pushedSw & BITPOS_SWU){
      // change display mode
      if(--monitorScrollType_.fMonitorDispSw_.byte > 3) monitorScrollType_.fMonitorDispSw_.byte = 3;
      // draw icons
      drawStatusIcon(SLI_HD, monitorScrollType_.fMonitorDispSw_.bit.hwfDisp);
      drawStatusIcon(SLI_SD, monitorScrollType_.fMonitorDispSw_.bit.swfDisp);
    }
    else fassigned = false;
    break;
  case LIST8:
    if(pushedSw & bitposSwC_){                     // 前のページに戻る
      newPage_ = pageList8[pageIndex_].previous;
      previousCursorPos_[currentPage_] = 0;   // clear current cursor pos
    }
    else if(pushedSw & bitposSwE_){                // カーソルのある行にページ移動
      newPage_ = pageList8[pageIndex_].destinations[cursorPos_];
      previousCursorPos_[currentPage_] = cursorPos_;   // save current cursor pos
    }
    else if(pushedSw & BITPOS_SWU){                // カーソルを上に
      if(--cursorPos_ < 0) cursorPos_ = repeatCount_ - 1;
      moveCursorPosition(cursorPos_);
    }
    else if(pushedSw & BITPOS_SWD){                // カーソルを下に
      if(++cursorPos_ >= repeatCount_) cursorPos_ = 0;
      moveCursorPosition(cursorPos_);
    }
    else fassigned = false;
    break;
  case BUTTON8:
    if(pushedSw & bitposSwC_){                     // 前のページに戻る
      newPage_ = pageButton8[pageIndex_].previous;
    }
    else if(pushedSw & bitposSwE_){                // カーソルのある行にボタンをセット
      changeButtonPos(cursorPos_);                  // ボタンリストのボタン位置を変更し設定値を変更
      previousCursorPos_[currentPage_] = cursorPos_;   // save current cursor pos
      // Save/LoadのYes/Noボタンによる動作
      if(regType_ == SLSV && cursorPos_ == SAVELOAD_YESPOS){
        setMan_->saveDeviceSettings(regIndex_);       // セーブ
        showNotice(NOTICETIME1, boxOI_notice, boxOC_notice, "Saved!");     // 画面にNotice表示
        newPage_ = pageButton8[pageIndex_].previous;  // 前のページに戻る
        previousCursorPos_[currentPage_] = 0;   // clear current cursor pos
      }
      else if(regType_ == SLLD && cursorPos_ == SAVELOAD_YESPOS){
        setMan_->loadDeviceSettings(regIndex_);       // ロード
        showNotice(NOTICETIME1, boxOI_notice, boxOC_notice, "Loaded!");    // 画面にNotice表示
        newPage_ = pageButton8[pageIndex_].previous;  // 前のページに戻る
        previousCursorPos_[currentPage_] = 0;   // clear current cursor pos
      }
    }
    else if(pushedSw & BITPOS_SWU){                // カーソルを上に
      if(--cursorPos_ < 0) cursorPos_ = repeatCount_ - 1;
      moveCursorPosition(cursorPos_);
    }
    else if(pushedSw & BITPOS_SWD){                // カーソルを下に
      if(++cursorPos_ >= repeatCount_) cursorPos_ = 0;
      moveCursorPosition(cursorPos_);
    }
    else fassigned = false;
    break;
  case VALUE:
    if(pushedSw & bitposSwC_){// Back button 値更新なし
      num2hexToScreenValObj(currentValue_);       // revert to original value
      newPage_ = pageValue[pageIndex_].previous;  // Go back to previous page
    }
    else if(pushedSw & bitposSwE_){// Apply button 値更新あり
      num2hexToScreenValObj(newValue_); // uint32をhexに変換し画面のValueのBoxObjを表示更新
      setNewValue_toSetman(); // set value by setman
      newPage_ = pageValue[pageIndex_].previous;  // Go back to previous page
    }
    else if(pushedSw & BITPOS_SWU){// カーソルの桁の値を1増やす
      incrementValue();
    }
    else if(pushedSw & BITPOS_SWD){// カーソルの桁の値を1減らす
      decrementValue();
    }
    else if(pushedSw & BITPOS_SWL){// カーソルを左に
      if(++cursorPos_ >= repeatCount_) cursorPos_ = 0;
      moveCursorPosition(cursorPos_);
    }
    else if(pushedSw & BITPOS_SWR){// カーソルを右に
      if(--cursorPos_ < 0) cursorPos_ = repeatCount_ - 1;
      moveCursorPosition(cursorPos_);
    }
    else fassigned = false;
    break;
  default:
    DEBUG_PRINTLN("KC1 case:ERROR");
    fassigned = false;
    break;
  }
  return fassigned;
}

// setManのOptionSwitchMappingフラグでSwitchを再マッピング
void Display::reMappingSw(void){
  int pageIndex, regIndex;
  eDeviceSettingRegType regType;

  page2typeIndexes(OPSMCE, pageIndex, regType, regIndex);
  bool flag_swapCE = setMan_->getSettingValue(regType, regIndex);
  if(flag_swapCE){            // swapping
    bitposSwC_ = BITPOS_SWE;
    bitposSwE_ = BITPOS_SWC;
  }
  else{                       // no swapping
    bitposSwC_ = BITPOS_SWC;
    bitposSwE_ = BITPOS_SWE;            
  }
}


