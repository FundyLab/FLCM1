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

#ifndef _FLCM1_IO_H_
#define _FLCM1_IO_H_

#include <Arduino.h>
#include <FlashStorage.h>
#include "debug.h"            // for debug out level setting
#include "mcp_can.h"
#include "screen.h"


// エラーレベル
#ifndef ERROR_GENERAL
#define ERROR_GENERAL   (-1)
#endif

// ***** PORT pin definitions
#define PA02  (14)
#define PA03  (42)
#define PA04  (17)
#define PA05  (18)
#define PA06  (8)
#define PA07  (9)
#define PA08  (4)
#define PA09  (3)
#define PA10  (1)
#define PA11  (0)
#define PA12  (22)
#define PA13  (38)
#define PA14  (2)
#define PA15  (5)
#define PA16  (11)
#define PA17  (13)
#define PA18  (10)
#define PA19  (12)
#define PA20  (6)
#define PA21  (7)
#define PA22  (20)
#define PA23  (21)
#define PA24  (28)  //USB D-
#define PA25  (29)  //USB D+
#define PA27  (26)
#define PA28  (27)
#define PA30  PIN_PA30
#define PA31  PIN_PA31
#define PB02  (19)
#define PB03  (25)
#define PB08  (15)
#define PB09  (16)
#define PB10  (23)
#define PB11  (24)
#define PB22  (30)
#define PB23  (31)
// ***** CAN definitions
#define CANMASKCOUNT    2
#define CANFILTERCOUNT  6
#define HWFFLMENUCOUNT  (CANFILTERCOUNT)
#define HWFMENUCOUNT    (CANMASKCOUNT+CANFILTERCOUNT)
#define F_MFTMASK       0
#define F_MFTFILTER     1
#define CANMASKVAL0     0x7ff   // default value 7ff:All bits must be match with filter to pass
#define CANMASKVAL1     0x300   // default value 7ff:All bits must be match with filter to pass
#define CANFILTERVAL0   0x20    // CAN ID filter default value
#define CANFILTERVAL1   0x30    // CAN ID filter default value
#define CANFILTERVAL2   0x140   // CAN ID filter default value
#define CANFILTERVAL3   0x150   // CAN ID filter default value
#define CANFILTERVAL4   0x160    // CAN ID filter default value
#define CANFILTERVAL5   0x280    // CAN ID filter default value
#define CANSTDIDDIGIT   3
#define CANEXTIDDIGIT   8
#define CANSTDIDNUMMAX  2047
#define CANEXTIDNUMMAX  536870911
#define CANFLSTD        0       // Filter Length (Standard 11bits)
#define CANFLEXT        1       // Filter Length (Extended 29bits)

// ***** SW definitions
#define BITPOS_SWC     (1 << 0)
#define BITPOS_SWE     (1 << 1)
#define BITPOS_SWU     (1 << 2)
#define BITPOS_SWD     (1 << 3)
#define BITPOS_SWL     (1 << 4)
#define BITPOS_SWR     (1 << 5)
#define BITPOS_SWS     (1 << 6)
#define BITPOS_SWP     (1 << 7)
// ***** Piezo Buzzer difinitions
#define NOTESCALE       0     // array pos
#define NOTEDURATION    1     // array pos
// ***** Screen definitions
#define TEXTSIZEX       6
#define TEXTSIZEY       8
#define TEXTMAGNITUDE   1
#define CURSORROWNUM    (ILI9341_TFTHEIGHT / (TEXTSIZEY * TEXTMAGNITUDE))   // 40
#define CURSORCOLNUM    (ILI9341_TFTWIDTH / (TEXTSIZEX * TEXTMAGNITUDE))   // 40
#define SCROLLMARGINLN  1
#define A_ROWSIZE       (TEXTSIZEY * TEXTMAGNITUDE)
#define POS_X0          0
#define POS_Y0          1
#define POS_X1          2
#define POS_Y1          3
#define POS_X2          4
#define POS_Y2          5
#define HEXDIGIT8       "%08X"
#define SAVELOAD_YESPOS 1
#define BACKGROUNDCOLOR             ILI9341_BLACK
#define STATUSLINE_BACKGROUNDCOLOR  ILI9341_BLUE
#define STATUSLINE_LINECOLOR        ILI9341_CYAN
#define STATUSLINE_TEXTCOLOR        ILI9341_YELLOW
#define SWFMENUCOUNT    MUTABLEOBJMAX
#define COMENUCOUNT     4
#define AUXMENUCOUNT    3
#define OPMENUCOUNT     1
#define DSHWFPOS_MASK0  0
#define DSHWFPOS_MASK1  3
#define DS_AOHSW_POS    0
#define DS_AOSSW_POS    1
#define DS_AOSBO_POS    2
#define VALUEISANY      1
#define VALUEISLIMITED  0
#define SIGNED64BITMIN  0x8000000000000000
#define SIGNED64BITMAX  0x7fffffffffffffff
#define NOTICETIME1     1000  // in ms
#define NOTICETIME3     3000  // in ms

