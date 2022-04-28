#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define APSSID "KennyNude"
#define APPSK  "034561596"
WiFiUDP Udp;
unsigned int UDPPort = 8888; 
char packetBuffer[255];
int packetSize ;
int len ;
int sL = D1 ; // 左邊感測器
int sR = D2 ; // 右側感測器
int smL = D9 ; // 中左邊感測器
int smR = D10 ; // 中右側感測器
int motorL1 = D3, motorL2 = D4 ; //  連接L298N IN1 與 IN2
int motorR1 = D5, motorR2 = D6 ; // 連接L298N IN3 與 IN4
int pwmL = D7, pwmR = D8 ; // 接上L298N 之 PWM角位

int r, ml, l, mr ;
void setup()
{
  WiFi.softAP(APSSID, APPSK);
  Udp.begin(UDPPort);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(sL, INPUT) ;
  pinMode(sR, INPUT) ;
  pinMode(motorL1, OUTPUT) ;
  pinMode(motorL2, OUTPUT) ;
  pinMode(motorR1, OUTPUT) ;
  pinMode(motorR2, OUTPUT) ;
  pinMode(pwmL, OUTPUT) ;
  pinMode(pwmR, OUTPUT) ;
  Handle() ;
  
}

int pwm()
{
  return 250 ;
}
void forward()
{
  digitalWrite(motorL1, HIGH) ;
  digitalWrite(motorL2, LOW) ;
  digitalWrite(motorR1, HIGH) ;
  digitalWrite(motorR2, LOW) ;
}

void left()
{
  digitalWrite(motorL1, LOW) ;
  digitalWrite(motorL2, HIGH) ;
  digitalWrite(motorR1, HIGH) ;
  digitalWrite(motorR2, LOW) ;
}

void right()
{
  digitalWrite(motorL1, HIGH) ;
  digitalWrite(motorL2, LOW) ;
  digitalWrite(motorR1, LOW) ;
  digitalWrite(motorR2, HIGH) ;
}

void back()
{
  digitalWrite(motorL1, LOW) ;
  digitalWrite(motorL2, HIGH) ;
  digitalWrite(motorR1, LOW) ;
  digitalWrite(motorR2, HIGH) ;
}

void pause()
{
  analogWrite( pwmL ,0);
  analogWrite( pwmR ,0);
  digitalWrite(motorL1, LOW) ;
  digitalWrite(motorL2, LOW) ;
  digitalWrite(motorR1, LOW) ;
  digitalWrite(motorR2, LOW) ;
}
void AutoMode()
{
  r = digitalRead(sR);
  l = digitalRead(sL);
  ml = digitalRead(smL);
  mr = digitalRead(smR);
  if( r == 1 )
  {
    analogWrite( pwmL, pwm() ) ;
    analogWrite( pwmR, pwm());
    right() ;
    while( ml == 0 )
    {
      ml = digitalRead(smL) ;
    }
  }
  else if ( l == 1 )
  { 
    analogWrite( pwmL, pwm() ) ;
    analogWrite( pwmR, pwm() ) ;
    left() ;
    while( mr == 0 )
    {
      mr = digitalRead(smR) ;
    }
  }


  else if ( ( mr == 1 && ml == 1 ) || ( mr == 0 && ml == 0 ) )
  {
    analogWrite( pwmL, pwm() ) ;
    analogWrite( pwmR, pwm() ) ;
    forward() ;
  }

  else if ( mr == 1 )
  {
    analogWrite( pwmL, pwm() ) ;
    analogWrite( pwmR, pwm() ) ;
    right() ;
  }

  else if ( ml == 1 )
  {
    analogWrite( pwmL, pwm() ) ;
    analogWrite( pwmR, pwm() ) ;
    left() ;
  }

}


void celebrate()
{
  int b = random(4);
  if(b == 0 ) forward();
  else if(b==1) right();
  else if(b==2) left();
  else back();

}


