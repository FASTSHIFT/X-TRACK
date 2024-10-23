#include <GPX.h>

GPX myGPX;

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  String dumbString("");
}

void loop(){
  Serial.print(myGPX.getOpen());
  myGPX.setMetaDesc("foofoofoo");
  myGPX.setName("track name");
  myGPX.setDesc("Track description");
  myGPX.setSrc("SUP500Ff");
  Serial.print(myGPX.getMetaData());
  Serial.print(myGPX.getTrakOpen());
  Serial.print(myGPX.getInfo());
  Serial.print(myGPX.getTrakSegOpen());
  Serial.print(myGPX.getPt(GPX_TRKPT,"41.123","-71.456"));
  Serial.print(myGPX.getPt(GPX_TRKPT,"42.123","-72.456"));
  Serial.print(myGPX.getPt(GPX_TRKPT,"43.123","-73.456"));
  Serial.print(myGPX.getTrakSegClose());
  Serial.print(myGPX.getTrakClose());
  Serial.print(myGPX.getClose());
  delay(10000);
}
