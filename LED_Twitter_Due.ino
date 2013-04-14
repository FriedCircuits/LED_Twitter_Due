/*
LEDGRAM Twitter Style v.1
Based on LEDGram Code 
Twitter Code from
http://arduino.cc/forum/index.php/topic,93480.0.html
Created: 03/19/2013
Updated: 03/19/2013
by: William Garrido
3x 8x8 LED Maxtrix Control
Takes a twitter hashtag feed and displays it on a LED Matrix.
*/


#include <LedControl.h> //  need the library\
#include <SPI.h>
#include <Ethernet.h>
#include <TextFinder.h>
#include <QueueList.h>

//Ethernet Stuff
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE4 };
EthernetClient client;
char TwitterHashtag[] = "tindie"; //change this to your own twitter hashtag, or follow arduino ;-)
String hashTag = "#tindie"; //For removing from tweet
char tweet[140];
char serverName[] = "search.twitter.com";  // twitter URL

unsigned long delayTwitter = 60000; //Time in milliseconds to connect to Twitter
unsigned long delayTime = millis();
unsigned long displayLastTime = millis();

const unsigned int tweetHistoryTime = 10000; //Time in hours 10000 being 1 hour Note: Doesn't account for midnight

int tweetCount = 0; //Keep track of numer of tweets received during last connection
int displayCycleTime = 10000; //Time betweek LED messages refresh, recalculated each Twitter connection
const int minDisplayTime = 5000; //Min time a tweet will be displayed on LED, if rearched, not all tweets may be displayed, timing could be adjusted.

bool firstBoot = true; 

const int devCount = 8;
bool scrollFlag = true;
int scrollSpeed = 0;
int brightness = 15;
bool ledShutdown = false;
const int thermPin = A5;
const int NUMSAMPLES = 10;

//Current mode, 0-Message, 1-Temp, 2-Date, 4-Weather, 5-Status
int currMode = 0; //Set startup mode
const int maxMode = 5; 

//Queue for Twitter Messages
QueueList <String> tweetMsg;


