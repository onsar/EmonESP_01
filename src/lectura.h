/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef lectura_h
#define lectura_h

#include "Arduino.h"

class Lectura
{
  public:
    Lectura(String name, String value);
    String dame_name();
    String dame_value();
  private:
    String _name;
    String _value;
};

#endif
