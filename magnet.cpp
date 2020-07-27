#include "magnet.h"

Magnet::Magnet(const std::string& type, int id, double position=0) 
  : type(type), id(id), position(position) 
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

Dipole::Dipole(int id, double pos) 
  : Magnet("RBEND", id, pos) {}

Quadrupole::Quadrupole(int id, double pos) 
  : Magnet("QUADRUPOLE", id, pos) {}

VerticalKicker::VerticalKicker(int id, double pos) 
  : Magnet("VKICKER", id, pos) {}

HorizontalKicker::HorizontalKicker(int id, double pos) 
  : Magnet("HKICKER", id, pos) {}
