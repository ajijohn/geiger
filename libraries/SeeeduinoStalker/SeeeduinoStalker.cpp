/*
  SeeeduinoStalker.h
  2012 Copyright (c) Seeed Technology Inc.  All right reserved.

  Author:Loovee
  2016-7-26

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

#include "SeeeduinoStalker.h"

// read voltage of battery
float Stalker::readBattery()
{
    float vol = 0;
    int value = 0;
    
    for(int i=0; i<32; i++)
    {
        value += analogRead(PIN_BAT_ADC);
    }
    
    value >>= 5;
    
    vol = (float)value/1023.0*3.3;      
    vol = vol*6.0;
    
    return vol;
}

// 0: no battery
// 1: charging
// 2: charge done
int Stalker::readChrgStatus()
{

    long value = 0;
    
    // get a average value
    for(int i=0; i<256; i++)
    {
        value += analogRead(PIN_CHRG_ST);
    }
    
    value >>= 8;
    
    //return value;
    
    if(value>185 & value<230)return 1;
    else if(value>125 & value<135)return 2;
    else return 0;
}

void Stalker::setTime(time t)
{
    
}

void Stalker::getTime(time &t)
{
    
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
