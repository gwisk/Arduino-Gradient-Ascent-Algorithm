#include <ResponsiveAnalogRead.h> // For input smoothing
#include <Adafruit_MCP4725.h> // Library for the DAC, digital-to-analog converter, which lets us output analog voltage
#include <Adafruit_ADS1015.h> // Library for the ADC, analog-to-digital converter, which lets us achieve 15 bits of input precision, as opposed to the default of 10
#include <Wire.h> // Library for I2C communication with the ADC and DAC
Adafruit_ADS1115 adc; 
Adafruit_MCP4725 dac;

ResponsiveAnalogRead analog(0, true); // Constructor for input smoothing library

double vStep = 5.0; // This defines how large your step interval is, this is 6 mV approximately
double vLess; // vLess is the power recorded at current voltage output - vStep
double vMore; // vMore is the power recorded at current voltage output + vStep
double voltage = 0; // starting voltage
int i; // used for logging, feel free to remove

void setup() {

  analog.enableEdgeSnap(); // This allows us to hit the edges of the voltage range more easily (aka, 0 V and max V)
  analog.setAnalogResolution(32765); // ADC is 15 bits, so we have to specify that
  
  adc.begin(); 
  adc.setGain(GAIN_TWO); // Our power meter only inputs 2 V, so we use this method so that the ADC only accepts 0 to 2.048 V
  dac.begin(0x62);

  Serial.begin(9600);
  Serial.println("CLEARSHEET");
  Serial.println("LABEL, Time, Serial Input, Arduino Output");
}

void loop() {
/* In the block below, we take the current voltage and subtract vStep
 * The Arduino outputs this voltage and saves it
 */
  dac.setVoltage(voltage - vStep, false);
  vLess = adc.readADC_SingleEnded(0); 
  analog.update(vLess);
  vLess = (analog.getValue()/8.0);

/* In the block below, we take the current voltage and add vStep
 * The Arduino outputs this voltage and saves it
 */
  dac.setVoltage(voltage + vStep, false);
  vMore = adc.readADC_SingleEnded(0);
  analog.update(vMore);
  vMore = (analog.getValue()/8.0);
  
/* Now, the Arduino compares the two values. Which is larger?
 *  If vMore > vLess, that means an increase in voltage is increasing our output, so we're
 *  below the maximum. So, increase your voltage.
 * 
 *  But, if vMore < vLess, that means an increase in voltage is decreasing our output,
 *  so we're past the maximum. So, decrease the voltage.
 */
 
  if(vMore > vLess) {
    voltage += vStep;
  }
  if(vMore < vLess)  {
    voltage -= vStep;
  }

  Serial.print("DATA,TIMER,");
  Serial.print(vLess);
  Serial.print(",");
  Serial.println(voltage);

  if(i==2000)
    Serial.println("PAUSELOGGING");
    i++;  
  delay(200);
}
