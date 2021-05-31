/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
#include "DataCenter.h"
#include "DataCenterLog.h"
#include <string.h>
#include <algorithm>

#define DC_USE_AUTO_CLOSE 0

DataCenter::DataCenter(const char* name) 
    : AccountMain(name, this)
{
    Name = name;
}

DataCenter::~DataCenter()
{
#if DC_USE_AUTO_CLOSE
    DC_LOG_INFO("DataCenter[%s] closing...", Name);
    while (!AccountPool.empty())
    {
        Account* account = AccountPool.back();
        
        DC_LOG_INFO("Delete: %s", account->ID);
        delete account;

        AccountPool.pop_back();
    }
    DC_LOG_INFO("DataCenter[%s] closed.", Name);
#endif
}

Account* DataCenter::SearchAccount(const char* id)
{
    return Find(&AccountPool, id);
}

Account* DataCenter::Find(std::vector<Account*>* vec, const char* id)
{
    for(auto iter : *vec)
    {
        if (strcmp(id, iter->ID) == 0)
        {
            return iter;
        }
    }
    return nullptr;
}

bool DataCenter::AddAccount(Account* account)
{
    if (account == &AccountMain)
    {
        return false;
    }

    if (SearchAccount(account->ID) != nullptr)
    {
        DC_LOG_ERROR("Multi add Account[%s]", account->ID);
        return false;
    }

    AccountPool.push_back(account);

    AccountMain.Subscribe(account->ID);

    return true;
}

bool DataCenter::RemoveAccount(Account* account)
{
    return Remove(&AccountPool, account);
}

bool DataCenter::Remove(std::vector<Account*>* vec, Account* account)
{
    auto iter = std::find(vec->begin(), vec->end(), account);

    if (iter == vec->end())
    {
        DC_LOG_ERROR("Account[%s] was not found", account->ID);
        return false;
    }

    vec->erase(iter);
    
    return true;
}

uint32_t DataCenter::GetAccountLen()
{
    return AccountPool.size();
}
