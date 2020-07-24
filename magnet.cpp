#include "magnet.h"

Magnet::Magnet(const std::string& type, int id) 
  : type(type), id(id), position(0) 
  {
    name = type + std::to_string(id);
  }

  std::string Magnet::GetType() const {
  return type;
}

int Magnet::GetId() const {
  return id;
}

void Magnet::SetPosition(double position_) {
  position = position_;
} 

double Magnet::GetPosition() const {
  return position;
}

std::string Magnet::GetName() const {
  return name;
}

Dipole::Dipole(int id) 
  : Magnet("RBEND", id) {}

Quadrupole::Quadrupole(int id) 
  : Magnet("QUADRUPOLE", id) {}

VerticalKicker::VerticalKicker(int id) 
  : Magnet("VKICKER", id) {}

HorizontalKicker::HorizontalKicker(int id) 
  : Magnet("HKICKER", id) {}
