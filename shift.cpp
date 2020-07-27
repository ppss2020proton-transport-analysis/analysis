#include "shift.h"

Shift::Shift() : dx(0), dy(0), dz(0) {}

Shift::Shift(double dx, double dy, double dz)
    : dx(dx), dy(dy), dz(dz) {}

double Shift::GetXShift() const {
  return dx;
}

double Shift::GetYShift() const {
  return dy;
}

double Shift::GetZShift() const {
  return dz;
}