// ***** TOUCH definitions
#define TOUCH_X_MAX     3700
#define TOUCH_X_MIN     300
#define TOUCH_Y_MAX     3853
#define TOUCH_Y_MIN     390
#define TOUCHDELAYTIME  250   // in ms


// **********************************************************
int powInt(int base, int exponent);

// **********************************************************
// CAN speed
// pageButton8[0]のconst char* buttonLabelsに内容を合わせること
const uint32_t CANspeedMap[] = {
  CAN_10KBPS, CAN_50KBPS, CAN_100KBPS, CAN_125KBPS, CAN_250KBPS, CAN_500KBPS, CAN_800KBPS, CAN_1000KBPS
  };

// CAN Software filtered value set
struct canSoftwareFilteredValueSet{
  int64_t value[MUTABLEOBJMAX];
  uint8_t len[MUTABLEOBJMAX];
  union{  // 0:false 1:true:filtered
    uint8_t byte;
    struct{
      unsigned b0 : 1;
      unsigned b1 : 1;
      unsigned b2 : 1;
      unsigned b3 : 1;
      unsigned b4 : 1;
      unsigned b5 : 1;
      unsigned b6 : 1;
      unsigned b7 : 1;
    } bit;
  } fIsFiltered;
};

// CAN Mask and Filter setting screen table
struct canMaskFilterTable{
  bool      fMaskFilter;  // 0:mask 1:filter
  byte      num;          // mask/filter number
  uint32_t  defaultValue;
};
extern const canMaskFilterTable canMFtable[HWFMENUCOUNT];

// **************************************************************************************************************
// Device Settings Manager class ********************************************************************************
// **************************************************************************************************************
// 設定データ種類
enum eDeviceSettingRegType{ //ePageのbutton以降の各行(画面ごと)を記載
  DSRT_ERROR = -1,
  // Button8 type
  CANSPEED,
  HWFFL,
  SWFSW, SWFSU,
  COSW, COPOL,
  AOSET,
  SLSV, SLLD,
  DS_OPSM,
  // Value type
  DS_HWF,
  SWFID, SWFSB, SWFSI, SWFEB, SWFEI,
  COUSF, COTRS,
  DSRTMAX
};

// SaveLoad場所を表す列挙型
enum eSaveLoadPos {
  SLP_SL0, SLP_SL1, SLP_SL2, SLP_SL3, SLP_SL4, SLP_SL5, SLP_SL6, SLP_SL7, SLP_TEMP, SLP_MAX
};

// 設定データ格納構造体
struct SoftwareFilter {
  int32_t canID;
  int8_t startByte;
  int8_t startBit;
  int8_t endByte;
  int8_t endBit;
  bool onoff;
  bool sign;
};

struct ComparatorOutput {
  uint64_t threshould;
  int8_t usingSwf;
  bool onoff;
  bool pol;
};

struct DeviceSettings {
  int8_t canSpeed;
  bool hwffl[HWFFLMENUCOUNT];         // 0:std, 1:extended
  int16_t hwf[HWFMENUCOUNT];
  SoftwareFilter swf[SWFMENUCOUNT];
  ComparatorOutput co[COMENUCOUNT];
  bool ao[AUXMENUCOUNT];
  bool op[OPMENUCOUNT];
};

// 設定記憶域操作クラスの定義
class SettingsManager{
private:
  DeviceSettings currentDeviceSetting_;  // 現在設定値（RAM内）
public:
  SettingsManager();

