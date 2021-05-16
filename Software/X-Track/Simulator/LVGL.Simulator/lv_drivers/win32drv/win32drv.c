/**
 * @file win32drv.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "win32drv.h"

#if USE_WIN32DRV

#include <windowsx.h>
#include <VersionHelpers.h>

#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

#define WINDOW_EX_STYLE \
    WS_EX_CLIENTEDGE

#define WINDOW_STYLE \
    (WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME))

#ifndef WIN32DRV_MONITOR_ZOOM
#define WIN32DRV_MONITOR_ZOOM 1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * @brief Creates a B8G8R8A8 frame buffer.
 * @param WindowHandle A handle to the window for the creation of the frame
 *                     buffer. If this value is NULL, the entire screen will be
 *                     referenced.
 * @param Width The width of the frame buffer.
 * @param Height The height of the frame buffer.
 * @param PixelBuffer The raw pixel buffer of the frame buffer you created.
 * @param PixelBufferSize The size of the frame buffer you created.
 * @return If the function succeeds, the return value is a handle to the device
 *         context (DC) for the frame buffer. If the function fails, the return
 *         value is NULL, and PixelBuffer parameter is NULL.
*/
static HDC lv_win32_create_frame_buffer(
    _In_opt_ HWND WindowHandle,
    _In_ LONG Width,
    _In_ LONG Height,
    _Out_ UINT32** PixelBuffer,
    _Out_ SIZE_T* PixelBufferSize);

/**
 * @brief Enables WM_DPICHANGED message for child window for the associated
 *        window.
 * @param WindowHandle The window you want to enable WM_DPICHANGED message for
 *                     child window.
 * @return If the function succeeds, the return value is non-zero. If the
 *         function fails, the return value is zero.
 * @remarks You need to use this function in Windows 10 Threshold 1 or Windows
 *          10 Threshold 2.
*/
static BOOL lv_win32_enable_child_window_dpi_message(
    _In_ HWND WindowHandle);

static void lv_win32_display_driver_flush_callback(
    lv_disp_drv_t* disp_drv,
    const lv_area_t* area,
    lv_color_t* color_p);

static void lv_win32_display_driver_rounder_callback(
    lv_disp_drv_t* disp_drv,
    lv_area_t* area);

static void lv_win32_mouse_driver_read_callback(
    lv_indev_drv_t* indev_drv,
    lv_indev_data_t* data);

static void lv_win32_keyboard_driver_read_callback(
    lv_indev_drv_t* indev_drv,
    lv_indev_data_t* data);

static void lv_win32_mousewheel_driver_read_callback(
    lv_indev_drv_t* indev_drv,
    lv_indev_data_t* data);

static LRESULT CALLBACK lv_win32_window_message_callback(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam);

#if LV_VERSION_CHECK(8, 0, 0)
static void lv_win32_message_handler(
    lv_timer_t* param);
#else
static void lv_win32_message_handler(
    lv_task_t* param);
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

EXTERN_C bool lv_win32_quit_signal = false;

/**********************
 *  STATIC VARIABLES
 **********************/

static HINSTANCE g_instance_handle = NULL;
static HWND g_window_handle = NULL;

static HDC g_buffer_dc_handle = NULL;
static UINT32* g_pixel_buffer = NULL;
static SIZE_T g_pixel_buffer_size = 0;

static lv_disp_t* g_display = NULL;

static bool volatile g_mouse_pressed = false;
static LPARAM volatile g_mouse_value = 0;

static bool volatile g_mousewheel_pressed = false;
static int16_t volatile g_mousewheel_value = 0;

static bool volatile g_keyboard_pressed = false;
static WPARAM volatile g_keyboard_value = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

