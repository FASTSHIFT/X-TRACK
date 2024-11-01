/*
 * MIT License
 * Copyright (c) 2024 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __BINDING_H
#define __BINDING_H

template <class T, class CONTEXT>
class Binding {
public:
    typedef void (*SET_FUNC)(CONTEXT*, T);
    typedef T (*GET_FUNC)(CONTEXT*);

public:
    Binding(T v, SET_FUNC setFunc, GET_FUNC getFunc, CONTEXT* ctx)
    {
        Binding(setFunc, getFunc, ctx);
        set(v);
    }

    Binding(SET_FUNC setFunc, GET_FUNC getFunc, CONTEXT* ctx)
        : _setFunc(setFunc)
        , _getFunc(getFunc)
        , _ctx(ctx)
    {
    }

    Binding()
        : _setFunc(nullptr)
        , _getFunc(nullptr)
        , _ctx(nullptr)
    {
    }

    void setCallback(SET_FUNC setFunc, GET_FUNC getFunc, CONTEXT* ctx)
    {
        _setFunc = setFunc;
        _getFunc = getFunc;
        _ctx = ctx;
    }

    void set(T v)
    {
        _setFunc(_ctx, v);
    }

    T get() const
    {
        return _getFunc(_ctx);
    }

    operator T() const
    {
        return get();
    }

    Binding& operator=(T v)
    {
        set(v);
        return *this;
    }

private:
    SET_FUNC _setFunc;
    GET_FUNC _getFunc;
    CONTEXT* _ctx;
};

#endif /* __BINDING_H */
