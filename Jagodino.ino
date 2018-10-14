#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>

// Display pinout
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

const int hi = 0; // Relay's inverse logic
const int lo = 1; // Relay's inverse logic

const int ventil = 8; // Relay1 valve
const int pumpa = 9;  // Relay2 pump

const int trig = 10;
const int echo = 11;

const int precnikBureta = 55; // [cm]
const int visinaBureta = 120; // [cm]
const int offset = 15;        // [cm]

const int ww=3; // Font Width
const int hh=5; // Font Height
const int wr=4; // Bar Height
const double pi=3.1415926; // Pi(e) (njami)

const int waterledpin = 0;
const int lowwaterledpin = 1;
const int greskaledpin = 2;

const int zalijbtn = 12;
const int stopbtn = 13;

const int wbd=24; // Width of barrel (in display)
const int hbd=30; // Height of barrel (in display)

const int td=950; // Threshold for sensor
const int punido=90; // Water plants till [%]

const int freqq=15; // Write status in EEPROM every [min]
const int limitminuta=30; // Max watering time [min]

const int adliter=0; // Addreses in EEPROM
const int adelect=10;
const int adrtime=20;

const double snagavalva=4.5+0.7;
const double snagaard=0.75;

void setup() {
  //Serial.begin(9600);
  
  pinMode(ventil,OUTPUT);
  pinMode(pumpa,OUTPUT);
  digitalWrite(ventil,lo);
  digitalWrite(pumpa,lo);
  
  pinMode(waterledpin,OUTPUT);
  pinMode(lowwaterledpin,OUTPUT);
  pinMode(greskaledpin,OUTPUT);
  
  pinMode(zalijbtn,INPUT_PULLUP);
  pinMode(stopbtn,INPUT_PULLUP);
  
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  display.begin();
  display.setContrast(50);
  showFirst();
  delay(5000);
  showStatus();
  delay(15000);
}
bool uklj=false;
unsigned long time1=0, time2=0;
double lp=0;
int ukupnolitaraa=0,ukupnovrijemeuklj=0;

