/*  mcp25625_can.h
    Modified by FundyLab, 2025
*/
/*
    mcp_can.h
    2012 Copyright (c) Seeed Technology Inc.  All right reserved.

    Author:Loovee (loovee@seeed.cc)
    2014-1-16

    Contributor:

    Cory J. Fowler
    Latonita
    Woodward1
    Mehtajaghvi
    BykeBlast
    TheRo0T
    Tsipizic
    ralfEdmund
    Nathancheek
    BlueAndi
    Adlerweb
    Btetz
    Hurvajs
    ttlappalainen

    The MIT License (MIT)

    Copyright (c) 2013 Seeed Technology Inc.

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
#ifndef _MCP25625_H_
#define _MCP25625_H_

#include "mcp_can.h"
#include "mcp25625_can_dfs.h"

#define MAX_CHAR_IN_MESSAGE 8

class mcp25625_can : public MCP_CAN
{
public:
    mcp25625_can(byte _CS) : MCP_CAN(_CS), nReservedTx(0){};
    /*
        MCP25625 driver function
    */
public:
    virtual void enableTxInterrupt(bool enable = true); // enable transmit interrupt
    virtual void reserveTxBuffers(byte nTxBuf = 0)
    {
        nReservedTx = (nTxBuf < MCP_N_TXBUFFERS ? nTxBuf : MCP_N_TXBUFFERS - 1);
    }
    virtual byte getLastTxBuffer()
    {
        return MCP_N_TXBUFFERS - 1; // read index of last tx buffer
    }
    virtual byte begin(uint32_t speedset, const byte clockset = MCP_16MHz);                                                                             // init can
    virtual byte begin_noSPIset(uint32_t speedset, const byte clockset = MCP_16MHz);                                                                    // init can with no SPI begin (for multiple SPI setting)
    virtual byte init_Mask(byte num, byte ext, unsigned long ulData);                                                                                   // init Masks
    virtual byte init_Filt(byte num, byte ext, unsigned long ulData);                                                                                   // init filters
    virtual void setSleepWakeup(byte enable);                                                                                                           // Enable or disable the wake up interrupt (If disabled the MCP25625 will not be woken up by CAN bus activity, making it send only)
    virtual byte sleep();                                                                                                                               // Put the MCP25625 in sleep mode
    virtual byte wake();                                                                                                                                // Wake MCP25625 manually from sleep
    virtual byte setMode(byte opMode);                                                                                                                  // Set operational mode
    virtual byte getMode();                                                                                                                             // Get operational mode
    virtual byte checkError(uint8_t* err_ptr = NULL);                                                                                                   // if something error

    virtual byte checkReceive(void);                                                                                                                    // if something received
    virtual byte readMsgBufID(byte status, volatile unsigned long *id, volatile byte *ext, volatile byte *rtr, volatile byte *len, volatile byte *buf); // read buf with object ID
    /* wrapper */
    byte readMsgBufID(unsigned long *ID, byte *ext, byte *len, byte *buf){
        return readMsgBufID(readRxTxStatus(), ID, ext, &rtr, len, buf);
    }
    byte readMsgBufID(unsigned long *ID, byte *len, byte *buf) {
        return readMsgBufID(readRxTxStatus(), ID, &ext_flg, &rtr, len, buf);
    }
    byte readMsgBuf(byte *len, byte *buf) {
        return readMsgBufID(readRxTxStatus(), &can_id, &ext_flg, &rtr, len, buf);
    }

    virtual byte trySendMsgBuf(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, byte iTxBuf = 0xff);                                 // as sendMsgBuf, but does not have any wait for free buffer
    virtual byte sendMsgBuf(byte status, unsigned long id, byte ext, byte rtrBit, byte len, volatile const byte *buf);                                  // send message buf by using parsed buffer status
    virtual byte sendMsgBuf(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, bool wait_sent = true);                                 // send buf


    virtual void clearBufferTransmitIfFlags(byte flags = 0);                                                                                            // Clear transmit flags according to status
    virtual byte readRxTxStatus(void);                                                                                                                  // read has something send or received
    virtual byte checkClearRxStatus(byte *status);                                                                                                      // read and clear and return first found rx status bit
    virtual byte checkClearTxStatus(byte *status, byte iTxBuf = 0xff);                                                                                  // read and clear and return first found or buffer specified tx status bit
    virtual bool mcpPinMode(const byte pin, const byte mode);                                                                                           // switch supported pins between HiZ, interrupt, output or input
    virtual bool mcpDigitalWrite(const byte pin, const byte mode);                                                                                      // write HIGH or LOW to RX0BF/RX1BF
    virtual byte mcpDigitalRead(const byte pin);

private:
    void mcp25625_reset(void); // reset mcp25625

    byte mcp25625_readRegister(const byte address); // read mcp25625's register

    void mcp25625_readRegisterS(const byte address,
                               byte values[],
                               const byte n);
    void mcp25625_setRegister(const byte address, // set mcp25625's register
                             const byte value);

    void mcp25625_setRegisterS(const byte address, // set mcp25625's registers
                              const byte values[],
                              const byte n);

    void mcp25625_initCANBuffers(void);

    void mcp25625_modifyRegister(const byte address, // set bit of one register
                                const byte mask,
                                const byte data);

    byte mcp25625_readStatus(void);                                  // read mcp25625's Status
    byte mcp25625_setCANCTRL_Mode(const byte newmode);               // set mode
    byte mcp25625_requestNewMode(const byte newmode);                // Set mode
    byte mcp25625_configRate(const byte canSpeed, const byte clock); // set baudrate
    byte mcp25625_init(const byte canSpeed, const byte clock);       // mcp25625init

    void mcp25625_write_id(const byte mcp_addr, // write can id
                          const byte ext,
                          const unsigned long id);

    void mcp25625_read_id(const byte mcp_addr, // read can id
                         byte *ext,
                         unsigned long *id);

    void mcp25625_write_canMsg(const byte buffer_sidh_addr, unsigned long id, byte ext, byte rtr, byte len,
                              volatile const byte *buf); // read can msg
    void mcp25625_read_canMsg(const byte buffer_load_addr, volatile unsigned long *id, volatile byte *ext,
                             volatile byte *rtr, volatile byte *len, volatile byte *buf); // write can msg
    void mcp25625_start_transmit(const byte mcp_addr);                                     // start transmit
    byte mcp25625_getNextFreeTXBuf(byte *txbuf_n);                                         // get Next free txbuf
    byte mcp25625_isTXBufFree(byte *txbuf_n, byte iBuf);                                   // is buffer by index free

    /*
        can operator function
    */

    byte sendMsg(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, bool wait_sent = true); // send message
private:
    byte nReservedTx; // Count of tx buffers for reserved send
};

struct canMessageSet{
  uint32_t id;
  byte ext;
  byte len;
  byte buf[MAX_CHAR_IN_MESSAGE];
};

#endif
/*********************************************************************************************************
    END FILE
*********************************************************************************************************/