void Handle()
{
  pause() ;
  int lspeed = 40 ;
  int rspeed = 40 ;
  
  while(1)
  {
    packetSize = Udp.parsePacket() ;
    if (packetSize)
    {
      len = Udp.read(packetBuffer, 255);
      if (len > 0) {
              packetBuffer[len] = 0 ;
      }
      if ( len >= 3 && packetBuffer[1] == 'T' )
      {
        lspeed = 0 ;
        if( len == 5 )
        {
          lspeed += packetBuffer[3] - '0' ;
          lspeed *= 10 ;
          lspeed += packetBuffer[4] - '0' ;
        }
        else if ( len == 6 )
        {
          lspeed += packetBuffer[3] - '0' ;
          lspeed *= 10 ;
          lspeed += packetBuffer[4] - '0' ;
          lspeed *= 10 ;
          lspeed += packetBuffer[5] - '0' ;
        }
        else 
        {
          lspeed += packetBuffer[3] - '0' ;
        }
      }

      else if ( len >= 3 && packetBuffer[1] == 'B' )
      {
        rspeed = 0 ;
        if( len == 5 )
        {
          rspeed += packetBuffer[3] - '0' ;
          rspeed *= 10 ;
          rspeed += packetBuffer[4] - '0' ;
        }

        else if ( len == 6 )
        {
          rspeed += packetBuffer[3] - '0' ;
          rspeed *= 10 ;
          rspeed += packetBuffer[4] - '0' ;
          rspeed *= 10 ;
          rspeed += packetBuffer[5] - '0' ;
        }
        else 
        {
          rspeed += packetBuffer[3] - '0' ;
        }
      }
    }

    if ( strcmp( packetBuffer, "R" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      forward();
      delay(5) ;
      lspeed = 40 ;
      rspeed = 40 ;
    }

    else if ( strcmp( packetBuffer, "B" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      right() ;
      delay(5) ;
      lspeed = 40 ;
      rspeed = 40 ;
    }

    else if ( strcmp( packetBuffer, "A" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      left() ;
      delay(1) ;
      lspeed = 40 ;
      rspeed = 40 ;
    }

    else if ( strcmp( packetBuffer, "L" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      back() ;
      delay(1) ;
      lspeed = 40 ;
      rspeed = 40 ;
    }

    else if ( strcmp( packetBuffer, "E" ) == 0 )
    {
      lspeed = 40 ;
      rspeed = 40 ;
      pause() ;
    }

    else if ( strcmp( packetBuffer, "I" ) == 0 )

    {
      break ;
    }

    
    
    else if ( rspeed > 50 && lspeed > 50 )
    {
       analogWrite( pwmL ,lspeed + 100);
       analogWrite( pwmR ,rspeed + 100);
       forward() ;
    }

    else if( rspeed < 30 && lspeed < 30 )
    {
      analogWrite( pwmL ,( lspeed - 80 ) * -1 + 100);
      analogWrite( pwmR ,( rspeed - 80 ) * -1 + 100);
      back() ;
    }

    else if ( rspeed < 30 && lspeed > 50 )
    {
      
      right() ;
      analogWrite( pwmL ,lspeed + 100);
      analogWrite( pwmR , ( rspeed - 80 ) * -1 + 100);
    }

    else if (  lspeed < 30 && rspeed > 50 )
    {
      
      left() ;
      analogWrite( pwmL , ( lspeed - 80 ) * -1 + 100);
      analogWrite( pwmR ,rspeed + 100);
    }
    else if ( lspeed > 50 || rspeed > 50 )
    {
      if ( rspeed > 50 )
      {
        
        forward() ;
        analogWrite( pwmL ,0);
        analogWrite( pwmR ,rspeed + 100);
      }
      else
      {
        
        forward() ;
        analogWrite( pwmL ,lspeed + 100);
        analogWrite( pwmR ,0);
      }
    }

    else if ( lspeed < 30 || rspeed < 30 )
    {
      if ( rspeed <= 30 )
      {
        
        back() ;
        analogWrite( pwmL ,0);
        analogWrite( pwmR ,( rspeed - 80 ) * -1 + 100);
      }
      else
      {
        
        back() ;
        analogWrite( pwmL ,( lspeed - 80 ) * -1 + 100);
        analogWrite( pwmR ,0);
      }
    }
    else 
    {
      pause() ;
    }
    
  }

   
}

void loop()
{
  packetSize = Udp.parsePacket() ;
  if (packetSize)
  {
    len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0 ;
    }

    if( strcmp( packetBuffer, "F" ) == 0 )
    {
      Handle() ;
    }

    else if ( strcmp( packetBuffer, "H" ) == 0 )
    {
      
      while( 1 )
      {
        celebrate() ;
        delay(50) ;
      }
    }
  }


  AutoMode() ;
}
