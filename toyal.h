// #define TOYAL_IMPLEMENTATION
#ifndef toyal_h_INCLUDED
#define toyal_h_INCLUDED

#include <stdint.h>

typedef struct toyal_rect {
    uint32_t height;
    uint32_t width;
    uint32_t y;
    uint32_t x;
} Toyal_Rect;

typedef enum toyal_error {
    toyal_SUCCESS,
    toyal_INVALID_RANGE,
    toyal_INVALID_ARGS,
} Toyal_Error;

Toyal_Error split_col(Toyal_Rect parent, uint32_t x, Toyal_Rect *child_1, Toyal_Rect *child_2);
Toyal_Error split_row(Toyal_Rect parent, uint32_t y, Toyal_Rect *child_1, Toyal_Rect *child_2);

#ifdef TOYAL_IMPLEMENTATION
Toyal_Error
split_col(Toyal_Rect parent, uint32_t x, Toyal_Rect *child_1, Toyal_Rect *child_2)
{
    if (!child_1 || !child_2) return toyal_INVALID_ARGS;
    if (x > parent.width) return toyal_INVALID_RANGE;
    child_1->x = parent.x;
    child_1->y = parent.y;
    child_1->width = x;
    child_1->height = parent.height;

    // parent has its own pos, x is relative to parent.x
    uint32_t abs_x = x + parent.x;
    child_2->x = abs_x;
    child_2->y = parent.y;
    child_2->width = parent.width - x;
    child_2->height = parent.height;
    return toyal_SUCCESS;
}

Toyal_Error
split_row(Toyal_Rect parent, uint32_t y, Toyal_Rect *child_1, Toyal_Rect *child_2)
{
    if (!child_1 || !child_2) return toyal_INVALID_ARGS;
    if (y > parent.height) return toyal_INVALID_RANGE;

    child_1->x = parent.x;
    child_1->y = parent.y;
    child_1->width  = parent.width;
    child_1->height = y;

    uint32_t abs_y = parent.y + y;
    child_2->x = parent.x;
    child_2->y = abs_y;
    child_2->width  = parent.width;
    child_2->height = parent.height - y;

    return toyal_SUCCESS;
}
#endif

#endif // toyal_h_INCLUDED
