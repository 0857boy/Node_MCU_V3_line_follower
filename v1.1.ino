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
int pwmL = D7, pwmR = D8, i = 0 ; // 接上L298N 之 PWM角位

int r, ml, l, mr, minus = 0 ;
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
  return 130 ;
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
    analogWrite( pwmR, pwm() );
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
    analogWrite( pwmL, pwm() - 40 ) ;
    analogWrite( pwmR, pwm() - 40 ) ;
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




void Handle()
{
  pause() ;
  int lspeed = 0 ;
  int rspeed = 0 ;
  
  while(1)
  {
    minus = 0 ;
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
        i = 3 ;
        while ( i < len )
        {
          if ( packetBuffer[i] == '-' ) 
          {
            minus = 1 ;
          }
          else 
          {
            lspeed *= 10 ;
            lspeed += packetBuffer[i] - '0' ;
          }
          i ++ ;
        }
        if( minus ) lspeed *= -1 ;
      }

      else if ( len >= 3 && packetBuffer[1] == 'B' )
      {
        rspeed = 0 ;
        i = 3 ;
        while ( i < len )
        {
          if ( packetBuffer[i] == '-' ) 
          {
            minus = 1 ;
          }
          else 
          {
            rspeed *= 10 ;
            rspeed += packetBuffer[i] - '0' ;
          }
          i ++ ;
        }
        if( minus ) rspeed *= -1 ;
      }
    }

    if ( strcmp( packetBuffer, "R" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      forward();
      lspeed = 0 ;
      rspeed = 0 ;
    }

    else if ( strcmp( packetBuffer, "B" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      right() ;
      lspeed = 0 ;
      rspeed = 0 ;
    }

    else if ( strcmp( packetBuffer, "A" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      left() ;
      lspeed = 0 ;
      rspeed = 0 ;
    }

    else if ( strcmp( packetBuffer, "L" ) == 0 )
    {
      analogWrite( pwmL ,pwm());
      analogWrite( pwmR ,pwm());
      back() ;
      lspeed = 0 ;
      rspeed = 0 ;
    }

    else if ( strcmp( packetBuffer, "E" ) == 0 )
    {
      lspeed = 0 ;
      rspeed = 0 ;
      pause() ;
    }

    else if ( strcmp( packetBuffer, "I" ) == 0 )

    {
      break ;
    }

    
    
    else if ( rspeed > 20 && lspeed > 20 )
    {
       analogWrite( pwmL ,lspeed + 50 );
       analogWrite( pwmR ,rspeed + 50 );
       forward() ;
    }

    else if( rspeed < -20 && lspeed < -20 )
    {
      analogWrite( pwmL , - lspeed + 50 );
      analogWrite( pwmR , - rspeed + 50 );
      back() ;
    }

    else if ( rspeed < -20 && lspeed > 20 )
    {
      
      right() ;
      analogWrite( pwmL , lspeed + 50 );
      analogWrite( pwmR , -rspeed + 50 );
    }

    else if (  lspeed < -20 && rspeed > 20 )
    {
      
      left() ;
      analogWrite( pwmL , -lspeed + 50 );
      analogWrite( pwmR , rspeed + 50 );
    }
    else if ( lspeed > 20 || rspeed > 20 )
    {
      if ( rspeed > 20 )
      {
        
        forward() ;
        analogWrite( pwmL ,0);
        analogWrite( pwmR ,rspeed + 50 );
      }
      else
      {
        
        forward() ;
        analogWrite( pwmL ,lspeed + 50 );
        analogWrite( pwmR ,0);
      }
    }

    else if ( lspeed < -20 || rspeed < -20 )
    {
      if ( rspeed < -20 )
      {
        
        back() ;
        analogWrite( pwmL ,0);
        analogWrite( pwmR ,  -rspeed + 50 );
      }
      else
      {
        
        back() ;
        analogWrite( pwmL , -lspeed + 50 );
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

  }


  AutoMode() 
    
 } /