byte Font8x5[104*8] =
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x02, 0x00,
        0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A, 0x00,
        0x0E, 0x15, 0x05, 0x0E, 0x14, 0x15, 0x0E, 0x00,
        0x13, 0x13, 0x08, 0x04, 0x02, 0x19, 0x19, 0x00,
        0x06, 0x09, 0x05, 0x02, 0x15, 0x09, 0x16, 0x00,
        0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x02, 0x01, 0x01, 0x01, 0x02, 0x04, 0x00,
        0x01, 0x02, 0x04, 0x04, 0x04, 0x02, 0x01, 0x00,
        0x00, 0x0A, 0x15, 0x0E, 0x15, 0x0A, 0x00, 0x00,
        0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x01,
        0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x00,
        0x0E, 0x11, 0x19, 0x15, 0x13, 0x11, 0x0E, 0x00,
        0x04, 0x06, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00,
        0x0E, 0x11, 0x10, 0x0C, 0x02, 0x01, 0x1F, 0x00,
        0x0E, 0x11, 0x10, 0x0C, 0x10, 0x11, 0x0E, 0x00,
        0x08, 0x0C, 0x0A, 0x09, 0x1F, 0x08, 0x08, 0x00,
        0x1F, 0x01, 0x01, 0x0E, 0x10, 0x11, 0x0E, 0x00,
        0x0C, 0x02, 0x01, 0x0F, 0x11, 0x11, 0x0E, 0x00,
        0x1F, 0x10, 0x08, 0x04, 0x02, 0x02, 0x02, 0x00,
        0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E, 0x00,
        0x0E, 0x11, 0x11, 0x1E, 0x10, 0x08, 0x06, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x02, 0x01,
        0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x00,
        0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x00,
        0x01, 0x02, 0x04, 0x08, 0x04, 0x02, 0x01, 0x00,
        0x0E, 0x11, 0x10, 0x08, 0x04, 0x00, 0x04, 0x00,
        0x0E, 0x11, 0x1D, 0x15, 0x0D, 0x01, 0x1E, 0x00,
        0x04, 0x0A, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x00,
        0x0F, 0x11, 0x11, 0x0F, 0x11, 0x11, 0x0F, 0x00,
        0x0E, 0x11, 0x01, 0x01, 0x01, 0x11, 0x0E, 0x00,
        0x07, 0x09, 0x11, 0x11, 0x11, 0x09, 0x07, 0x00,
        0x1F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x1F, 0x00,
        0x1F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x01, 0x00,
        0x0E, 0x11, 0x01, 0x0D, 0x11, 0x19, 0x16, 0x00,
        0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11, 0x00,
        0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x07, 0x00,
        0x1C, 0x08, 0x08, 0x08, 0x08, 0x09, 0x06, 0x00,
        0x11, 0x09, 0x05, 0x03, 0x05, 0x09, 0x11, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x0F, 0x00,
        0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11, 0x00,
        0x11, 0x13, 0x13, 0x15, 0x19, 0x19, 0x11, 0x00,
        0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00,
        0x0F, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x01, 0x00,
        0x0E, 0x11, 0x11, 0x11, 0x15, 0x09, 0x16, 0x00,
        0x0F, 0x11, 0x11, 0x0F, 0x05, 0x09, 0x11, 0x00,
        0x0E, 0x11, 0x01, 0x0E, 0x10, 0x11, 0x0E, 0x00,
        0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00,
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00,
        0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04, 0x00,
        0x41, 0x41, 0x41, 0x49, 0x2A, 0x2A, 0x14, 0x00,
        0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11, 0x00,
        0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x00,
        0x1F, 0x10, 0x08, 0x04, 0x02, 0x01, 0x1F, 0x00,
        0x07, 0x01, 0x01, 0x01, 0x01, 0x01, 0x07, 0x00,
        0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10, 0x00,
        0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07, 0x00,
        0x00, 0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00,
        0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x06, 0x08, 0x0E, 0x09, 0x0E, 0x00,
        0x01, 0x01, 0x0D, 0x13, 0x11, 0x13, 0x0D, 0x00,
        0x00, 0x00, 0x06, 0x09, 0x01, 0x09, 0x06, 0x00,
        0x10, 0x10, 0x16, 0x19, 0x11, 0x19, 0x16, 0x00,
        0x00, 0x00, 0x06, 0x09, 0x07, 0x01, 0x0E, 0x00,
        0x04, 0x0A, 0x02, 0x07, 0x02, 0x02, 0x02, 0x00,
        0x00, 0x00, 0x06, 0x09, 0x09, 0x06, 0x08, 0x07,
        0x01, 0x01, 0x0D, 0x13, 0x11, 0x11, 0x11, 0x00,
        0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00,
        0x04, 0x00, 0x06, 0x04, 0x04, 0x04, 0x04, 0x03,
        0x01, 0x01, 0x09, 0x05, 0x03, 0x05, 0x09, 0x00,
        0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,
        0x00, 0x00, 0x15, 0x2B, 0x29, 0x29, 0x29, 0x00,
        0x00, 0x00, 0x0D, 0x13, 0x11, 0x11, 0x11, 0x00,
        0x00, 0x00, 0x06, 0x09, 0x09, 0x09, 0x06, 0x00,
        0x00, 0x00, 0x0D, 0x13, 0x13, 0x0D, 0x01, 0x01,
        0x00, 0x00, 0x16, 0x19, 0x19, 0x16, 0x10, 0x10,
        0x00, 0x00, 0x0D, 0x13, 0x01, 0x01, 0x01, 0x00,
        0x00, 0x00, 0x0E, 0x01, 0x06, 0x08, 0x07, 0x00,
        0x00, 0x02, 0x07, 0x02, 0x02, 0x02, 0x04, 0x00,
        0x00, 0x00, 0x11, 0x11, 0x11, 0x19, 0x16, 0x00,
        0x00, 0x00, 0x11, 0x11, 0x11, 0x0A, 0x04, 0x00,
        0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0A, 0x00,
        0x00, 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x00,
        0x00, 0x00, 0x09, 0x09, 0x09, 0x0E, 0x08, 0x06,
        0x00, 0x00, 0x0F, 0x08, 0x06, 0x01, 0x0F, 0x00,
        0x04, 0x02, 0x02, 0x01, 0x02, 0x02, 0x04, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,
        0x01, 0x02, 0x02, 0x04, 0x02, 0x02, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1C, 0x2A, 0x49, 0x49, 0x41, 0x22, 0x1C, 0x00,
        0x1C, 0x22, 0x51, 0x49, 0x41, 0x22, 0x1C, 0x00,
        0x1C, 0x22, 0x41, 0x79, 0x41, 0x22, 0x1C, 0x00,
        0x1C, 0x22, 0x41, 0x49, 0x51, 0x22, 0x1C, 0x00,
        0x1C, 0x22, 0x41, 0x49, 0x49, 0x2A, 0x1C, 0x00,
        0x1C, 0x22, 0x41, 0x49, 0x45, 0x22, 0x1C, 0x00,
        0x1C, 0x22, 0x41, 0x4F, 0x41, 0x22, 0x1C, 0x00,
        0x1C, 0x22, 0x45, 0x49, 0x41, 0x22, 0x1C, 0x00,
};


