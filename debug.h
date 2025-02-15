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

#ifndef _DEBUG_H_
#define _DEBUG_H_

// Debug level setting
#define DEBUG_LEVEL 0  // 0: デバッグ無効, 1: 基本デバッグ, 2: 詳細デバッグ

// デバッグ出力用のマクロ定義
#if DEBUG_LEVEL >= 2
  #define DEBUG2_PRINT(x)  Serial.print(x)
  #define DEBUG2_PRINTLN(x)  Serial.println(x)
  #define DEBUG2_PRINT2LN(x, y)  Serial.println(x,y)
  #define DEBUG_PRINT(x)  Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#elif DEBUG_LEVEL >= 1
  #define DEBUG2_PRINT(x)
  #define DEBUG2_PRINTLN(x)
  #define DEBUG2_PRINT2LN(x, y)
  #define DEBUG_PRINT(x)  Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG2_PRINT(x)
  #define DEBUG2_PRINTLN(x)
  #define DEBUG2_PRINT2LN(x, y)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif



#endif