EXTERN_C bool lv_win32_init(
    HINSTANCE instance_handle,
    int show_window_mode,
    lv_coord_t hor_res,
    lv_coord_t ver_res,
    HICON icon_handle)
{
    WNDCLASSEXW WindowClass;

    WindowClass.cbSize = sizeof(WNDCLASSEX);

    WindowClass.style = 0;
    WindowClass.lpfnWndProc = lv_win32_window_message_callback;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = instance_handle;
    WindowClass.hIcon = icon_handle;
    WindowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = L"lv_sim_visual_studio";
    WindowClass.hIconSm = icon_handle;

    if (!RegisterClassExW(&WindowClass))
    {
        return false;
    }

    g_instance_handle = instance_handle;

    RECT NewWindowSize;

    NewWindowSize.left = 0;
    NewWindowSize.right = hor_res * WIN32DRV_MONITOR_ZOOM;
    NewWindowSize.top = 0;
    NewWindowSize.bottom = ver_res * WIN32DRV_MONITOR_ZOOM;

    AdjustWindowRectEx(
        &NewWindowSize,
        WINDOW_STYLE,
        FALSE,
        WINDOW_EX_STYLE);
    OffsetRect(
        &NewWindowSize,
        -NewWindowSize.left,
        -NewWindowSize.top);

    g_window_handle = CreateWindowExW(
        WINDOW_EX_STYLE,
        WindowClass.lpszClassName,
        L"LVGL Simulator for Windows Desktop",
        WINDOW_STYLE,
        CW_USEDEFAULT,
        0,
        NewWindowSize.right,
        NewWindowSize.bottom,
        NULL,
        NULL,
        instance_handle,
        NULL);

    if (!g_window_handle)
    {
        return false;
    }

#if LV_VERSION_CHECK(8, 0, 0)
    lv_timer_create(lv_win32_message_handler, 0, NULL);
#else
    lv_task_create(lv_win32_message_handler, 0, LV_TASK_PRIO_HIGHEST, NULL);
#endif

    lv_win32_enable_child_window_dpi_message(g_window_handle);

    HDC hNewBufferDC = lv_win32_create_frame_buffer(
        g_window_handle,
        hor_res,
        ver_res,
        &g_pixel_buffer,
        &g_pixel_buffer_size);

    DeleteDC(g_buffer_dc_handle);
    g_buffer_dc_handle = hNewBufferDC;

#if LV_VERSION_CHECK(8, 0, 0)
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(
        &disp_buf,
        (lv_color_t*)malloc(hor_res * ver_res * sizeof(lv_color_t)),
        NULL,
        hor_res * ver_res);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = hor_res;
    disp_drv.ver_res = ver_res;
    disp_drv.flush_cb = lv_win32_display_driver_flush_callback;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.rounder_cb = lv_win32_display_driver_rounder_callback;
    g_display = lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_win32_mouse_driver_read_callback;
    lv_indev_drv_register(&indev_drv);

    static lv_indev_drv_t kb_drv;
    lv_indev_drv_init(&kb_drv);
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read_cb = lv_win32_keyboard_driver_read_callback;
    lv_indev_drv_register(&kb_drv);

    static lv_indev_drv_t enc_drv;
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = lv_win32_mousewheel_driver_read_callback;
    lv_indev_drv_register(&enc_drv);
#else
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(
        &disp_buf,
        (lv_color_t*)malloc(hor_res * ver_res * sizeof(lv_color_t)),
        NULL,
        hor_res * ver_res);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = hor_res;
    disp_drv.ver_res = ver_res;
    disp_drv.flush_cb = lv_win32_display_driver_flush_callback;
    disp_drv.buffer = &disp_buf;
    disp_drv.rounder_cb = lv_win32_display_driver_rounder_callback;
    g_display = lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_win32_mouse_driver_read_callback;
    lv_indev_drv_register(&indev_drv);

    lv_indev_drv_t kb_drv;
    lv_indev_drv_init(&kb_drv);
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read_cb = lv_win32_keyboard_driver_read_callback;
    lv_indev_drv_register(&kb_drv);

    lv_indev_drv_t enc_drv;
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = lv_win32_mousewheel_driver_read_callback;
    lv_indev_drv_register(&enc_drv);
#endif

    ShowWindow(g_window_handle, show_window_mode);
    UpdateWindow(g_window_handle);

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static HDC lv_win32_create_frame_buffer(
    HWND WindowHandle,
    LONG Width,
    LONG Height,
    UINT32** PixelBuffer,
    SIZE_T* PixelBufferSize)
{
    HDC hFrameBufferDC = NULL;

    if (PixelBuffer && PixelBufferSize)
    {
        HDC hWindowDC = GetDC(WindowHandle);
        if (hWindowDC)
        {
            hFrameBufferDC = CreateCompatibleDC(hWindowDC);
            ReleaseDC(WindowHandle, hWindowDC);
        }

        if (hFrameBufferDC)
        {
            BITMAPINFO BitmapInfo = { 0 };
            BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            BitmapInfo.bmiHeader.biWidth = Width;
            BitmapInfo.bmiHeader.biHeight = -Height;
            BitmapInfo.bmiHeader.biPlanes = 1;
            BitmapInfo.bmiHeader.biBitCount = 32;
            BitmapInfo.bmiHeader.biCompression = BI_RGB;

            HBITMAP hBitmap = CreateDIBSection(
                hFrameBufferDC,
                &BitmapInfo,
                DIB_RGB_COLORS,
                (void**)PixelBuffer,
                NULL,
                0);
            if (hBitmap)
            {
                *PixelBufferSize = Width * Height * sizeof(UINT32);
                DeleteObject(SelectObject(hFrameBufferDC, hBitmap));
                DeleteObject(hBitmap);
            }
            else
            {
                DeleteDC(hFrameBufferDC);
                hFrameBufferDC = NULL;
            }
        }
    }

    return hFrameBufferDC;
}

static BOOL lv_win32_enable_child_window_dpi_message(
    HWND WindowHandle)
{
    // This hack is only for Windows 10 only.
    if (!IsWindowsVersionOrGreater(10, 0, 0))
    {
        return FALSE;
    }

    // We don't need this hack if the Per Monitor Aware V2 is existed.
    OSVERSIONINFOEXW OSVersionInfoEx = { 0 };
    OSVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    OSVersionInfoEx.dwBuildNumber = 14393;
    if (VerifyVersionInfoW(
        &OSVersionInfoEx,
        VER_BUILDNUMBER,
        VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL)))
    {
        return FALSE;
    }

    HMODULE ModuleHandle = GetModuleHandleW(L"user32.dll");
    if (!ModuleHandle)
    {
        return FALSE;
    }

    typedef BOOL(WINAPI* FunctionType)(HWND, BOOL);

    FunctionType pFunction = (FunctionType)(
        GetProcAddress(ModuleHandle, "EnableChildWindowDpiMessage"));
    if (!pFunction)
    {
        return FALSE;
    }

    return pFunction(WindowHandle, TRUE);
}