void loop() {
  int a[4],i,m[4],w[4],x0,y0,psz,htbd,minni,avg=0,no=0;
  bool sens[4],zalj=false;
  
  double zap,h,trzap,ht;
  String zp,poruka="JAGODINO  ",smin,vrz="",vrz2="",ltz;
  ht=getDist();
  if(ht<=visinaBureta+offset)
  h=ht;
  else h=visinaBureta+offset;
  for(i=0;i<4;i++){
    a[i]=analogRead(i+14);
    if(a[i]>td)
      sens[i]=true;
    else sens[i]=false;
    m[i]=map(a[i],td,200,0,100);
    m[i]=constrain(m[i],0,100);
    if(!sens[i]){
      avg+=m[i];
      no++;
    }
    w[i]=map(m[i],0,100,0,25);
    //Serial.println("a["+String(i)+"]="+String(a[i]));
  }
  minni=constrain(map(analogRead(A5),1023,0,5,85),10,80);
  zap=pi*precnikBureta*precnikBureta/4*visinaBureta/1000;
  trzap=pi*precnikBureta*precnikBureta/4*(visinaBureta-h+offset)/1000;
  avg/=no;
  psz=100*trzap/zap;
  if(no<4){
    digitalWrite(greskaledpin,1);
  }
  else{
    digitalWrite(greskaledpin,0);
  }
  if((avg<minni && no!=0 && psz>10) || digitalRead(zalijbtn)==LOW){
    time1=millis()/1000;
    lp=trzap;
    uklj=true;
  }
  if((avg>punido || no==0 || millis()/1000-time1>limitminuta*60 || digitalRead(stopbtn)==LOW) && uklj){
    ukupnovrijemeuklj+=millis()/1000-time1;
    ukupnolitaraa+=lp-trzap;
    lp-=trzap;
    uklj=false;
  }
  //Serial.println("\ntrzap = "+String(trzap)+"\nlp = "+String(lp));
  /*Serial.println(h);
  Serial.println();*/

  /*Serial.println(zap);
  Serial.println(trzap);*/
  zp=String(int(trzap))+"L "+String(psz)+"%";
  if(psz<=25){
    poruka="Malo vode";
    digitalWrite(lowwaterledpin,1);
  }
  else{
    digitalWrite(lowwaterledpin,0);
  }
  
  if(psz<=10){
    poruka="Nema vode";
    digitalWrite(lowwaterledpin,1);
  }
  else{
    digitalWrite(lowwaterledpin,0);
  }
  
  if(uklj){
    digitalWrite(ventil,hi);
    poruka="Zalijevanje";
    digitalWrite(waterledpin,1);
  }
  else{
    digitalWrite(ventil,lo);
    digitalWrite(waterledpin,0);
  }
  
  display.clearDisplay();
  //poruka="";
  for(i=0;i<14-poruka.length();i++)
    display.print(" ");
  display.println(poruka);  
  for(i=0;i<14-zp.length();i++)
    display.print(" ");
  display.println(zp);  
  display.println();
  smin=String(minni)+"%";
  for(i=0;i<9-smin.length();i++)
    display.print(" ");
  display.println(smin);
  
  int satiz=(millis()/1000-time1)/3600;
  int minsz=((millis()/1000-time1)%3600)/60;
  if(satiz/10==0)
  vrz+="0";
  vrz+=String(satiz);
  if(minsz/10==0)
  vrz2+="0";
  vrz2+=String(minsz);
  ltz=String(int(lp))+"L";
  if(!uklj && time1!=0){
    /*for(i=0;i<10-vrz2.length();i++)
      display.print(" ");*/

    
    display.setCursor(31,32);
    display.print(vrz);
    display.setCursor(41,32);
    display.print(":");
    display.setCursor(45,32);
    display.println(vrz2);
    for(i=0;i<9-ltz.length();i++)
      display.print(" ");
    display.print(ltz);
    
  }
  x0=0;y0=5;
  for(i=0;i<4;i++){
    if(!sens[i]){
      int posoff;
      if(uklj && i==0)
      posoff=3;
      else posoff=7;
      p(String(m[i])+"%",x0+posoff,y0+i*(wr+hh+2));
      display.drawRect(x0,y0+i*(wr+hh+2)+hh+1,25,wr,BLACK);
      display.fillRect(x0,y0+i*(wr+hh+2)+hh+1,w[i],wr,BLACK);
    }
    else{
      display.setCursor(x0,y0+i*(wr+hh+2)+3);
      display.print("Grska");
      display.drawPixel(x0+4*ww+1,y0+i*(wr+hh+2)+3,BLACK);
      display.drawPixel(x0+4*ww+2,y0+i*(wr+hh+2)+4,BLACK);
      display.drawPixel(x0+4*ww+3,y0+i*(wr+hh+2)+3,BLACK);
      //p(String(m[i])+"%",x0,y0+i*(wr+hh+2));
    }
  }
  display.drawRect(display.width()-wbd,display.height()-hbd,wbd,hbd,BLACK);
  htbd=map(constrain(psz,0,100),0,100,0,hbd);
  display.fillRect(display.width()-wbd,display.height()-htbd,wbd,htbd,BLACK);

  display.drawFastVLine(display.width()-wbd,display.height()-hbd+hbd/5,hbd/5,WHITE);
  display.drawFastVLine(display.width()-wbd,display.height()-hbd+3*hbd/5,hbd/5,WHITE);
  display.drawFastVLine(display.width()-1,display.height()-hbd+hbd/5,hbd/5,WHITE);
  display.drawFastVLine(display.width()-1,display.height()-hbd+3*hbd/5,hbd/5,WHITE);
  
  display.drawFastVLine(display.width()-wbd+1,display.height()-hbd+hbd/5,hbd/5,BLACK);
  display.drawFastVLine(display.width()-wbd+1,display.height()-hbd+3*hbd/5,hbd/5,BLACK);
  display.drawFastVLine(display.width()-2,display.height()-hbd+hbd/5,hbd/5,BLACK);
  display.drawFastVLine(display.width()-2,display.height()-hbd+3*hbd/5,hbd/5,BLACK);
/*
  int literss,energyspent,minutaradi;
    EEPROM.get(adliter,literss);
    EEPROM.get(adelect,energyspent);
    EEPROM.get(adrtime,minutaradi);
    Serial.println("Vrijeme = "+String(minutaradi)+"min\nLiters = "+String(literss)+"L\nEnergy = "+String(energyspent)+"Wh\n");
    minutaradi=0;
    literss=0;
    energyspent=0;
    EEPROM.put(adliter,literss);
    EEPROM.put(adelect,energyspent);
    EEPROM.put(adrtime,minutaradi);
    */
  display.display();
  if(millis()/1000-time2>freqq*60){
    int minutaradi,energyspent,literss;
    EEPROM.get(adliter,literss);
    delay(100);
    EEPROM.get(adelect,energyspent);
    delay(100);
    EEPROM.get(adrtime,minutaradi);
    delay(100);
    
    minutaradi+=freqq;
    literss+=ukupnolitaraa;
    ukupnolitaraa=0;
    
    energyspent+=(freqq*snagaard+ukupnovrijemeuklj*snagavalva)/60;
    ukupnovrijemeuklj=0;
    
    EEPROM.put(adliter,literss);
    delay(100);
    EEPROM.put(adelect,energyspent);
    delay(100);
    EEPROM.put(adrtime,minutaradi);
    delay(100);
    delay(100);
    time2=millis()/1000;
  }
  delay(100);
}

