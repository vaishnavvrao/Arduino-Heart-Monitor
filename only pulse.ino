int analogpin=A5;
volatile int counter=0;
int Delay=10;
byte beat=LOW;

int rise_threshold=7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(analogpin, INPUT);

}

int AnalogMaxima() {
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
      if(rise_count> rise_threshold){
        beat=HIGH;
        //Serial.print(100);
        return millis();
      }
      rise_count=0;
    }   
  Serial.print("0 1023 "); 
  Serial.println(1023-analogRead(analogpin));  
  }
  
}

void loop() {
  int last_beat, current_beat=0;
  int period_sum=0;
  int period[10];
  last_beat= AnalogMaxima();
  beat=LOW;
  
  for(int i=0; i<10; i++){
    current_beat= AnalogMaxima();
    beat=LOW;
    period[i]= current_beat-last_beat;
    last_beat=current_beat;
  }
  int c=0;
  for(int i=1; i<10; i++) {
    if ( (period[i] <  period[i-1] * 1.1)  &&  
                    (period[i] >  period[i-1] / 1.1)  ) {
                       c++;
                       period_sum+= period[i];
                    }
  }

 
  float pulse= 60000.0/(period_sum/c);
  if(pulse>40.0 && pulse<220.0) {Serial.println(pulse);}
}
