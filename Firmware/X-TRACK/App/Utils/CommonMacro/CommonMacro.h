/**
 * @file CommonMacro.h
 * @author _VIFEXTech (vifextech@foxmail.com)
 * @brief General macro definition method library
 * @version 1.8
 * @date 2023-02-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __COMMON_MACRO_H
#define __COMMON_MACRO_H

// 2019-03-21 v1.0 Add notes
// 2019-03-21 v1.1 Add __ValueCloseTo
// 2019-05-16 v1.2 Add __ExecuteexprWithTimeout
// 2019-05-16 v1.3 Add __ValueStep
// 2019-09-25 v1.4 Add __ExecuteOnce
// 2020-01-27 v1.5 Add __SemaphoreTake
// 2020-03-10 v1.6 Add __ValuePlus
// 2022-03-14 v1.7 Rename APIs: CM_XXX; __Sizeof -> __ARRAY_SIZE; Add __EventMonitor
// 2023-02-04 v1.8 Refactoring; use English comments

#ifndef CM_TICK_GET
#ifdef ARDUINO
#define CM_TICK_GET() millis()
#endif
#endif

/**
 * @brief  Variable Watcher, which fires an event when a variable changes
 * @param  now: monitored variables (integer)
 * @param  expr: an expression triggered by an event
 */
#define CM_VALUE_MONITOR(now, expr) \
    do {                            \
        static int last = (now);    \
        if (last != (now)) {        \
            expr;                   \
            last = (now);           \
        }                           \
    } while (0)

/**
 * @brief  Make a variable approach a specified value in designed steps
 * @param  src: controlled variable
 * @param  dest: value being approximated
 * @param  step: step length
 */
#define CM_VALUE_CLOSE_TO(src, dest, step) \
    do {                                   \
        if ((src) < (dest)) {              \
            (src) += (step);               \
        } else if ((src) > (dest)) {       \
            (src) -= (step);               \
        }                                  \
    } while (0)

/**
 * @brief  Let a variable increase or subtract a value,
 *         starting from 0 when it is greater than or equal to the maximum value,
 *         and starting from the maximum value after being lower than 0
 * @param  src: controlled variable.
 * @param  step: step length
 * @param  max: maximum value
 */
#define CM_VALUE_STEP(src, step, max)     \
    ((src) = (((step) >= 0)               \
             ? (((src) + (step)) % (max)) \
             : (((src) + (max) + (step)) % (max))))

/**
 * @brief  Let a variable increase or decrease a value,
 *         start from the minimum value after it is greater than the maximum value,
 *         and start from the maximum value after it is less than the minimum value
 * @param  src: controlled variable
 * @param  plus: value added
 * @param  min: minimum value
 * @param  max: maximum value
 */
#define CM_VALUE_PLUS(src, plus, min, max) \
    do {                                   \
        int cm_tmp_value = (src);          \
        cm_tmp_value += (plus);            \
        if (cm_tmp_value < (min)) {        \
            cm_tmp_value = (max);          \
        } else if (cm_tmp_value > (max)) { \
            cm_tmp_value = (min);          \
        }                                  \
        (src) = cm_tmp_value;              \
    } while (0)

/**
 * @brief  Get constraint value
 * @param  x: original value
 * @param  min: minimum value
 * @param  max: maximum value
 * @retval constraint value
 */
#define CM_CONSTRAIN(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

/**
 * @brief  Restrict a value to a range
 * @param  x: original value
 * @param  min: minimum value
 * @param  max: maximum value
 */
#define CM_VALUE_LIMIT(x, min, max) ((x) = CM_CONSTRAIN((x), (min), (max)))

/**
 * @brief  Linearly maps a range of changes in one value to another
 * @param  x: mapped value
 * @param  in_min: the minimum value to be mapped
 * @param  in_max: the maximum value to be mapped
 * @param  out_min: the minimum value of the map output
 * @param  out_max: the maximum value of the map output
 * @retval mapped value
 */
#define CM_VALUE_MAP(x, in_min, in_max, out_min, out_max) \
    (((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

/**
 * @brief  Non-blocking execution of an expression at specified time intervals
 * @param  expr: executed expression
 * @param  time: time interval
 */
#define CM_EXECUTE_INTERVAL(expr, time)           \
    do {                                          \
        static unsigned long lasttime = 0;        \
        if (CM_TICK_GET() - lasttime >= (time)) { \
            expr;                                 \
            lasttime = CM_TICK_GET();             \
        };                                        \
    } while (0)

/**
 * @brief  Repeated call expression
 * @param  expr: executed expression
 * @param  n: number of calls
 */
#define CM_EXECUTE_LOOP(expr, n)                  \
    do {                                          \
        for (unsigned long i = 0; i < (n); i++) { \
            expr;                                 \
        }                                         \
    } while (0)

/**
 * @brief  ִExecutes an expression until the exprtion returns
 *         the specified value without timing out
 * @param  expr: executed expression
 * @param  retval: expected exprtion return value
 * @param  timeout: time out
 * @param  is_timeout: Provide variables externally to check whether timeout
 */
#define CM_EXECUTE_WITH_TIMEOUT(expr, n, timeout, is_timeout) \
    do {                                                      \
        unsigned long start = CM_TICK_GET();                  \
        (is_timeout) = false;                                 \
        while (CM_TICK_GET() - start < (timeout)) {           \
            if (expr == (n)) {                                \
                (is_timeout) = true;                          \
                break;                                        \
            }                                                 \
        }                                                     \
    } while (0)

/**
 * @brief  Execute the expression only once
 * @param  expr: executed expression
 */
#define CM_EXECUTE_ONCE(expr)             \
    do {                                  \
        static bool cm_is_inited = false; \
        if (!cm_is_inited) {              \
            expr;                         \
            cm_is_inited = true;          \
        }                                 \
    } while (0)

/**
 * @brief  Get the semaphore and execute the expression once when sem is true
 * @param  sem: semaphore (bool)
 * @param  expr: executed expression
 */
#define CM_EXECUTE_SEM_TAKE(sem, expr) \
    do {                               \
        if ((sem)) {                   \
            expr;                      \
            (sem) = false;             \
        }                              \
    } while (0)

/**
 * @brief  Get the number of elements in the array
 * @param  arr: array
 * @retval the number of elements in the array
 */
#define CM_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief  Force conversion of a value, often used for structure copy
 * @param  type: cast type
 * @param  data: data being cast
 * @retval transformed data
 */
#define CM_TYPE_CAST(type, data) (*((type*)(&(data))))

#endif