static void lv_win32_display_driver_flush_callback(
    lv_disp_drv_t* disp_drv,
    const lv_area_t* area,
    lv_color_t* color_p)
{
#if LV_COLOR_DEPTH == 32
    UNREFERENCED_PARAMETER(area);
    memcpy(g_pixel_buffer, color_p, g_pixel_buffer_size);
#else
    for (int y = area->y1; y <= area->y2; ++y)
    {
        for (int x = area->x1; x <= area->x2; ++x)
        {
            g_pixel_buffer[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
            color_p++;
        }
    }
#endif

    HDC hWindowDC = GetDC(g_window_handle);
    if (hWindowDC)
    {
        StretchBlt(
            hWindowDC,
            0,
            0,
            disp_drv->hor_res * WIN32DRV_MONITOR_ZOOM,
            disp_drv->ver_res * WIN32DRV_MONITOR_ZOOM,
            g_buffer_dc_handle,
            0,
            0,
            disp_drv->hor_res,
            disp_drv->ver_res,
            SRCCOPY);

        ReleaseDC(g_window_handle, hWindowDC);
    }

    lv_disp_flush_ready(disp_drv);
}

static void lv_win32_display_driver_rounder_callback(
    lv_disp_drv_t* disp_drv,
    lv_area_t* area)
{
    area->x1 = 0;
    area->x2 = disp_drv->hor_res - 1;
    area->y1 = 0;
    area->y2 = disp_drv->ver_res - 1;
}

static void lv_win32_mouse_driver_read_callback(
    lv_indev_drv_t* indev_drv,
    lv_indev_data_t* data)
{
    UNREFERENCED_PARAMETER(indev_drv);

    data->state = (lv_indev_state_t)(
        g_mouse_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);
    data->point.x = GET_X_LPARAM(g_mouse_value) / WIN32DRV_MONITOR_ZOOM;
    data->point.y = GET_Y_LPARAM(g_mouse_value) / WIN32DRV_MONITOR_ZOOM;
}

static void lv_win32_keyboard_driver_read_callback(
    lv_indev_drv_t* indev_drv,
    lv_indev_data_t* data)
{
    UNREFERENCED_PARAMETER(indev_drv);

    data->state = (lv_indev_state_t)(
        g_keyboard_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);

    WPARAM KeyboardValue = g_keyboard_value;

    switch (KeyboardValue)
    {
    case VK_UP:
        data->key = LV_KEY_UP;
        break;
    case VK_DOWN:
        data->key = LV_KEY_DOWN;
        break;
    case VK_LEFT:
        data->key = LV_KEY_LEFT;
        break;
    case VK_RIGHT:
        data->key = LV_KEY_RIGHT;
        break;
    case VK_ESCAPE:
        data->key = LV_KEY_ESC;
        break;
    case VK_DELETE:
        data->key = LV_KEY_DEL;
        break;
    case VK_BACK:
        data->key = LV_KEY_BACKSPACE;
        break;
    case VK_RETURN:
        data->key = LV_KEY_ENTER;
        break;
    case VK_NEXT:
        data->key = LV_KEY_NEXT;
        break;
    case VK_PRIOR:
        data->key = LV_KEY_PREV;
        break;
    case VK_HOME:
        data->key = LV_KEY_HOME;
        break;
    case VK_END:
        data->key = LV_KEY_END;
        break;
    default:
        if (KeyboardValue >= 'A' && KeyboardValue <= 'Z')
        {
            KeyboardValue += 0x20;
        }

        data->key = (uint32_t)KeyboardValue;

        break;
    }
}

static void lv_win32_mousewheel_driver_read_callback(
    lv_indev_drv_t* indev_drv,
    lv_indev_data_t* data)
{
    UNREFERENCED_PARAMETER(indev_drv);

    data->state = (lv_indev_state_t)(
        g_mousewheel_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);
    data->enc_diff = g_mousewheel_value;
    g_mousewheel_value = 0;
}

static LRESULT CALLBACK lv_win32_window_message_callback(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    {
        g_mouse_value = lParam;
        if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
        {
            g_mouse_pressed = (uMsg == WM_LBUTTONDOWN);
        }
        else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
        {
            g_mousewheel_pressed = (uMsg == WM_MBUTTONDOWN);
        }
        return 0;
    }
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        g_keyboard_pressed = (uMsg == WM_KEYDOWN);
        g_keyboard_value = wParam;
        break;
    }
    case WM_MOUSEWHEEL:
    {
        g_mousewheel_value = -(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
        break;
    }
    case WM_DPICHANGED:
    {
        LPRECT SuggestedRect = (LPRECT)lParam;

        SetWindowPos(
            hWnd,
            NULL,
            SuggestedRect->left,
            SuggestedRect->top,
            SuggestedRect->right,
            SuggestedRect->bottom,
            SWP_NOZORDER | SWP_NOACTIVATE);

        RECT ClientRect;
        GetClientRect(hWnd, &ClientRect);

#if LV_VERSION_CHECK(8, 0, 0)
        int WindowWidth = g_display->driver->hor_res;
        int WindowHeight = g_display->driver->ver_res;
#else
        int WindowWidth = g_display->driver.hor_res;
        int WindowHeight = g_display->driver.ver_res;
#endif

        SetWindowPos(
            hWnd,
            NULL,
            SuggestedRect->left,
            SuggestedRect->top,
            SuggestedRect->right + (WindowWidth - ClientRect.right),
            SuggestedRect->bottom + (WindowHeight - ClientRect.bottom),
            SWP_NOZORDER | SWP_NOACTIVATE);

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

#if LV_VERSION_CHECK(8, 0, 0)
static void lv_win32_message_handler(
    lv_timer_t* param)
#else
static void lv_win32_message_handler(
    lv_task_t* param)
#endif
{
    UNREFERENCED_PARAMETER(param);

    MSG Message;
    BOOL Result = PeekMessageW(&Message, NULL, 0, 0, TRUE);
    if (Result != 0 && Result != -1)
    {
        TranslateMessage(&Message);
        DispatchMessageW(&Message);

        if (Message.message == WM_QUIT)
        {
            lv_win32_quit_signal = true;
        }
    }
}

#endif /*USE_WIN32DRV*/
