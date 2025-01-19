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
/*
  Revision
  v1.0.0: first

*/
/*
  Usage
  Required Arduino board setting: FundyLab plain SAMD21 (SAMD21)
  Required Arduino libraries:
    Adafruit ZeroDMA library
    Adafruit gfx library
    Adafruit ILI9341 library
*/

#include <SPI.h>
#include "wiring_private.h"   // for pinPeripheral() function
#include "utility/dma.h"      // dmac for SPI communication with display
#include <Adafruit_ZeroDMA.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h" // for display
#include "debug.h"            // for debug out level setting
#include "mcp25625_can.h"     // CAN control
#include "FLCM1_IO.h"         // screen, flash, settings, SW, beep
#include "FL_melody.h"        // for piezo buzzer

// SPI sercom port settings
#define TFT_MISO    PA16
#define TFT_SCK     PA19
#define TFT_MOSI    PA18
#define TFT_MISO_SERCOM  (PIO_SERCOM)  // PIO_SERCOM or PIO_SERCOM_ALT
#define TFT_SCK_SERCOM  (PIO_SERCOM)  // PIO_SERCOM or PIO_SERCOM_ALT
#define TFT_MOSI_SERCOM  (PIO_SERCOM)  // PIO_SERCOM or PIO_SERCOM_ALT
#define TOUCH_MISO  PA22
#define TOUCH_SCK   PA21
#define TOUCH_MOSI  PA20
#define TOUCH_MISO_SERCOM  (PIO_SERCOM)  // PIO_SERCOM or PIO_SERCOM_ALT
#define TOUCH_SCK_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define TOUCH_MOSI_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define MCPSD_MISO  PA06
#define MCPSD_SCK   PA05
#define MCPSD_MOSI  PA04
#define MCPSD_MISO_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define MCPSD_SCK_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define MCPSD_MOSI_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define AUX_MISO    PA12
#define AUX_SCK     PB11
#define AUX_MOSI    PB10
#define AUX_MISO_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define AUX_SCK_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT
#define AUX_MOSI_SERCOM  (PIO_SERCOM_ALT)  // PIO_SERCOM or PIO_SERCOM_ALT

