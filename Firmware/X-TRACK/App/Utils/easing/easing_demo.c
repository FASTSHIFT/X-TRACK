/**
 * @file easing_demo.c
 * @author uYanki (https://github.com/uYanki)
 * @brief easing demo
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#if 0

#include "easing.h"
#include <stdio.h>

void easing_demo(void)
{
#if 0
    // easing_t e = easing_create(EASING_DIR_FORWARD | EASING_TIMES_SINGLE, _easing_calc_Linear, 10, 5, 0);
    // easing_t e = easing_create(EASING_DIR_FORWARD | EASING_TIMES_MANYTIMES | (2 << EASING_TIMES_SET), _easing_calc_Linear, 0, 5, 0);
    // easing_t e = easing_create(EASING_DIR_REVERSE | EASING_TIMES_MANYTIMES | (2 << EASING_TIMES_SET), _easing_calc_Linear, 0, 1, 0);
    easing_t e = easing_create(EASING_DIR_BACKANDFORTH | EASING_TIMES_MANYTIMES | (2 << EASING_TIMES_SET), _easing_calc_Linear, 0, 5, 0);
    easing_start_relative(&e, 10);  // 0 [2.5 5 7.5] 10
    while (!easing_isok(&e)) {
        easing_update(&e);
        printf("%f\r\n", easing_curpos(&e));
        // printf("%f\r\n", e.nCurrent);
    }
#endif

    easing_t e = easing_create(EASING_MODE_DEFAULT, _easing_calc_Linear, 0, 400, 0);

    easing_start_absolute(&e, -1.57f, 1.57f);

    char buff[32];
    FILE* f = fopen("data.csv", "w");
    while (!easing_isok(&e)) {
        easing_update(&e);
        float x = easing_curpos(&e);
        snprintf(buff, sizeof(buff),"%f\n", x);
        fputs(buff, f);
    }
    fclose(f);
}

#endif