  // 設定値の呼び出し
  int32_t getSettingValue(eDeviceSettingRegType regType, int regIndex);
  uint64_t getSettingAnyvalue(eDeviceSettingRegType regType, int regIndex); // for COTRS
  // 設定値の範囲内チェック
  bool isValidSetting(int32_t value, eDeviceSettingRegType regType, int pageIndex);
  // 設定値の書き込み
  void setSettingValue(int32_t value, eDeviceSettingRegType regType, int pageIndex, int regIndex);
  void setSettingValue(uint64_t value, eDeviceSettingRegType regType, int pageIndex, int regIndex);// for COTRS
  // 現在設定値をFlash領域にsave
  void saveDeviceSettings(int pos);
  // 現在設定値をFlash領域からload
  void loadDeviceSettings(int pos);

};

// bit列抽出用設定のLengthをあらかじめ計算
// startBit of startByte must be left or same pos from endbit of endByte
void extractLen(SettingsManager *setMan, uint8_t swfIndex, uint8_t *byteLen, uint8_t *bitLen);


// **************************************************************************************************************
// Interval Timer **************************************************************************************************
// **************************************************************************************************************
class IntervalTimer {
private:
  unsigned int duration;
  unsigned long startTimestamp;
  unsigned long elapsedIntervals; // 経過したインターバルの回数を記録
  bool accumulateMode;            // 蓄積モードかどうかを示すフラグ
public:
  IntervalTimer(unsigned int duration, bool accumulateMode = false);
  bool isExpired();                               // タイマーが期限切れかどうかを確認
  unsigned long getElapsedIntervals();            // 経過したインターバルの回数を取得
  void reset();                                   // タイマーの開始時間をリセットする
  void setDuration(unsigned int newDuration);     // タイマーの持続時間を変更する
};


// **************************************************************************************************************
// Display ******************************************************************************************************
// **************************************************************************************************************
// Screen Page *********************************************************************************
// 画面のページを表す列挙型
// 各Typeにおける行(メニュー項目)を追加したらpage2typeIndexes,eDeviceSettingRegTypeも変更すること
enum ePage {
  // Monitor type:0 START:1 STOP:2
  PAGEERROR = -1, MONITOR_TYPE, INMONITOR,
  // List8 type
  LIST_TYPE, MENU_TOP, HWF, SWF, CO, AO, SL, OP,                  // Main menu
  HWFF0, HWFF1, HWFF2, HWFF3, HWFF4, HWFF5,                       // HardWareFilter Filter
  SWF0, SWF1, SWF2, SWF3, SWF4, SWF5, SWF6, SWF7,                 // SoftWareFilter
  CO0, CO1, CO2, CO3,                                             // CompareOut
  SL0, SL1, SL2, SL3, SL4, SL5, SL6, SL7,                         // SaveLoad
  // Button8 type
  BUTTON_TYPE, CAN_SPEED,
  HWFF0L, HWFF1L, HWFF2L, HWFF3L, HWFF4L, HWFF5L,                 // HWF FilterLength(std11/ext29)
  SWF0SW, SWF1SW, SWF2SW, SWF3SW, SWF4SW, SWF5SW, SWF6SW, SWF7SW, // SWF 
  SWF0SU, SWF1SU, SWF2SU, SWF3SU, SWF4SU, SWF5SU, SWF6SU, SWF7SU,
  CO0SW, CO1SW, CO2SW, CO3SW, CO4SW, CO5SW,
  CO0POL, CO1POL, CO2POL, CO3POL, CO4POL, CO5POL,
  AOHSW, AOSSW, AOSBO,
  SL0SV, SL1SV, SL2SV, SL3SV, SL4SV, SL5SV, SL6SV, SL7SV,
  SL0LD, SL1LD, SL2LD, SL3LD, SL4LD, SL5LD, SL6LD, SL7LD,
  OPSMCE,
  // Value type
  VALUE_TYPE, HWF0, HWF1, HWF2, HWF3, HWF4, HWF5, HWF6, HWF7,
  SWF0ID, SWF1ID, SWF2ID, SWF3ID, SWF4ID, SWF5ID, SWF6ID, SWF7ID,
  SWF0SB, SWF1SB, SWF2SB, SWF3SB, SWF4SB, SWF5SB, SWF6SB, SWF7SB, // start byte
  SWF0SI, SWF1SI, SWF2SI, SWF3SI, SWF4SI, SWF5SI, SWF6SI, SWF7SI, // start bit
  SWF0EB, SWF1EB, SWF2EB, SWF3EB, SWF4EB, SWF5EB, SWF6EB, SWF7EB, // end byte
  SWF0EI, SWF1EI, SWF2EI, SWF3EI, SWF4EI, SWF5EI, SWF6EI, SWF7EI, // end bit
  CO0USF, CO1USF, CO2USF, CO3USF, CO4USF, CO5USF,
  CO0TRS, CO1TRS, CO2TRS, CO3TRS, CO4TRS, CO5TRS,
  PAGEMAX
};