// SPI Class difinitions  (&SERCOM#, MISOp#, SCKp#, MOSIp#, MOSI&SCKPAD#, MISOPAD#)
SPIClass tftSPI(&sercom1, TFT_MISO, TFT_SCK, TFT_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
SPIClass touchSPI(&sercom3, TOUCH_MISO, TOUCH_SCK, TOUCH_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
SPIClass mcpsdSPI(&sercom0, MCPSD_MISO, MCPSD_SCK, MCPSD_MOSI, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_2);
SPIClass auxSPI(&sercom4, AUX_MISO, AUX_SCK, AUX_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
/*
SPIClassSAMD tftSPI(&sercom1, TFT_MISO, TFT_SCK, TFT_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
SPIClassSAMD touchSPI(&sercom3, TOUCH_MISO, TOUCH_SCK, TOUCH_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
SPIClassSAMD mcpsdSPI(&sercom0, MCPSD_MISO, MCPSD_SCK, MCPSD_MOSI, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_2);
SPIClassSAMD auxSPI(&sercom4, AUX_MISO, AUX_SCK, AUX_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
*/

// ***** Port set speedup definitions
#define digitalWrite(x, state) { \
    PortGroup *port; \
    port = (PortGroup *)digitalPinToPort(x); \
    if (state == HIGH) { \
        port->OUTSET.reg = digitalPinToBitMask(x); \
    } else { \
        port->OUTCLR.reg = digitalPinToBitMask(x); \
    } \
}

// ***** Device Settings difinitions
SettingsManager setMan = SettingsManager();

// ***** Display ILI9341 difinitions
#define PIN_TFT_DC    PB22
#define PIN_TFT_CS    PA17
// TFTのLED端子は3.3Vに接続しないと暗くて見えない
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//SPIClass *spiClass, int8_t dc, int8_t cs = -1, int8_t rst = -1);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&tftSPI, PIN_TFT_DC, PIN_TFT_CS);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC);
// ***** Display difinitions
Display disp = Display(&tft, &setMan, 240, 320, 20, 20);
                // Adafruit_ILI9341* tft, int width, int height, int titleH, int listItemH
#define INITLOGOTIME    500   // in ms

// ***** Touch SPI definitions
#define PIN_TOUCH_CS    PA23
#define CMD_READ_X      0xD0  // 0xD0:12bit
#define CMD_READ_Y      0x90  // 0x90:12bit
#define CMD_READ_Z1     0xB8  // 0xB8:8bit
#define CMD_READ_Z2     0xC8  // 0xC8:8bit

// ***** CAN definitions
#define CANMFCOUNT      (HWFMENUCOUNT)     // CAN mask&filter setting registor count
#define PID_ENGIN_PRM       0x0C
#define PID_VEHICLE_SPEED   0x0D
#define PID_COOLANT_TEMP    0x05
#define CAN_ID_PID          0x7DF

#define CAN_2515
#define PIN_MCP_CS      PA07
#define CAN_INT         PB09
mcp25625_can CAN(PIN_MCP_CS); // Make CAN object and Set CS pin
volatile int8_t canrxIntFlag = 0;
canSoftwareFilteredValueSet canFiltVal;  // CAN software filtered value set

#define CANIDDIGIT3     3     // in Hex
#define CANIDDIGIT8     8     // in Hex
#define HEXDIGIT3       "%03X"
#define HEXDIGIT8       "%08X"

// ***** CAN definitions
// CAN isr
void MCP25625_ISR() {
  canrxIntFlag = 1;
}

// ***** SD  SPI definitions
#define PIN_SD_CS       PB08

// ***** AUX SPI definitions
#define PIN_AUX_CS      PA13
byte buf_aux[MAX_CHAR_IN_MESSAGE];

// ***** Comparator Output definitions
#define PIN_CO0         PA11
#define PIN_CO1         PA10
#define PIN_CO2         PA09
#define PIN_CO3         PA08
#define PIN_COCOUNT     (4) // The number of CO
const int outputPorts_CO[PIN_COCOUNT] = {PIN_CO0, PIN_CO1, PIN_CO2, PIN_CO3};

// ***** SW definitions
#define PIN_SWA         PB23 // Digital Output
#define PIN_SWB         PA28 // Digital Output PCBv1.0
#define PIN_SWC         PA15 // Digital Output PCBv1.0
#define PIN_SWD         PA14 // Digital Output PCBv1.0
#define PIN_SW0         PA27 // Digital Input PCBv1.0
#define PIN_SW1         PB03 // Digital Input
#define SWDETPERIOD     5    // in ms
#define SWLONGTHRESH    1000 // in ms
#define PUSHDELAYTIME   10   // in ms
const int OUTCNT_SW = 4;       // get SW mux output port count
const int INCNT_SW = 2;        // get SW mux input port count
const int outputPorts_SW[OUTCNT_SW] = {PIN_SWA, PIN_SWB, PIN_SWC, PIN_SWD};    // SW mux outputs
const int inputPorts_SW[INCNT_SW] = {PIN_SW0, PIN_SW1};              // SW mux inputs
const int SWLONGPUSHT = SWLONGTHRESH/(SWDETPERIOD*OUTCNT_SW);

// ***** A/D definitions
#define PIN_VCAN        PA02    // Digital Output PCBv1.0
#define ADRESOLUTION    10      // A/D分解能 10/12 ノイズ多いと上げても意味ない
#define ADVREF          3.25    // A/D converter vref voltage (VDD)
#define ADVDIVRATIO     0.0974  // vCAN input voltage division ratio
#define ADSFSHIFTBITS   10      // Scale Factor ratio
#define ADDECSCALE      10      // decimal value scale ratio
#define ADDETPERIOD     200     // in ms
const long adScaleFactor = (ADVREF * ADDECSCALE) / (1 << ADRESOLUTION)
                            / ADVDIVRATIO * (1 << ADSFSHIFTBITS); // scale factor[mV]

// ***** Piezo Buzzer difinitions
#define PIN_PZ          PA03  // Digital Output PCBv1.0
MelodyPlayer mplay(PIN_PZ);

// ***** LED definitions
#define PIN_ERROR       PB02 // PCBv1.0

// ***** ERROR detection definitions
#define ERRDETPERIOD    100    // in ms
uint8_t mcp_error = 0;

// ***** DEBUG definitions
volatile bool error = false;
unsigned char PID_INPUT;
unsigned char getPid    = 0;

// ***** DMA definitions
#define DATA_LENGTH 16
#define TRANSFER_LENGTH 8
Adafruit_ZeroDMA auxDMA;
DmacDescriptor *auxDMA_dsc;

uint8_t tftDMA_srcmem[DATA_LENGTH];
//uint8_t touchDMA_srcmem[DATA_LENGTH];
uint8_t mcpsdDMA_srcmem[DATA_LENGTH];
uint8_t auxDMA_srcmem[DATA_LENGTH];
volatile bool tftDMA_done = true;
//volatile bool touchDMA_done = true;
volatile bool mcpsdDMA_done = true;
volatile bool auxDMA_done = true;
void tftdma_callback([[maybe_unused]] Adafruit_ZeroDMA *dma) {
  // CS disabled (more faster descriptyon than digitalWrite)
  digitalPinToPort(PIN_TFT_CS)->OUTSET.reg = digitalPinToBitMask(PIN_TFT_CS);
  auxDMA_done = true;
}
void mcpsddma_callback([[maybe_unused]] Adafruit_ZeroDMA *dma) {
  // CS disabled (more faster descriptyon than digitalWrite)
  if((digitalPinToPort(PIN_MCP_CS)->OUT.reg & digitalPinToBitMask(PIN_MCP_CS)) == 0){
    digitalPinToPort(PIN_MCP_CS)->OUTSET.reg = digitalPinToBitMask(PIN_MCP_CS);
  }
  else if((digitalPinToPort(PIN_SD_CS)->OUT.reg & digitalPinToBitMask(PIN_SD_CS)) == 0){
    digitalPinToPort(PIN_SD_CS)->OUTSET.reg = digitalPinToBitMask(PIN_SD_CS);
  }
  else{
    error = true;
    return;
  }
  auxDMA_done = true;
}
void auxdma_callback([[maybe_unused]] Adafruit_ZeroDMA *dma) {
  // CS disabled (more faster descriptyon than digitalWrite)
  digitalPinToPort(PIN_AUX_CS)->OUTSET.reg = digitalPinToBitMask(PIN_AUX_CS);
  auxDMA_done = true;
}

// ***** Interval timer definitions
IntervalTimer swDetTimer(SWDETPERIOD);      // push switches,touch detection timer
IntervalTimer adDetTimer(ADDETPERIOD);      // A/D detection timer
IntervalTimer pushDelay(PUSHDELAYTIME);     // push switches
IntervalTimer touchDelay(TOUCHDELAYTIME);   // touch detection
IntervalTimer errDetTimer(ERRDETPERIOD);    // error detection


// **************************************************************************************
// Class                                                                                *
// **************************************************************************************

// SW Class *********************************************************************************
class PushSwitchMatrix {
private:
  const int* outputPorts; // output Ports list
  const int* inputPorts;  // input Ports listcurrentSwStates
  const int outputCount;  // Output Port count
  const int inputCount;   // Input Port count
  int longPressThreshold_; // long Press Threshold times
  uint16_t currentSwStates; // スイッチの状態
  uint16_t previousSwStates; // 前回のスイッチの状態
  int* longPressStates; // 長押しの状態
  int prevPortNum;  // previous selected output port num
  bool fAlternateOutput;   // flag false:turnoff true:turnon to make output stable time
  bool fPressed;      // High if new pressed any sw
  bool fLongPressed;  // High if new long pressed any sw
  uint16_t pressedSWs;    // a corresponding bit goes high if a SW pressed
  uint16_t longPressedSWs;  // a corresponding bit goes high if a SW long pressed

public:
  PushSwitchMatrix(const int* outputs, const int* inputs, const int outCnt, const int inCnt,
    int longPressThreshold) 
  : outputPorts(outputs), inputPorts(inputs), outputCount(outCnt), inputCount(inCnt),
    longPressThreshold_(longPressThreshold) {
      longPressStates = new int[outCnt * inCnt];
      currentSwStates = previousSwStates = outputPtr = longPressedSWs = 0;
      fAlternateOutput = fPressed = false;
      prevPortNum = outputPorts[outputPtr];
    }
  int outputPtr;    // output port select pointer

  // スイッチの状態検知メソッド。チャタリング検知しない周期でコールすること
  // 戻り値：True=全ポート検知完了, False=未完了
  bool periodicDetSwitchState() {
    bool retval = false;
    if(fAlternateOutput){
      // スイッチの状態を読み取る
      for (int j = 0; j < inputCount; j++) {
        bool state = digitalRead(inputPorts[j]);                        // state:0 if pushed
        currentSwStates |= ((!state) << (outputPtr * inputCount + j));  // bit invert and set 
      }
      // 次の行を選択、及び全SW読み取った時の処理
      if(++outputPtr >= outputCount){
        outputPtr = 0;
        detSwitchStateChanges();
        currentSwStates = 0;
        retval = true;
      }
    }
    // 行出力、次このメソッド呼ばれるまで出力安定待ち
    selectOut(outputPorts[outputPtr]);
    // 1周したか判定出力
    return retval;
  }

  // SWが短押しされたかの変数を取得するインタフェース
  uint16_t getPressedSWs(){
    if(fPressed){
      fPressed = false;
      return pressedSWs;
    }
    else return 0;
  }
  // SWが長押しされたかの変数を取得するインタフェース
  uint16_t getLongPressedSWs(){
    if(fLongPressed){
      fLongPressed = false;
      return longPressedSWs;
    }
    else return 0;
  }

private:
  void detSwitchStateChanges() {
    uint16_t pressedSw = 0, longPressedSw = 0;
    // プッシュSWの短押しを判定する
    if(currentSwStates != previousSwStates){
      // スイッチが変化し且つ押された時だけ対応bitをHighにセット
      pressedSw = ~previousSwStates & currentSwStates;
      if(pressedSw) fPressed = true;           // reset by external
      previousSwStates = currentSwStates;   // 前回の状態を更新
      pressedSWs = pressedSw;                      // copy for external access
    }
    // プッシュSWの長押しを判定する
    for (int i = 0; i < outputCount * inputCount; i++) {
      if (((currentSwStates >> i) & 1) == 1) {
        longPressStates[i]++;
        if (longPressStates[i] >= longPressThreshold_) {
          // 長押しを検知したら対応bitをHighにセット
          longPressedSw |= (1 << i);
          fLongPressed = true;          // reset by external
        }
      }
      else {
        longPressStates[i] = 0; // SWが離されたのでカウントをリセット
      }
    }
    longPressedSWs = longPressedSw;       // copy for external access
  }

  // SW mux output を出力してSW検出対象行を選択
  void selectOut(int newPortNum) {
    for(int i = 0; i < outputCount; i++){
      digitalPinToPort(outputPorts[i])->OUTSET.reg = digitalPinToBitMask(outputPorts[i]);
    }
    if(fAlternateOutput){     // 今の出力をdisable(high)にする
      digitalPinToPort(prevPortNum)->OUTSET.reg = digitalPinToBitMask(prevPortNum);
      fAlternateOutput = false;
    }
    else{
      // 次の出力ポートをactive(Low)にする
      digitalPinToPort(newPortNum)->OUTCLR.reg = digitalPinToBitMask(newPortNum);
      prevPortNum = newPortNum;
      fAlternateOutput = true;
    }
  }
};
// PushSwitchMatrixクラスのインスタンスを作成
PushSwitchMatrix swMux(outputPorts_SW, inputPorts_SW, OUTCNT_SW, INCNT_SW, SWLONGPUSHT);



// **************************************************************************************
// Functions                                                                            *
// **************************************************************************************

// ***** TOUCH screen
int readTouchPos(int &x,int &y) {
  int zout = 0;
  uint8_t z1,z2;

  // read touch data via SPI
  digitalWrite(PIN_TOUCH_CS, LOW);
  touchSPI.transfer(CMD_READ_X);
  x = touchSPI.transfer16(0) >> 3;
  //x = x >> 3;
  touchSPI.transfer(CMD_READ_Y);
  y = touchSPI.transfer16(0) >> 3;
  //y = y >> 3;
  touchSPI.transfer(CMD_READ_Z1);
  z1 = touchSPI.transfer16(0);
  touchSPI.transfer(CMD_READ_Z2);
  z2 = touchSPI.transfer16(0);
  digitalWrite(PIN_TOUCH_CS, HIGH);
  // touched judgment
  if (z1 > 0 && z2 > 0) {   // タッチと判定された
    zout = z1 + z2;
    // Coordinate transformation
    x = ILI9341_TFTWIDTH * (x - TOUCH_X_MIN) / (TOUCH_X_MAX - TOUCH_X_MIN);
    if(x < 0) zout = 0;     // no touch
    else if(x >= ILI9341_TFTWIDTH) x = ILI9341_TFTWIDTH - 1;
    y = ILI9341_TFTHEIGHT * (TOUCH_Y_MAX - y) / (TOUCH_Y_MAX - TOUCH_Y_MIN);
    if(y < 0) zout = 0;     // no touch
    else if(y >= ILI9341_TFTHEIGHT) y = ILI9341_TFTHEIGHT - 1;
  }
  return zout;
}

// AUX SPI output *********************************************************************************
void auxSend(uint8_t *buf, uint8_t len){
  if(auxDMA_done){
    auxDMA.changeDescriptor(auxDMA_dsc, buf, (void *)(&SERCOM4->SPI.DATA.reg), len);
                        // DMA description*, from, to, count
    // CAN data are sent from SPI with DMA
    digitalPinToPort(PIN_AUX_CS)->OUTCLR.reg = digitalPinToBitMask(PIN_AUX_CS);
    auxDMA_done = false;
    auxDMA.startJob();
  }
  else{
    error = true;
    DEBUG_PRINTLN("auxSend ERROR");
  }
}

void auxSend_filtered(int64_t value, uint8_t len){
  if(auxDMA_done){
    bool byteOrder = setMan.getSettingValue(AOSET, DS_AOSBO_POS);
    // copying data to the DMA transmission source_memory
    for(int i = 0; i < len; i++){
      if(byteOrder) buf_aux[i] = (value >> i * 8) & 0x0ff;// little endian to send out
      else buf_aux[len - 1 - i] = (value >> i * 8) & 0x0ff;// big endian to send out
    }
    auxSend(buf_aux, len);  // send out buf to aux output
  }
  else{
    error = true;
    DEBUG_PRINTLN("auxSend_filtered ERROR");
  }
}

// Comparator Output *********************************************************************************
// output invert by COPOL. active high of the mcppin output, low of the CO output
void outputCOwithPolarity(int coNum, bool lowHigh){
  bool copol = (bool)setMan.getSettingValue(COPOL, coNum);
  bool lh = copol ^ lowHigh;
  DEBUG2_PRINT("lowHigh, copol, lh= ");DEBUG2_PRINT(lowHigh);DEBUG2_PRINT(", ");DEBUG2_PRINT(copol);DEBUG2_PRINT(", ");DEBUG2_PRINTLN(lh);
  if(lh){   // active
    digitalPinToPort(outputPorts_CO[coNum])->OUTSET.reg = digitalPinToBitMask(outputPorts_CO[coNum]);
    DEBUG2_PRINTLN("CO out on!");
  }
  else{ // inactive
    digitalPinToPort(outputPorts_CO[coNum])->OUTCLR.reg = digitalPinToBitMask(outputPorts_CO[coNum]);
    DEBUG2_PRINTLN("CO out off!");
  }
}

void calcComparaterOut(int64_t value, int swfNum){
  for (int coNum = 0; coNum < PIN_COCOUNT; coNum++){
    if(setMan.getSettingValue(COSW, coNum) == true && setMan.getSettingValue(COUSF, coNum) == swfNum){
      int64_t trs = setMan.getSettingAnyvalue(COTRS, coNum);
      DEBUG2_PRINT("trs, CO value=");DEBUG2_PRINT(trs);DEBUG2_PRINT(", ");DEBUG2_PRINTLN(value);
      if(value >= trs) outputCOwithPolarity(coNum, HIGH);
      else outputCOwithPolarity(coNum, LOW);
    }
  }
}


// SW and Touch detecting *********************************************************************************
// Call every 1-100ms
void swDet(uint16_t &pushedSw, uint16_t &longpushedSw){
  static bool fPushDelay = false;           // push switches
  if(!fPushDelay){
    if(swMux.periodicDetSwitchState()){     // checked all SWs
      // get change state of SWs
      pushedSw = swMux.getPressedSWs();
      longpushedSw = swMux.getLongPressedSWs();
      // pushed process
      if(pushedSw){
        fPushDelay = true;
        pushDelay.reset();
        DEBUG2_PRINT("SW pressed = "); DEBUG2_PRINTLN(pushedSw);
      }
      if(longpushedSw){
        DEBUG2_PRINT(" SW long pressed = "); DEBUG2_PRINTLN(longpushedSw);
      }
    }
  }
  else{               // in the push delay time
    pushedSw = 0;     // output clear
    if(pushDelay.isExpired()) fPushDelay = false;  // reset process
  }
}

void touchDet(int &touched, int &tx, int &ty){
  static bool fTouchDelay = false;          // touch detection
  if(!fTouchDelay){   // in the touch detecting
    touched = readTouchPos(tx, ty);   // detecting
    // touched process
    if(touched > 0){
      fTouchDelay = true;
      touchDelay.reset();
      DEBUG2_PRINT("Touched!! x,y,z = "); DEBUG2_PRINT(tx); DEBUG2_PRINT(", "); DEBUG2_PRINT(ty);
      DEBUG2_PRINT(", "); DEBUG2_PRINTLN(touched);
    }
  }
  else{               // in the touch delay time
    touched = 0;      // output clear
    if(touchDelay.isExpired()) fTouchDelay = false;  // reset process
  }
}


// CAN functions *********************************************************************************
// init CAN command
void setCANspeed(){
  // read and limit speed map
  uint32_t speedset = CANspeedMap[setMan.getSettingValue(CANSPEED, 0)];
  // init can bus
  while (CAN_OK != CAN.begin_noSPIset(speedset)) {
    DEBUG_PRINTLN("CAN init fail, retry...");
    delay(500);
  }
  DEBUG_PRINTLN("CAN init ok!");
}

void setMaskFilter() {
  bool ext;
  int32_t value;
  for(int i = 0; i < CANMFCOUNT; i++){
    // read Mask/Filter value from memory
    value = setMan.getSettingValue(DS_HWF, i);
    if(!setMan.isValidSetting(value, DS_HWF, i)) value = canMFtable[i].defaultValue;
    DEBUG2_PRINT("setMaskFilterValue = ");DEBUG2_PRINTLN(value);
    DEBUG2_PRINT("fMaskFilter, num = ");DEBUG2_PRINT(canMFtable[i].fMaskFilter);DEBUG2_PRINT(", ");DEBUG2_PRINTLN(canMFtable[i].num);
    // write Mask/FilterValue to MCP
    if(canMFtable[i].fMaskFilter){                    // write filter value
      ext = setMan.getSettingValue(HWFFL, canMFtable[i].num);
      CAN.init_Filt(canMFtable[i].num, ext, value);
    }
    else CAN.init_Mask(canMFtable[i].num, 0, value);  // write mask value
  }
}

// Out: &msgSet: CAN message
void getCanMsg(canMessageSet &msgSet){
  // read CAN msg
  CAN.readMsgBufID(&msgSet.id, &msgSet.ext, &msgSet.len, msgSet.buf);
  if(msgSet.len > MAX_CHAR_IN_MESSAGE) msgSet.len = MAX_CHAR_IN_MESSAGE;  // limitation of len
}

// make a string for display 1 line
void formatMsg1line(canMessageSet &msgSet, String &canString){
  // 数値をフォーマット
  char hexStr[CANIDDIGIT8 + 1];   // 桁数+null文字分
  sprintf(hexStr, HEXDIGIT8, msgSet.id);
  // 文字列に変換
  canString = String(hexStr) + " Msg: ";
  for (int i = 0; i < msgSet.len; i++) canString += String(msgSet.buf[i], HEX) + " ";
}

// make a filtered string for display 1 line
void formatMsg1line_filtered(canMessageSet &msgSet, String &canString, int swf_num){
  // 数値をフォーマット
  char hexStr[CANIDDIGIT8 + 1];   // ID変換 桁数+null文字分
  sprintf(hexStr, HEXDIGIT8, msgSet.id);
  // 文字列に変換
  canString = "0x" + String(hexStr) + " SWF" + String(swf_num, HEX) + ": ";
  if(canFiltVal.len[swf_num] <= 1) canString += String((uint8_t)canFiltVal.value[swf_num], HEX);
  else if(canFiltVal.len[swf_num] <= 2) canString += String((uint16_t)canFiltVal.value[swf_num], HEX);
  else if(canFiltVal.len[swf_num] <= 4) canString += String((uint32_t)canFiltVal.value[swf_num], HEX);
  else{
    canString += String((uint32_t)(canFiltVal.value[swf_num] >> 32), HEX);
    canString += String((uint32_t)canFiltVal.value[swf_num], HEX);
  }
}

// bit列抽出 BigEndian Out: &len,&result
// CAN msg: [byte0][byte1]...[byte7]   [byteX]: [bit7][bit6]...[bit0]
// startBit of startByte must be left or same pos from endbit of endByte
void extractBits(const uint8_t* data, bool sign, uint8_t startByte, uint8_t startBit, uint8_t endByte,
                  uint8_t endBit, int64_t &result) {
  // calc sign
  bool negative = false;   // true: signed and negative, false: positive
  if(sign && ((data[startByte] >> startBit) & 0x1)) negative = true;
  // extract
  int64_t temp_res = 0;
  for (uint8_t byteIndex = startByte; byteIndex <= endByte; ++byteIndex) {
    if(byteIndex == startByte){       // calc startByte
      uint8_t mask = (uint8_t)(((uint16_t)1 << (startBit + 1)) - 1);
      if(negative) temp_res = (int8_t)(data[startByte] | ~mask);
      else temp_res = data[startByte] & mask;
      // when startByte == endByte
      if(byteIndex == endByte) temp_res = temp_res >> endBit;
    }
    else if(byteIndex == endByte){    // calc endByte
      temp_res = temp_res << (8 - endBit) | data[endByte] >> endBit;
    }                                 // calc mid bytes
    else temp_res = temp_res << 8 | data[byteIndex];
  }
  result = temp_res;
  DEBUG2_PRINT("extractBits result= ");DEBUG2_PRINTLN(result);
}

// calc SWF byte length
void calcLen(){
  uint8_t dummy;
  for(int i = 0; i < MUTABLEOBJMAX; i++){   // repeat i for SWF0-7
    extractLen(&setMan, i, &canFiltVal.len[i], &dummy);
  }
}

// CAN software filtering
void applySoftwareFilter(canMessageSet &msgSet){
  canFiltVal.fIsFiltered.byte = 0;          // filterに引っかかったフラグ初期化
  for(int i = 0; i < MUTABLEOBJMAX; i++){   // repeat i for SWF0-7
    if(setMan.getSettingValue(SWFSW, i) != false && msgSet.id == (uint32_t)setMan.getSettingValue(SWFID, i)
        && canFiltVal.len[i] != 0){
      // filterに引っかかったフラグON
      canFiltVal.fIsFiltered.byte |= (1 << i);
      // データ切出準備
      bool sign = setMan.getSettingValue(SWFSU, i);
      uint8_t startByte = setMan.getSettingValue(SWFSB, i);
      uint8_t startBit = setMan.getSettingValue(SWFSI, i);
      uint8_t endByte = setMan.getSettingValue(SWFEB, i);
      uint8_t endBit = setMan.getSettingValue(SWFEI, i);
      // データ切出＆データ保存
      extractBits(msgSet.buf, sign, startByte, startBit, endByte, endBit, canFiltVal.value[i]);
    }
  }
}

// Display functions *********************************************************************************
// status line icons
void setSetmanIconsSw(){
  bool flag = false;
  // HWF icon on/off
  disp.setStatusIconSw(SLI_HF, 
    (setMan.getSettingValue(DS_HWF, DSHWFPOS_MASK0) && setMan.getSettingValue(DS_HWF, DSHWFPOS_MASK1)));
  // HA icon on/off
  disp.setStatusIconSw(SLI_HA, setMan.getSettingValue(AOSET, DS_AOHSW_POS));
  // SWF icon on/off
  for(int i = 0; i < SWFMENUCOUNT; i++) flag |= setMan.getSettingValue(SWFSW, i);
  disp.setStatusIconSw(SLI_SF, flag);
  // SA icon on/off
  disp.setStatusIconSw(SLI_SA, setMan.getSettingValue(AOSET, DS_AOSSW_POS));
  // CP icon on/off
  for(int i = 0; i < COMENUCOUNT; i++) flag |= setMan.getSettingValue(COSW, i);
  disp.setStatusIconSw(SLI_CP, flag);
  // HD/SD icon is changed from Display
}


// **************************************************************************************
// Setup                                                                                *
// **************************************************************************************
void setup() {
  // port init
  pinMode(PIN_TFT_DC, OUTPUT);
  pinMode(PIN_TOUCH_CS, OUTPUT);
  pinMode(PIN_SD_CS, OUTPUT);
  pinMode(PIN_MCP_CS, OUTPUT);
  pinMode(PIN_AUX_CS, OUTPUT);
  for(int i = 0; i < OUTCNT_SW; i++) pinMode(outputPorts_SW[i], OUTPUT);
  for(int i = 0; i < INCNT_SW; i++) pinMode(inputPorts_SW[i], INPUT);
  pinMode(CAN_INT, INPUT);
  pinMode(PIN_PZ, OUTPUT);
  pinMode(PIN_ERROR, OUTPUT);
  digitalWrite(PIN_ERROR, HIGH);
  for(int coNum = 0; coNum < PIN_COCOUNT; coNum++){
    pinMode(outputPorts_CO[coNum], OUTPUT);
    outputCOwithPolarity(coNum, LOW);
  }
  digitalWrite(PIN_TFT_DC, HIGH);
  digitalWrite(PIN_TOUCH_CS, HIGH);
  digitalWrite(PIN_SD_CS, HIGH);
  digitalWrite(PIN_MCP_CS, HIGH);
  digitalWrite(PIN_AUX_CS, HIGH);
  
  // A/D init
  analogReadResolution(ADRESOLUTION);
  
  // serial init
  Serial.begin(115200);
  //while (!Serial);
  delay(100);
  DEBUG2_PRINTLN("ILI9341 Test!");

  // SPI init
  tftSPI.begin();
  pinPeripheral(TFT_MISO, TFT_MISO_SERCOM);
  pinPeripheral(TFT_SCK, TFT_SCK_SERCOM);
  pinPeripheral(TFT_MOSI, TFT_MOSI_SERCOM);
  touchSPI.begin();
  touchSPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  pinPeripheral(TOUCH_MISO, TOUCH_MISO_SERCOM);
  pinPeripheral(TOUCH_SCK, TOUCH_SCK_SERCOM);
  pinPeripheral(TOUCH_MOSI, TOUCH_MOSI_SERCOM);
  mcpsdSPI.begin();
  pinPeripheral(MCPSD_MISO, MCPSD_MISO_SERCOM);
  pinPeripheral(MCPSD_SCK, MCPSD_SCK_SERCOM);
  pinPeripheral(MCPSD_MOSI, MCPSD_MOSI_SERCOM);
  auxSPI.begin();
  auxSPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  pinPeripheral(AUX_MISO, AUX_MISO_SERCOM);
  pinPeripheral(AUX_SCK, AUX_SCK_SERCOM);
  pinPeripheral(AUX_MOSI, AUX_MOSI_SERCOM);
  tft.begin();
  auxDMA.setTrigger(SERCOM4_DMAC_ID_TX);
  auxDMA.setAction(DMA_TRIGGER_ACTON_BEAT);
  auxDMA.allocate();
  auxDMA_dsc = auxDMA.addDescriptor(
    auxDMA_srcmem, (void *)(&SERCOM4->SPI.DATA.reg), TRANSFER_LENGTH,
    // move data from, move data to, this many...
    DMA_BEAT_SIZE_BYTE, true, false);
    // bytes/hword/words, increment source addr?, increment dest addr?
  auxDMA.setCallback(auxdma_callback);

  // Device Settings load from TEMP
  setMan.loadDeviceSettings(SLP_TEMP);
  disp.reMappingSw();                   // reMapping Switches

  // making the melody
  mplay.start(&melody1); // メロディ1を再生開始

  // display init
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  DEBUG2_PRINT("Display Power Mode: 0x"); DEBUG2_PRINT2LN(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  DEBUG2_PRINT("MADCTL Mode: 0x"); DEBUG2_PRINT2LN(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  DEBUG2_PRINT("Pixel Format: 0x"); DEBUG2_PRINT2LN(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  DEBUG2_PRINT("Image Format: 0x"); DEBUG2_PRINT2LN(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  DEBUG2_PRINT("Self Diagnostic: 0x"); DEBUG2_PRINT2LN(x, HEX);

  // display test
  DEBUG2_PRINTLN(F("Show initial screen"));
  //disp.testFillScreen();
  disp.showInitialScreen();
  
  // CAN init
  attachInterrupt(digitalPinToInterrupt(CAN_INT), MCP25625_ISR, FALLING); // interrupt init
  CAN.setSPI(&mcpsdSPI);
  setCANspeed();        // デバイス設定値のcanspeedにセット
  setMaskFilter();      // set mcp mask and filter
  calcLen();            // calc SWF byte length
  Serial.println("Setup fin!");

  // display init2
  DEBUG2_PRINTLN(F("Done!"));
  disp.clearScreen(ILI9341_BLACK);
  setSetmanIconsSw();              // ステータスアイコンsw更新
  disp.setCanMFtable(canMFtable);

  // show can monitor page
  disp.setNewpageDirectly(INMONITOR);
  disp.changePage();                      // 表示ページを更新
  
  // reset Interval timer
  swDetTimer.reset();     // push switches,touch detection timer
  adDetTimer.reset();     // A/D detection timer
  pushDelay.reset();      // push switches
  touchDelay.reset();     // touch detection
  errDetTimer.reset();    // error detection
}

// **************************************************************************************
// Main                                                                                 *
// **************************************************************************************
void loop(void) {
  canMessageSet msgSet;                     // CAN messages
  String canString;                         // CAN string for display 1line
  uint16_t pushedSw = 0, longPushedSw = 0;  // push switches
  int touchX, touchY, touched =0;           // touch detection position and power
  long vCan_mV = 0;                         // CAN Vcc Voltage [mV]

  // SW detecting every SWDETPERIOD
  if(swDetTimer.isExpired()){
    swDet(pushedSw, longPushedSw);
    touchDet(touched, touchX, touchY);
  }

  // A/D detecting every ADETPERIOD
  if(adDetTimer.isExpired()){
    // read the value from A/D:
    vCan_mV = (analogRead(PIN_VCAN) * adScaleFactor >> ADSFSHIFTBITS);
    int integerPart = vCan_mV / 10;
    int decimalPart = vCan_mV % 10;
    String result = String(integerPart) + "." + String(decimalPart);
    disp.drawCANvoltage(result);
    DEBUG2_PRINT("SF= ");DEBUG2_PRINT(adScaleFactor);DEBUG2_PRINT(", Vcan[mV] = ");DEBUG2_PRINTLN(result);
  }

  // display
  if(pushedSw){                     // キー操作検出時の処理
    if(disp.keyControl(pushedSw)) mplay.start(&melody2); // ビープ
  }
  else if(touched > 0){             // タッチ検出時の処理
    if(disp.touched(touchX, touchY)) mplay.start(&melody3); // ビープ
  }
  else if(disp.isPageChanged()){    // 画面が変更された時の処理
    // 画面ページ書き換え
    if(disp.isPageBackToMonitorMode()){        // 設定画面からモニター画面に戻った時はこっちの処理
      // 表示ページ更新前に行う処理
      setSetmanIconsSw();              // ステータスアイコンsw更新
      // 表示ページを更新
      disp.changePage();                      // 表示ページを更新
      // 設定値の処理
      setMan.saveDeviceSettings(SLP_TEMP);    // currentDeviceSettingsの設定をflashのtemp領域に保存
      mplay.start(&melody4);                  // ビープ
      setCANspeed();                          // デバイス設定値のcanspeedにセット
      setMaskFilter();                        // set mcp mask and filter
      calcLen();                              // calc SWF byte length
      disp.reMappingSw();                     // reMapping Switches
    }
    else disp.changePage();                   // 表示ページを更新
  }
  else if(disp.isMonitorMode()){     // モニターモード時の処理
    // CAN割込があった時はmsgを取得して出力
    if(canrxIntFlag){
      canrxIntFlag = 0;
      // get and display CAN data
      while (CAN.checkReceive() == CAN_MSGAVAIL){     // get CAN msg until empty
        // get CAN message
        getCanMsg(msgSet);                            // get and format CAN message
        // output HardWareFiltered one line with 8bytes
        if(disp.getMonitorScrollType().fMonitorDispSw_.bit.hwfDisp){
          formatMsg1line(msgSet, canString);          // format CAN message to 1line
          disp.postLine(canString);                   // display formatted CAN string
          if(setMan.getSettingValue(AOSET, DS_AOHSW_POS)){
            auxSend(msgSet.buf, msgSet.len);            // send CAN msg to AUX SPI output
          }
        }
        // output SoftWareFiltered some lines with some bytes
        applySoftwareFilter(msgSet);
        if(canFiltVal.fIsFiltered.byte != 0){         // Filtered by the Software filter
          DEBUG_PRINT("Value is Filtered");
          for(int swfNum = 0; swfNum < MUTABLEOBJMAX; swfNum++){
            if((canFiltVal.fIsFiltered.byte >> swfNum) & 1){
              formatMsg1line_filtered(msgSet, canString, swfNum);  // format CAN message to 1line
              if(disp.getMonitorScrollType().fMonitorDispSw_.bit.swfDisp){
                disp.postLine(canString, ILI9341_YELLOW);             // display formatted CAN string
              }
              if(setMan.getSettingValue(AOSET, DS_AOSSW_POS)){
                auxSend_filtered(canFiltVal.value[swfNum], canFiltVal.len[swfNum]);   // send CAN msg to AUX SPI output
              }
              calcComparaterOut(canFiltVal.value[swfNum], swfNum);
            }
          }
        }
      }
    }
  }
  else{
    ;
  }
  
  // Periodic routine for Melody Player
  mplay.update();
  
  // Error detecting every ERRDETPERIOD
  if(errDetTimer.isExpired()){
    if(CAN.checkError(&mcp_error)){
      // notice the MCP error
      digitalWrite(PIN_ERROR, HIGH);                                // error LED on
      Serial.print("MCP error! code= ");Serial.println(mcp_error);  // error message
    }
    else digitalWrite(PIN_ERROR, LOW);
  }

}

// **************************************************************************************
// END of file                                                                          *
// **************************************************************************************