byte lentbl_S[104] =
{
        2, 2, 3, 5, 5, 5, 5, 2, 
        3, 3, 5, 5, 2, 5, 1, 5, 
        5, 4, 5, 5, 5, 5, 5, 5, 
        5, 5, 1, 2, 4, 4, 4, 5, 
        5, 5, 5, 5, 5, 5, 5, 5, 
        5, 3, 5, 5, 4, 5, 5, 5, 
        5, 5, 5, 5, 5, 5, 5, 7, 
        5, 5, 5, 3, 5, 3, 5, 5, 
        2, 4, 5, 4, 5, 4, 4, 4, 
        5, 2, 3, 4, 2, 6, 5, 4, 
        5, 5, 5, 4, 3, 5, 5, 5, 
        5, 4, 4, 3, 2, 3, 0, 0, 
        7, 7, 7, 7, 7, 7, 7, 7
};


// pin 6 is connected to the MAX7219 pin 1
// pin 5 is connected to the CLK pin 13
// pin 3 is connected to LOAD pin 12

LedControl lc=LedControl(6,5,3,devCount); 

char msg[140] = "No Msg...";
int msgsize =  strlen(msg);
char inString[144]="";
int inCount=0;

int binLED[192];
// The character set courtesy of cosmicvoid.  It is rowwise


int curcharix = 0;
int curcharbit = 0;
int curcharixsave = 0;
int curcharbitsave = 0;
int curcharixsave2 = 0;
int curcharbitsave2 = 0;
char curchar;

 int i,j,k;

void setup()
{
  SerialUSB.begin(9600);
  tweetMsg.setPrinter (SerialUSB);

  //we have already set the number of devices when we created the LedControl
  //int devices=lc.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devCount;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,brightness);
    /* and clear the display */
    lc.clearDisplay(address);
  }

  Ethernet.begin(mac);
  SerialUSB.print("Server IP is ");
  SerialUSB.println(Ethernet.localIP());
  // connect to Twitter:
  delay(3000);
  
  SerialUSB.println(hashTag);  
}

void loop()
{
  
  if (SerialUSB.available() > 0){ if (getMessage()) parseCmd();}
  

  
  if (scrollFlag){
    curcharixsave2 = curcharix;
    curcharbitsave2 = curcharbit;
    

    for (i=devCount-1;i>=0;i--) // Loop through our 8 displays
    {
      for (j=0;j<8;j++) // Set up rows on current  display
      {      
        byte outputbyte = 0;
      
        curchar = msg[curcharix];
      
        curcharixsave = curcharix;
        curcharbitsave = curcharbit;
      
        for (k=7;k>=0;k--) // Copy over data for 8 columns to current row and send it to current display
        {
          // This byte is the bitmap of the current character for the current row
          byte currentcharbits = Font8x5[((curchar-32)*8)+j];
      
          if (currentcharbits & (1<<curcharbit))
            outputbyte |= (1<<k);
      
          // advance the current character bit of current character
      
          curcharbit ++;
      
          if (curcharbit > lentbl_S[curchar-32]) // we are past the end of this character, so advance.
          {
            curcharbit = 0;
            curcharix += 1;          
            if (curcharix+1 > msgsize) curcharix=0;
            curchar = msg[curcharix];
          }
        }
      
        lc.setRow(i, j, outputbyte);

        if (j != 7) // if this is not the last row, roll back advancement, if it is, leave the counters advanced.
        {
          curcharix = curcharixsave;
          curcharbit = curcharbitsave;
        }
      }
    }
  
    curcharix = curcharixsave2;
    curcharbit = curcharbitsave2;
    curchar = msg[curcharix];
 
  
    // advance the current character bit of current character
      
    curcharbit ++;
      
    if (curcharbit > lentbl_S[curchar-32]) // we are past the end of this character, so advance.
    {
      curcharbit = 0;
      curcharix += 1;
      if (curcharix+1 > msgsize) curcharix=0;
      curchar = msg[curcharix];
    }
    
    delay(scrollSpeed);
  }
  
   getTwitter();
}





