/*
 * GPX Library -- GPX.h
 * Created by: Ryan Sutton
 *
 * Copyright (c) 2010, Ryan M Sutton
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Ryan M Sutton nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GPX_h
#define GPX_h

#ifdef  ARDUINO
#include "WString.h"
#else
#include "../WString/WString.h"
#endif //  ARDUINO

// 'Public' Tags
#define GPX_TRKPT               "trkpt"
#define GPX_WPT                 "wpt"
#define GPX_RTEPT               "rtept"

class GPX
{
public:
    GPX();
    String getOpen();
    String getClose();
    String getMetaData();
    String getTrakOpen();
    String getTrakClose();
    String getTrakSegOpen();
    String getTrakSegClose();
    String getInfo();
    String getPt(String typ, String lon, String lat);
    String getPt(String typ, String lon, String lat, String ele);
    void setMetaName(String name);
    void setMetaDesc(String desc);
    void setName(String name);
    void setDesc(String desc);
    void setEle(String ele);
    void setSym(String sym);
    void setSrc(String src);
    void setTime(String time);
private:
    //Variables
    String _metaName;
    String _metaDesc;
    String _name;
    String _desc;
    String _ele;
    String _sym;
    String _src;
    String _time;

    //Functions
    String wrapCDATA(String input);
};

#endif

