#include "magnet.h"

const std::map<std::string, int> magnet_name_to_number = {{"dipole(1)", 1}, {"dipole(2)", 2}, 
                                                          {"quadrupole(1)", 3}, {"quadrupole(2)", 4}, 
                                                          {"quadrupole(3)", 5}, {"quadrupole(4)", 6},
                                                          {"quadrupole(5)", 7}, {"quadrupole(6)", 8},
                                                          {"vertical_kicker(1)", 9}, {"vertical_kicker(2)", 10},
                                                          {"vertical_kicker(3)", 11}, {"vertical_kicker(4)", 12}, 
                                                          {"vertical_kicker(5)", 13}, {"horizontal_kicker(1)", 14}, 
                                                          {"horizontal_kicker(2)", 15}, {"horizontal_kicker(3)", 16}, 
                                                          {"horizontal_kicker(4)", 17}, {"horizontal_kicker(5)", 18}};

Magnet::Magnet(const std::string& type, int id) 
  : type(type), id(id) 
  {
    std::string name = type + "(" + std::to_string(id) + ")"; 
    number = magnet_name_to_number.at(name);
  }

  std::string Magnet::GetType() const {
  return type;
}

int Magnet::GetId() const {
  return id;
}

int Magnet::GetNumber() const {
  return number;
}

Dipole::Dipole(int id) 
  : Magnet("dipole", id) {}

Quadrupole::Quadrupole(int id) 
  : Magnet("quadrupole", id) {}

VerticalKicker::VerticalKicker(int id) 
  : Magnet("vertical_kicker", id) {}

HorizontalKicker::HorizontalKicker(int id) 
  : Magnet("horizontal_kicker", id) {}