void p(String a, int x, int y){
  int i=0;
  while(a[i]!='\0'){
    if(x+4>83){
      x=0;
      y+=hh+1;
    }
    pr(a[i],x,y);
    x+=ww+1;
    i++;
  }
}
void pr(char a, int x, int y){
  switch(a){
    case '1':
      display.drawFastVLine(x+2,y,hh,BLACK);
      display.drawPixel(x+ww/2,y+hh/2-1,BLACK);
      break;
      
    case '2':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x+ww-1,y,hh/2+1,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      display.drawFastVLine(x,y+hh/2,hh/2+1,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;

    case '3':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x+ww-1,y,hh,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;
     
    case '4':
      display.drawFastVLine(x,y,hh/2+1,BLACK);
      display.drawFastVLine(x+ww-1,y,hh,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      break;
     
    case '5':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x,y,hh/2+1,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      display.drawFastVLine(x+ww-1,y+hh/2,hh/2+1,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;
     
    case '6':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x,y,hh,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      display.drawFastVLine(x+ww-1,y+hh/2,hh/2+1,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;
     
    case '7':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x,y,hh/2+1,BLACK);
      display.drawFastVLine(x+ww-1,y,hh,BLACK);
      break;
     
    case '8':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x,y,hh,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      display.drawFastVLine(x+ww-1,y,hh,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;
     
    case '9':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x,y,hh/2+1,BLACK);
      display.drawFastHLine(x,y+hh/2,ww,BLACK);
      display.drawFastVLine(x+ww-1,y,hh,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;
     
    case '0':
      display.drawFastHLine(x,y,ww,BLACK);
      display.drawFastVLine(x,y,hh,BLACK);
      display.drawFastVLine(x+ww-1,y,hh,BLACK);
      display.drawFastHLine(x,y+hh-1,ww,BLACK);
      break;
     
    case '%':
      display.drawPixel(x,y+hh/5-1,BLACK);
      display.drawLine(x,y+hh-hh/5-1,x+ww-1,y+hh/5,BLACK);
      display.drawPixel(x+ww-1,y+hh-hh/5,BLACK);
      break;

    case ' ':
      break;
     
    default:
      display.fillRect(x,y,ww,hh,BLACK);
  }
}

double getDist(){
  double d,t;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  t = pulseIn(echo, HIGH);
  d = t*0.034/2;
  return d;
}

void showFirst(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("   JAGODINO\n");
  display.setTextSize(2);
  display.println(" HASAK");
  display.setTextSize(1);
  display.print("\n      MAY 2018");
  display.display();
}

void showStatus(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  int minutaradi,energyspent,literss,i;
  EEPROM.get(adliter,literss);
  delay(100);
  EEPROM.get(adelect,energyspent);
  delay(100);
  EEPROM.get(adrtime,minutaradi);
  delay(100);
  
  //display.println("   JAGODINO");
  display.println("Radnih d hh:mm");
  String tt=String(minutaradi/1440)+" ";
  if(((minutaradi%1440)/60)/10==0)
    tt+="0";
  tt+=String((minutaradi%1440)/60)+":";
  if((minutaradi%60)/10==0)
    tt+="0";
  tt+=String(minutaradi%60);
  for(i=0;i<14-tt.length();i++)
    display.print(" ");
  display.println(tt);
  display.println("Zaliveno");
  String ll=String(literss)+" L";
  for(i=0;i<14-ll.length();i++)
    display.print(" ");
  display.println(ll);
  display.println("Utroseno");
  String ee=String(energyspent)+" Wh";
  for(i=0;i<14-ee.length();i++)
    display.print(" ");
  display.println(ee);
  display.display();
}

