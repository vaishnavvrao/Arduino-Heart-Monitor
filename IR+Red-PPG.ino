#define samp_siz 4
#define rise_threshold 5
// Pulse Monitor Test Script
int sensorPin = A5;
int REDLed = 7;
int IRLed = 8;
void setup() {
   Serial.begin(9600);
  pinMode(REDLed,OUTPUT);
  pinMode(IRLed,OUTPUT);
}
void loop ()
{
  float IRreading[samp_siz], Redreading[samp_siz], IRsum, Redsum;
  long int now, ptr;
  float IRlast, Redlast, reader, start_time;
  int n;
  for (int i = 0; i < samp_siz; i++){IRreading[i] = 0; Redreading[i]=0;}
  IRsum = 0;
  Redsum=0;
  ptr = 0;
   while(1)
   {
     
     digitalWrite(REDLed,LOW);
     digitalWrite(IRLed,HIGH);
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
     reader /= n;  // we got an average
     // Add the newest measurement to an array
     // and subtract the oldest measurement from the array
     // to maintain a sum of last measurements
     IRsum -= IRreading[ptr];
     IRsum += reader;
     IRreading[ptr] = reader;
     IRlast = IRsum / samp_siz;
     //Serial.print("0 1023 ");
     //Serial.println(analogRead(sensorPin));
     
     
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
     reader /= n;  // we got an average
     // Add the newest measurement to an array
     // and subtract the oldest measurement from the array
     // to maintain a sum of last measurements
     Redsum -= Redreading[ptr];
     Redsum += reader;
     Redreading[ptr] = reader;
     Redlast = Redsum / samp_siz;
     //Serial.print("0 1023 ");
     
     Serial.print(IRlast);
     Serial.print(",");
     Serial.println(Redlast);
      
     ptr++;
     ptr %= samp_siz;
   }
} 
