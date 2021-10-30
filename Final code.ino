#include <LiquidCrystal.h>
const int maxperiod_siz=80; // max number of samples in a period
const int samp_siz=4;



const int sensorPin=A5;
const int analogpin=A1;
const int Threshold1 = 500;
const int Threshold2=900;
volatile int counter=0;
const int Delay=10;
byte beat=LOW;
float oldPulse = 0.0;
const int rise_threshold=6; //Ref AnalogMaxima()
volatile unsigned long lastNoFin, lastPulse, now;



const int REDLed = 7;
const int IRLed = 8;



const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup() {

// put your setup code here, to run once:
Serial.begin(9600);
pinMode(REDLed,OUTPUT);
pinMode(IRLed,OUTPUT);

pinMode(analogpin, INPUT);
pinMode(sensorPin, INPUT);

lcd.begin(16, 2);
lcd.setCursor(0,0);
lcd.print("Testing...");
}



int AnalogMaxima() { //Function to find peak in pulse
int rise_count=0;
int check1;
int check2;
while(!beat) {
check1 = 1023-analogRead(analogpin);
delay(Delay);
check2 = 1023-analogRead(analogpin);

if(check2>check1){
rise_count++;
}

else if(check2<check1) {
if(rise_count> rise_threshold){ //Waveform should be rising for at least
beat=HIGH; //rise_threshold number of times to be counted as maxima
Serial.print(100);
lastPulse = millis();
return millis();
}
rise_count=0;
}
Serial.print("0 1023 "); //Prints reference lines
Serial.println(1023-analogRead(analogpin)); //Prints Pulse Waveform
now = millis();
if(now - lastNoFin > 15000 && now - lastPulse> 3000) //If it detects no peaks for 3 second
{ //and 15 seconds have passed for it to have stabilized
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("NO FINGER");



lastNoFin = millis();
}
}
}



void loop() {
digitalWrite(REDLed,LOW);
digitalWrite(IRLed,HIGH); //IR ON Red OFF
int last_beat, current_beat=0;
int period_sum=0;
int period[10];
int index, samples, samplesCounter;
index=0;
samples=0;
samplesCounter=0;
last_beat= AnalogMaxima();
beat=LOW;
for(int i=0; i<10; i++){ //Storing multiple pulses for calculation
current_beat= AnalogMaxima();
beat=LOW;
period[i]= current_beat-last_beat;
last_beat=current_beat;
}
int c=0;
for(int i=1; i<10; i++) {
if ( (period[i] < period[i-1] * 1.1) &&
(period[i] > period[i-1] / 1.1) ) { //Adjacent pulses should have periods within 10% of each other
c++;
period_sum+= period[i];
}
}
samples=period_sum/(samp_siz*Delay*c); //Number of samples in one time period
float pulse= 60000.0/(period_sum/c);

pulseprint(pulse); //Function to print pulse on LCD



float IRreading[samp_siz], Redreading[samp_siz];
long int now, ptr = 0;
float IRlast, Redlast, reader, start_time;
int n;
for (int i = 0; i < samp_siz; i++){IRreading[i] = 0; Redreading[i]=0;} //Initialization
float IRdata[maxperiod_siz],REDdata[maxperiod_siz];
int Rptr =0;
for (int i = 0; i < maxperiod_siz; i++) { IRdata[i] = 0;REDdata[i]=0;} //Initialization
float IRmax=0, IRmin=0, REDmax=0, REDmin=0, IRsum = 0, Redsum = 0;
double R=0;

while(true){ //Calculating SpO2
//Red OFF IR ON
digitalWrite(REDLed,LOW);
digitalWrite(IRLed,HIGH);
n = 0;
start_time = millis();
reader = 0.0;
do //Taking readings over a fixed time period
{
reader += analogRead (sensorPin);
n++;
now = millis();
}
while (now < start_time + 20);
reader /= n; // Average reading in 20 ms
IRsum -= IRreading[ptr]; //Removing last measurement of moving average
IRsum += reader; //Adding latest measurement to moving average
IRreading[ptr] = reader; //Adding latest measurement
IRlast = IRsum / samp_siz; //Moving average reading
//Serial.print("0 1023 ");
//Serial.println(IRlast);



//IR OFF, Red ON
digitalWrite(IRLed,LOW);
digitalWrite(REDLed,HIGH);



n = 0;
start_time = millis();
reader = 0.0;
do
{
reader += analogRead (sensorPin);
n++;
now = millis();
}
while (now < start_time + 20);
reader /= n;
Redsum -= Redreading[ptr];
Redsum += reader;
Redreading[ptr] = reader;
Redlast = Redsum / samp_siz;
//Serial.print("0 1023 ");
//Serial.println(Redlast);



// R CALCULATION
IRdata[Rptr]=IRlast;
REDdata[Rptr]=Redlast;
Rptr++;
Rptr %= maxperiod_siz;
samplesCounter++;
if(samplesCounter>=samples){ //Finding min and max of IR and RED LED DC values
samplesCounter =0;
IRmax = 0; IRmin=1023; REDmax = 0; REDmin=1023;
for(int i=0;i<maxperiod_siz;i++) {
if( IRdata[i]> IRmax) IRmax = IRdata[i];
if( IRdata[i]>0 && IRdata[i]< IRmin ) IRmin = IRdata[i];
IRdata[i] =0;
if( REDdata[i]> REDmax) REDmax = REDdata[i];
if( REDdata[i]>0 && REDdata[i]< REDmin ) REDmin = REDdata[i];
REDdata[i] =0;
}
R = ( (REDmax-REDmin) / REDmin) / ( (IRmax-IRmin) / IRmin ) ;
break;
}
ptr++;
ptr %= samp_siz;
}
int spo2= 50*R+49.5;
spo2print(spo2);
}



void pulseprint(float pulse){ //Function to print pulse on LCD
if(pulse>40.0 && pulse<220.0) {
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("BPM: "+ String(int(pulse)));
lcd.setCursor(0,1);
lcd.print(" ");
}
else
{
lcd.clear();
lcd.setCursor(0,1);
lcd.print("CALIBRATING");
}
if(pulse <=50 && pulse >40)
{
lcd.setCursor(12,0);
lcd.print("LOW");
}
else if(pulse >=120 && pulse <220){
lcd.setCursor(12,0);
lcd.print("HIGH");
}
else if(pulse <=120 && pulse >50){
lcd.setCursor(12,0);
lcd.print("NORM");
}
}



void spo2print (int spo2){
if(spo2 <90 && spo2 > 70)
{
lcd.setCursor(0,1);
lcd.print("SpO2: "+String(spo2));
lcd.setCursor(12,1);
lcd.print("LOW");
}
else if(spo2>=90 && spo2<=100){
lcd.setCursor(0,1);
lcd.print("SpO2: "+String(spo2));
lcd.setCursor(12,1);
lcd.print("NORM");
}
else if(spo2>100){
lcd.setCursor(0,16);
lcd.print("W.P.");
}
}
