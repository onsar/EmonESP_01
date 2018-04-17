/*
 * -------------------------------------------------------------------
 * EmonESP Serial to Emoncms gateway
 * -------------------------------------------------------------------
 * Adaptation of Chris Howells OpenEVSE ESP Wifi
 * by Trystan Lea, Glyn Hudson, OpenEnergyMonitor
 * All adaptation GNU General Public License as below.
 *
 * -------------------------------------------------------------------
 *
 * This file is part of OpenEnergyMonitor.org project.
 * EmonESP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * EmonESP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with EmonESP; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "emonesp.h"
#include "config.h"
#include "wifi.h"
#include "web_server.h"
#include "ota.h"
#include "input.h"
#include "emoncms.h"
#include "mqtt.h"
#include "lectura.h"
#include "power_measurement.h"

uint32_t t_last_tx=0;
uint32_t current_time= 0;
Lectura lectura("inten","2.00");
Power_measurement Prometeo;
int8_t measurement_status;

// -------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------
void setup() {
  delay(2000);

  Serial.begin(115200);
#ifdef DEBUG_SERIAL1
  Serial1.begin(115200);
#endif

  DEBUG.println();
  DEBUG.print("EmonESP ");
  DEBUG.println(ESP.getChipId());
  DEBUG.println("Firmware: "+ currentfirmware);

  // Read saved settings from the config
  config_load_settings();

  // Initialise the WiFi
  wifi_setup();

  // Bring up the web server
  web_server_setup();

  // Start the OTA update systems
  ota_setup();

  DEBUG.println("Server started");

  delay(100);


  Serial.println();
  Serial.println();

  Prometeo.config(1.125, 0.330, 0.32, 0.32, 0.03322, 0.03322, 0.03322);
  Serial.println("Medición en...3");
  delay(1000);
  Serial.println("...2");
  delay(1000);
  Serial.println("...1");
  delay(1000);

} // end setup

// -------------------------------------------------------------------
// LOOP
// -------------------------------------------------------------------


void loop()
{

  current_time= millis();

  ota_loop();
  web_server_loop();
  wifi_loop();




  if ((current_time - t_last_tx) > 10000)
  {
    // String input_i = lectura.dame_name() + ":" + lectura.dame_value();
    String string_real_power =  String(Prometeo.realPower[0], 2);
    String input_i = "power_0:" + String(Prometeo.realPower[0], 2)
                  + ",power_1:" + String(Prometeo.realPower[1], 2)
                  + ",power_2:" + String(Prometeo.realPower[2], 2)
                  ;
    t_last_tx = current_time;
    Serial.println(current_time);
    emoncms_publish(input_i);



    Serial.println("=========================================================");
    measurement_status = Prometeo.calcVI(20,2000); 	// Realiza la medida. (Nº de pasos por cero, timeout para sincronizar el paso por 0)
    if (measurement_status == 0)
    {
  		Prometeo.serialprint(SOCKET1);    // Muestra los datos (Vrms, Irms, potencia activa, potencia aparente, factor de potencia)
  		Prometeo.serialprint(SOCKET2);    // Muestra los datos (Vrms, Irms, potencia activa, potencia aparente, factor de potencia)
  		Prometeo.serialprint(SOCKET3);    // Muestra los datos (Vrms, Irms, potencia activa, potencia aparente, factor de potencia
      Serial.println("prometeo_irms_0");
      Serial.println(Prometeo.Irms[0]);
      Serial.println("prometeo_real_power_0");
      Serial.println(Prometeo.realPower[0]);
      Serial.println("prometeo_aparent_power_0");
      Serial.println(Prometeo.apparentPower[0]);

    }
  	else
  	{
  		Serial.print("Valor devuelto: ");
  		Serial.println(measurement_status, DEC);
  	}


  }



  String input = "";
  boolean gotInput = input_get(input);

  if (wifi_mode == WIFI_MODE_STA || wifi_mode == WIFI_MODE_AP_AND_STA)
  {
    if(emoncms_apikey != 0 && gotInput) {
      emoncms_publish(input);
    }
    if(mqtt_server != 0)
    {
      mqtt_loop();
      if(gotInput) {
        mqtt_publish(input);
      }
    }
  }
} // end loop