// 画面の種類を表す列挙型
enum ePageType {
  ERROR = -1, MONITOR, LIST8, BUTTON8, VALUE,
  PAGETYPEMAX
};

// status line icons on/off flag index
enum eStatuLineIcon {
  SLI_HF, SLI_HD, SLI_HA, SLI_SF, SLI_SD, SLI_SA, SLI_CP, SLIMAX
};

// ページの情報を格納する構造体の定義
struct pageInfo_list8 {
  int8_t listCount;
  ePage previous;             // 戻り先ページ
  ePage destinations[8];      // 遷移先ページ
  const char* listLabels[8];  // 各行の説明文字列
  const char* topTitle;       // Top title
  const char* subTitle;       // Sub title
};

// ページの情報を格納する構造体の定義
struct pageInfo_button8 {
  int8_t buttonCount;              // ボタン数
  ePage previous;               // 戻り先ページ
  const char* buttonLabels[8];  // 各ボタンの説明文字列
  const char* topTitle;       // Top title
  const char* subTitle;       // Sub title
};

// ページの情報を格納する構造体の定義
struct pageInfo_value {
  ePage previous;           // 戻り先ページ
  int8_t digitCount;           // 入力桁数
  bool isAnyVal;            // 0:value is limited in int32_t by minNumber and maxNumber, 1:value is any(for CO)
  int32_t minNumber;       // 入力可能最小値
  int32_t maxNumber;       // 入力可能最大値
  const char* topTitle;       // Top title
  const char* subTitle;       // Sub title
};

// Status line
const char StatusLine[] = "CANID: Message0-7      V";

// Function prototypes ********************************************************************************
int getButtonCount(int pageIndex);
int32_t getValueMax(int pageIndex);
int32_t getValueMin(int pageIndex);
bool getValueIsAny(int pageIndex);
// 画面ページから画面の種類に変換する関数
ePageType page2pageType(ePage page);
// 画面ページからPageTypeの配列番号に変換する
int page2pageIndex(ePage page);
// 画面ページからPageType情報の配列番号と、設定データ格納先情報とその配列番号に変換する関数定義
void page2typeIndexes(ePage page, int &pageIndex, eDeviceSettingRegType &regType, int &regIndex);


// Display class ********************************************************************************
struct monitorScrollType{
  union{  // 0:false 1:true:filtered
    uint8_t byte;
    struct{
      unsigned hwfDisp : 1; // true:HWF line on, false:off
      unsigned swfDisp : 1; // true:SWF line on, false:off
      unsigned : 6;
    } bit;
  } fMonitorDispSw_;
  bool fMonitorScrollSw_; // true:START false:STOP
};

