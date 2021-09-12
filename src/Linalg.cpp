
#include <cmath>

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
    const float discr = b * b - 4 * a * c;
    if (discr < 0)
        return false;
    else if (discr == 0) {
        x0 = x1 = -0.5f * b / a;
    } else {
        const float q = -0.5f * (b + sqrt(discr) * ((b > 0) * 2 - 1));
        x0 = q / a;
        x1 = c / q;
    }
    return true;
}
