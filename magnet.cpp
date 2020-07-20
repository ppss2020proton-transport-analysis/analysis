#include "magnet.h"

Magnet::Magnet(const std::string& type, int id) 
  : type(type), id(id) {}

  std::string Magnet::GetType() const {
  return type;
}

int Magnet::GetId() const {
  return id;
}

Dipole::Dipole(int id) 
  : Magnet("dipole", id) {}

Quadrupole::Quadrupole(int id) 
  : Magnet("quadrupole", id) {}

VerticalKicker::VerticalKicker(int id) 
  : Magnet("vertical_kicker", id) {}

HorizontalKicker::HorizontalKicker(int id) 
  : Magnet("horizontal_kicker", id) {}
