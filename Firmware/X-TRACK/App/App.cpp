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
#include "App.h"
#include "Config/Config.h"
#include "Frameworks/PageManager/PageManager.h"
#include "Service/DataProc/DataProc.h"
#include "UI/AppFactory.h"
#include "UI/Resource/ResourcePool.h"
#include "UI/StatusBar/StatusBar.h"

struct AppContext {
    DataBroker* broker;
    DataProc::Global_Helper* global;
    PageManager* manager;
    Page::StatusBar* statusBar;
};

static void App_ParseArgsToEnv(AppContext_t* context, int argc, const char* argv[])
{
    if (argc < 2) {
        return;
    }

    DataProc::Env_Helper env(context->broker->mainNode());

    int i = 1;
    while (i < argc) {
        const char* key = argv[i++];
        const char* value = i < argc ? argv[i++] : nullptr;
        LV_LOG_USER("Setting env: %s = %s", key, value);
        env.set(key, value);
    }

    context->global->publish(DataProc::GLOBAL_EVENT::APP_ARGS_PARSED);
}

AppContext_t* App_CreateContext(int argc, const char* argv[])
{
    AppContext_t* context = new AppContext;

    /* Resource pool manager */
    ResourcePool::init();

    /* Data processor */
    context->broker = new DataBroker("Broker");
    DataProc_Init(context->broker);

    context->global = new DataProc::Global_Helper(context->broker->mainNode());
    context->global->publish(DataProc::GLOBAL_EVENT::DATA_PROC_INIT_FINISHED);

    App_ParseArgsToEnv(context, argc, argv);

    /* Page manager */
    context->manager = new PageManager(AppFactory::getInstance());
    AppFactory::getInstance()->intsallAll(context->manager);
    context->global->publish(DataProc::GLOBAL_EVENT::PAGE_MANAGER_INIT_FINISHED, context->manager);

    /* StatusBar */
    context->statusBar = new Page::StatusBar(context->manager->getLayerTop());
    context->global->publish(DataProc::GLOBAL_EVENT::STATUS_BAR_INIT_FINISHED);

    /* App started */
    context->global->publish(DataProc::GLOBAL_EVENT::APP_STARTED);

    return context;
}

uint32_t App_RunLoopExecute(AppContext_t* context)
{
    LV_ASSERT_NULL(context);
    context->global->publish(DataProc::GLOBAL_EVENT::APP_RUN_LOOP_EXECUTE);
    return context->broker->handleTimer();
}

void App_DestroyContext(AppContext_t* context)
{
    LV_ASSERT_NULL(context);
    context->global->publish(DataProc::GLOBAL_EVENT::APP_STOPPED);

    delete context->manager;
    delete context->statusBar;
    delete context->broker;
    ResourcePool::deinit();
    delete context;
}
