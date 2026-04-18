#include "Vector2D.h"

// konstruktor wektora 2D, nic specjalnego
Vector2D Vector2D_Construct(float dx, float dy) {
        Vector2D v;
        v.x = dx;
        v.y = dy;

        return v;
}
