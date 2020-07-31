#ifndef shift_h
#define shift_h

class Shift {
public:
  Shift();

  Shift(double, double, double);

  double GetXShift() const;

  double GetYShift() const;

  double GetZShift() const;

private:
  double dx; 
  double dy; 
  double dz;
};

#endif
