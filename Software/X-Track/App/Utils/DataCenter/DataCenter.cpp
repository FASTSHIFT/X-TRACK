/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#include "DataCenter.h"
#include "DataCenterLog.h"
#include <string.h>
#include <algorithm>

/* Configure whether to automatically clear all accounts */
#define DC_USE_AUTO_CLOSE 0

/**
  * @brief  Data center constructor
  * @param  name: The name of the data center
  * @retval None
  */
DataCenter::DataCenter(const char* name)
    : AccountMain(name, this)
{
    Name = name;
}

/**
  * @brief  Data center destructor
  * @param  None
  * @retval None
  */
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

/**
  * @brief  Search account
  * @param  id: Account ID
  * @retval If the search is successful, return the pointer of the account
  */
Account* DataCenter::SearchAccount(const char* id)
{
    return Find(&AccountPool, id);
}

/**
  * @brief  Search account in vector
  * @param  vec: Pointer to vector
  * @param  id:  Account ID
  * @retval If the search is successful, return the pointer of the account
  */
Account* DataCenter::Find(Account::AccountVector_t* vec, const char* id)
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

/**
  * @brief  Add an account to the account pool
  * @param  account: Pointer to account
  * @retval If the addition is successful, return true
  */
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

/**
  * @brief  Remove the account from the account pool
  * @param  account: Pointer to account
  * @retval Return true if the removal is successful
  */
bool DataCenter::RemoveAccount(Account* account)
{
    return Remove(&AccountPool, account);
}

/**
  * @brief  Remove account in vector
  * @param  vec: Pointer to vector
  * @param  id:  Account ID
  * @retval Return true if the removal is successful
  */
bool DataCenter::Remove(Account::AccountVector_t* vec, Account* account)
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

/**
  * @brief  Get the number of accounts in the account pool
  * @param  None
  * @retval Number of accounts
  */
size_t DataCenter::GetAccountLen()
{
    return AccountPool.size();
}