boolean getMessage()
{
  char in_char = '\0';    //Create a character to store the incoming byte from the serial port.
  inCount = 0;
 
  if (SerialUSB.available() > 0) {
   in_char=SerialUSB.read();
   if (in_char=='#'){
    while(in_char != '!'){
      while(SerialUSB.available() <=0);
       in_char = SerialUSB.read();   
       inString[inCount]=in_char; 
      inCount++;
      //SerialUSB.println(in_char);
    }
    
    inString[inCount+1]='\0';    //Strings must end with a Null terminator so we need to add this to our message.

    return true;
   }
  }

 return false;
}


void parseCmd()
{

     char cmd[28]="";
    char cmdChar;
    int cmdValue=0;
    int code=0;
   
      if (inString[0] == '/'){
        
        for (int x = 0; x < inCount; x++)
        {
           cmd[x] = inString[x+1];
        
        }
       if (cmd[0] == 'B')
        {
          for (int x = 0; x < inCount-1; x++)
            {
               binLED[x] = cmd[x+1];
             }
          setDisplay();
        }
        else 
        {
        
        cmdChar = cmd[0];
        
        for (int x = 0; x < inCount-1; x++)
        {
           cmd[x] = cmd[x+1];
        }
        
     
        cmdValue = atoi(cmd);
        
        pCmd(cmdChar, cmdValue);
        
        }

      }
      
      else {

      for (int i = 0; i < inCount-1; i++){
        
       msg[i]=inString[i];

      }
                msg[inCount-1]='\0';
              

    msgsize =  strlen(msg);
    //SerialUSB.println(msgsize);
    
    if (msgsize < 7 )
    {
      
     scrollFlag = false;
     staticDisplay();
      
    }
    else 
    {
     scrollFlag = true; 
    }
      }
     
     
     
} 
  





void pCmd(char cmd, int value)
{
  switch (cmd)
  {
   
    case 's':
    
    if (scrollFlag)
    {
       scrollFlag = false;
    }
    else 
    {
      scrollFlag = true;
    }
    
    break;

   case 'f':
        scrollSpeed = value;
    break;
    
    case 'b':
       brightness = value;
       for(int address=0;address<devCount;address++)
         {
            lc.setIntensity(address,brightness);
         }
     break;
     
     case 'o':
       if (ledShutdown)
        {     
          for(int address=0;address<devCount;address++)
          {
            lc.shutdown(address,false);
            ledShutdown = false;
            SerialUSB.println("On");
          }
        }
        else
         {
            for(int address=0;address<devCount;address++)
            {
              lc.shutdown(address,true);
              ledShutdown = true;
              SerialUSB.println("Off");
            } 
        
         }
      break;
  }
  
}


void setDisplay()
{
  
       if (inCount < 27) {
         SerialUSB.println ("Error Binary Command too short, should be 24 bits."); 
       }
       else {
        int currPos = 0;
      
        if (scrollFlag)
          {
            scrollFlag = false;
            clearDisplay();
      }
      
      for (int d=devCount-1;d>=0;d--) // Loop through displays
        {
          for (int r=0;r<8;r++) // Set up rows on current display
            {  
             
              lc.setRow(d,r,binLED[currPos]);
              currPos++;
            }
            
            
        } 
      
     }

     
}