// 画面描画クラスの定義
class Display {
private:
  Adafruit_ILI9341* tft_; // ILI9341 TFTディスプレイ
  SettingsManager* setMan_; // Device setting manager
  int tftWidth, tftHeight; // 画面の幅, 高さ
  int titleHeight; // タイトル行の高さ
  int listItemHeight; // リストアイテムの高さ
  int cln = 0;   // Cursor Line Number
  int pcln = 0;  // g-ram Pointer regarding Cursor Line Number 
  uint16_t color_ = ILI9341_WHITE;  // text color for monitor mode
  uint16_t statusColor_ = ILI9341_YELLOW;  // status line text color
  // Monitor動作状態
  monitorScrollType monitorScrollType_; 
  // status line
  bool statuIconSw_[SLIMAX];  // status line icons on/off flag. index: eStatuLineIcon
  BoxObject *iconHF_, *iconHD_, *iconHA_, *iconSF_, *iconSD_, *iconSA_, *iconCD_;
  BoxObject *objVcan_;    // CAN voltage obj
  // ページ情報
  ePage currentPage_ = INMONITOR, newPage_ = INMONITOR; // 現在ページ、新ページ
  ePageType pageType_ = ERROR;                  // ページ画面種類
  int repeatCount_ = 0;                         // 繰り返し表示オブジェクトの数
  int pageIndex_ = 0;                           // 画面種類におけるデータ位置
  eDeviceSettingRegType regType_ = DSRT_ERROR;  // データ保存のある画面の保存レジスタ種類
  int regIndex_ = 0;                            // 保存レジスタのデータ位置
  int cursorPos_ = 0;                     // cursor position
  int8_t previousCursorPos_[VALUE_TYPE];  // previous page's cursor position
  // ページvalue情報
  uint8_t byteLen_, bitLen_;             // byte length(max8[bytes]), bit length (max32[bits])
  uint32_t currentValue_ = 0, newValue_ = 0;  // for value setting page
  bool valueIsSign_;                          // 0:value is unsigned  1:value is signed
  uint32_t valueMax_, valueMin_;           // for value limiting in the value set page. sign bit is used valueIsSign_
  bool valueIsAny_;                    // 0:value is limiting with valueMax_, valueMin  1:value is uint64 for COTRS
  // Others
  BoxObject* iconType2obj(eStatuLineIcon iconType);
  const canMaskFilterTable *pCanMaskFilterTable_;
  void noticeByteBitPositionError(int swfIndex);
  uint16_t bitposSwC_ = BITPOS_SWC, bitposSwE_ = BITPOS_SWE;     // for swapping sw

public:
  Display(Adafruit_ILI9341* tft, SettingsManager* setMan, int width, int height, int titleH,
          int listItemH);
  
  // set CAN Mask/Filter flag for HWF menu
  void setCanMFtable(const canMaskFilterTable *pCanMaskFilterTable);

  // clear screen
  void clearScreen(uint16_t color);
  
  // show initial screen
  void showInitialScreen();

  // Set text color
  void setTextColor(uint16_t color);
  // home cursor
  void homeCursor();
  
  // monitor mode scroll type
  void resetMonitorScrollType();
  monitorScrollType getMonitorScrollType();

  // Write Status line
  void writeStatusLine(uint16_t color);
  void setStatusIconSw(eStatuLineIcon iconType, bool offOn);
  void drawStatusIcon(eStatuLineIcon iconType, bool offOn);
  void drawStatusIconAll();
  void drawCANvoltage(String vCan);

  // Show 1 line with scrolling
  void write1Line(String* s, uint16_t color = ILI9341_WHITE);
  void postLine(String& s, uint16_t color = ILI9341_WHITE);

  // check if the display mode is monitor
  bool isMonitorMode();
  
  // check if the display mode is changed
  bool isPageChanged();

  // check if the display page went back to monitor mode from other
  bool isPageBackToMonitorMode();

  // change page
  void changePage();

  // calc digit count 
  void calcRepeatCountOfDigit();
  // set limiting value
  void setLimitValue();
  // get / set the different type value with setman
  uint32_t getCurrentValue_fromSetman();
  void setNewValue_toSetman();

  // draw page
  void drawPage();

  // 設定値をdraw描画後表示する
  void setButtonToButtonListObj();
  void num2hexToScreenValObj(uint32_t val);
  // set newPage directly
  void setNewpageDirectly(ePage page);
  // check touched BOX for touch processing
  bool checkTouchedBox(int tx, int ty, const int16_t *boxPos);
  // check touched BACK button for touch processing
  bool checkTouchedBack(int tx, int ty);
  // show notice
  void showNotice(unsigned long time, const boxObjectInfo &info,
                  const boxObjectColor &color, const char* str);
  // ボタンリストのボタン位置を変更し設定値を変更
  void changeButtonPos(int newPos);
  // Increment / Decriment digit value
  void incrementValue();
  void decrementValue();
  // move cursor position
  void moveCursorPosition(int pos);
  // process touched sequence
  bool touched(int x, int y);
  // Mutableオブジェクトの型確認＆手動キャスト
  bool checkObject_ButtonList(int mutableObjPos, ButtonListObject *&obj);
  bool checkObject_Box(int mutableObjPos, BoxObject *&obj);
  bool checkObject_Cursor(CursorObject *&obj);
  // process key control
  bool keyControl(uint16_t pushedSw);
  void reMappingSw(void);
};


#endif