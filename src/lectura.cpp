/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "lectura.h"

Lectura::Lectura(String name, String value)
  {
    _name = name;
    _value = value;
  }

  String Lectura::dame_name() { return _name;}
  String Lectura::dame_value() { return _value; }
