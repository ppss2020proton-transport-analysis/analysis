#ifndef magnet_h
#define magnet_h

#include <string>
#include <iostream>
#include <map>

class Magnet {
public:
  Magnet(const std::string&, int);

  std::string GetType() const;

  int GetId() const;

  int GetNumber() const;

private:
  std::string type = "";
  int id = 0;
  int number = 0;
};

class Dipole : public Magnet {
public:
  Dipole(int);
};

class Quadrupole : public Magnet {
public:
  Quadrupole(int);
};

class VerticalKicker : public Magnet {
public:
  VerticalKicker(int);
};

class HorizontalKicker : public Magnet {
public:
  HorizontalKicker(int);
};

#endif