void staticDisplay()
{
  
curcharix = 0;
curcharbit = 0;
curcharixsave = 0;
curcharbitsave = 0;
curcharixsave2 = 0;
curcharbitsave2 = 0;

clearDisplay();
  
bool endFlag = false;
  
    for (i=devCount-1;i>=0;i--) // Loop through our X displays
    {
      for (j=0;j<8;j++) // Set up rows on current  display
      {      
        byte outputbyte = 0;
   
        curchar = msg[curcharix];
   
        curcharixsave = curcharix;
        curcharbitsave = curcharbit;
      
        for (k=7;k>=0;k--) // Copy over data for 8 columns to current row and send it to current display
        {
          // This byte is the bitmap of the current character for the current row
          byte currentcharbits = Font8x5[((curchar-32)*8)+j];
      
          if (currentcharbits & (1<<curcharbit))
            outputbyte |= (1<<k);
      
          // advance the current character bit of current character
      
          curcharbit ++;
      
          if (curcharbit > lentbl_S[curchar-32]) // we are past the end of this character, so advance.
          {
            curcharbit = 0;
            curcharix += 1; 
            if (curcharix+1 > msgsize) k=-1;
            curchar = msg[curcharix];
            
          }
        }
      
        lc.setRow(i, j, outputbyte);
        
       if (endFlag) return;

        if (j != 7) // if this is not the last row, roll back advancement, if it is, leave the counters advanced.
        {
          curcharix = curcharixsave;
          curcharbit = curcharbitsave;
          if (curcharix+1 > msgsize) endFlag=true;
        }

      }
    }

  

  
}

void clearDisplay()
{
  
 for(int address=0;address<devCount;address++)
          {
         lc.clearDisplay(address);
       } 
  
}


void getTwitter(){
  
  if ((millis() > (delayTime + delayTwitter)) || (firstBoot)){
    
   bool firstLoop = true;
   unsigned long firstTime = 0;
   unsigned long currTime = 0;
   
  
   SerialUSB.println("connecting to server...");
   
   if (client.connect(serverName, 80)) {
    TextFinder  finder( client,5 );
    SerialUSB.println("making HTTP request...");
    // make HTTP GET request to twitter:
    client.print("GET /search.atom?q=%23");
    client.print(TwitterHashtag);
    client.println("&count=1 HTTP/1.1");
    client.println("HOST: search.twitter.com");
    client.println();
    SerialUSB.println("sended HTTP request...");
    while (client.connected()) {
      if (client.available()) {
        SerialUSB.println("looking for tweet...");
        while (finder.find("<published>")){
          
          if (firstLoop) {
           
            finder.find("T");
            firstTime = finder.getValue(':');
            //SerialUSB.println(firstTime);
            firstLoop = false;
            
          }
          else {
            
            finder.find("T");
            currTime = finder.getValue(':');
            //SerialUSB.println(currTime);   
            
            if (currTime < firstTime - tweetHistoryTime) break;  
            
          
          }
                   
          
          if((finder.getString("<title>","</title>",tweet,140)!=0)){
            String tweetClean(tweet);
            tweetClean.replace(hashTag, " * "); //Remove search hashtag before being displayed
            SerialUSB.println(tweetClean);
            tweetMsg.push(tweetClean); //Need to figure out once mem is full will it every free up
          }
        }
        break;
      }
    }
    delay(1);
    client.stop();
    firstLoop = true;
  }
  
  //SerialUSB.println("delay...");

   displayCycleTime = delayTwitter / tweetMsg.count(); //Calculation to spread display of tweets evenly over the time between connections
   if (displayCycleTime < minDisplayTime) displayCycleTime = minDisplayTime; //If there are too many tweets for messages to be display set display time to min display time.
   //SerialUSB.println(displayCycleTime);  
 
   delayTime = millis();
   firstBoot = false;

 }
 
 
  if (millis() > (displayLastTime + displayCycleTime)){

    if (!tweetMsg.isEmpty()) {
      
      String currTweet = tweetMsg.pop();
    
      for (int i = 0; i < currTweet.length(); i++){
          msg[i]=currTweet[i];
      }
   
      msg[currTweet.length()]='\0';
   
      msgsize =  currTweet.length();
      
    }
    
    displayLastTime = millis();
    
    
 }
  
}
  
