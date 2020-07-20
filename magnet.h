#ifndef magnet_h
#define magnet_h

#include <string>

class Magnet {
public:
  Magnet(const std::string&, int id);

  std::string GetType() const;

  int GetId() const;

private:
  std::string type = "";
  int id = 0;
};

class Dipole : public Magnet {
public:
  Dipole(int id);
};

class Quadrupole : public Magnet {
public:
  Quadrupole(int id);
};

class VerticalKicker : public Magnet {
public:
  VerticalKicker(int id);
};

class HorizontalKicker : public Magnet {
public:
  HorizontalKicker(int id);
};

#endif
