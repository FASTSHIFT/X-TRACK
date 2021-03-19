/*
 * Print.h - Base class that provides print() and println()
 * Copyright (c) 2008 David A. Mellis.  All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * Modified 12 April 2011 by Marti Bolivar <mbolivar@leaflabs.com>
 */

#ifndef _PRINT_H_
#define _PRINT_H_

#include "libmaple_types.h"
#include "WString.h"
#include "Printable.h"

#define SUPPORTS_PRINTF
#define PRINTF_BUFFER_LENGTH 128

enum
{
    BYTE = 0,
    BIN  = 2,
    OCT  = 8,
    DEC  = 10,
    HEX  = 16
};

class Print
{
public:
    virtual size_t write(uint8_t ch) = 0;
    virtual size_t write(const char *str);
    virtual size_t write(const void *buf, uint32_t len);

    size_t print(const String &);
    size_t print(char);
    size_t print(const char[]);
    size_t print(uint8, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(long long, int = DEC);
    size_t print(unsigned long long, int = DEC);
    size_t print(double, int = 2);
    size_t print(const __FlashStringHelper *);
    size_t print(const Printable&);
    size_t println(void);
    size_t println(const String &s);
    size_t println(char);
    size_t println(const char[]);
    size_t println(uint8, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(long long, int = DEC);
    size_t println(unsigned long long, int = DEC);
    size_t println(double, int = 2);
    size_t println(const __FlashStringHelper *);
    size_t println(const Printable&);
#ifdef SUPPORTS_PRINTF
// Roger Clark. Work in progress to add printf support
    int printf(const char * format, ...);
#endif
    Print() : write_error(0) {}

    int getWriteError()
    {
        return write_error;
    }
    void clearWriteError()
    {
        setWriteError(0);
    }

protected:
    void setWriteError(int err = 1)
    {
        write_error = err;
    }

private:
    int write_error;
    size_t printNumber(unsigned long long, uint8);
    size_t printFloat(double, uint8);
};

template<class T> inline Print &operator << (Print &obj, T arg)
{
    obj.print(arg);
    return obj;
}

#endif
