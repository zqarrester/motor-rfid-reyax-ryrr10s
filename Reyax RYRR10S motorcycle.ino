#include <SoftwareSerial.h>
SoftwareSerial NFCserial(4, 5);//Pin 4 and 5 to TX RX Reyax RYRR10S

int piezoPin = 11; //Pin Buzzer
int relayPin = 7; //Pin Relay
int status; //ON OFF Status

String received_id = "";
boolean received_complete;
char Byte_In_Hex[3];

uint8_t response_byte;
uint8_t received_buf_pos;
uint8_t received_data[256];
uint8_t data_len;

uint8_t echo_command[1] = {0x55};
uint8_t info_command[2] = {0x01, 0x00};
uint8_t initialise_cmd_iso14443_1[6] =  {0x09, 0x04, 0x68, 0x01, 0x07, 0x10};
uint8_t initialise_cmd_iso14443_2[6] =  {0x09, 0x04, 0x68, 0x01, 0x07, 0x00};
uint8_t initialise_cmd_iso14443_3[6] =  {0x02, 0x04, 0x02, 0x00, 0x02, 0x80};
uint8_t initialise_cmd_iso14443_4[6] =  {0x09, 0x04, 0x3A, 0x00, 0x58, 0x04};
uint8_t initialise_cmd_iso14443_5[6] =  {0x09, 0x04, 0x68, 0x01, 0x01, 0xD3};
uint8_t detect_cmd_iso14443_1[4] =  {0x04, 0x02, 0x26, 0x07};
uint8_t detect_cmd_iso14443_2[5] =  {0x04, 0x03, 0x93, 0x20, 0x08};

void setup() 
    {
  Serial.begin(9600);
  pinMode(piezoPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin,HIGH);
  status = 0;//set status to 0
  init_nfc();
    }

void loop() 
    {
  scan_tag();
  delay(1000);
    }

void init_nfc()
    { 
NFCserial.begin(57600);
Serial.println("initializing...");
  delay(1000);
 
Serial.println("Receiving device info...");
    
NFCserial.write(info_command, 2);
  delay(1000);
  show_serial_data();//request info about the CH95HF
NFCserial.write(echo_command, 1);  
  delay(1000); show_serial_data();//send an echo command
  NFCserial.write(initialise_cmd_iso14443_1, 6);
  delay(1000); show_serial_data(); 
  NFCserial.write(initialise_cmd_iso14443_2, 6);
  delay(1000); show_serial_data();
  NFCserial.write(initialise_cmd_iso14443_3, 6);
  delay(1000); show_serial_data();
  NFCserial.write(initialise_cmd_iso14443_4, 6);
  delay(1000); show_serial_data();
  NFCserial.write(initialise_cmd_iso14443_5, 6); 
  delay(1000); show_serial_data();
    }

void serial_receive() 
    { 
uint8_t received_char;
  while(NFCserial.available()!=0){
    received_char = char (NFCserial.read());
    
if(received_buf_pos==0)response_byte = received_char; 
  else if (received_buf_pos==1)data_len = received_char; 
  else if (received_buf_pos>=2 and received_buf_pos<6) {
sprintf (Byte_In_Hex,"%x", received_char);
        received_id += Byte_In_Hex; //adding to a string
      }
received_buf_pos++;
if(received_buf_pos >= data_len){
        received_complete = true;
      }
   }
}

void scan_tag() 
    { 
  received_buf_pos = 0;
  received_complete = false;
  received_id="";
  response_byte=0;
  Serial.println("search new card");
  NFCserial.write(detect_cmd_iso14443_1, 4);
  delay(800);
Serial.println("Response:"); show_serial_data();
  
 NFCserial.write(detect_cmd_iso14443_2, 5);
  delay(300);

if(NFCserial.available()) {
    serial_receive();
if (received_id !=0 )//detect and show Card ID in serial monitor
    { 
      
Serial.println("==================");
Serial.print("Card ID: ");
Serial.println(received_id); Serial.println("==================");
 if ((status==0)&&(received_id =="bf265663"))//replace bf265663 with your card ID for register card
    {
    tone(piezoPin, 1000, 1000);
    delay (500);
    digitalWrite (relayPin, LOW);
    Serial.println("ENGINE READY TO START");
    Serial.println("");
    delay (1000);
    status = 1;
    } 
else if 
    ((status==1)&&(received_id =="bf265663")) //replace bf265663 with your card ID
        
    {
    tone(piezoPin, 1000, 1000);
    delay (700);
    digitalWrite (relayPin, HIGH);
    Serial.println("ENGINE STOP");
            Serial.println("");
    status = 0;
    }
else if (received_id !="bf265663")////replace bf265663 with your card ID
    { Serial.println("TAG NOT REGISTERED");    
    tone(piezoPin, 1000, 200);
    delay(400);
    
tone(piezoPin, 1000, 200);
    delay(400);  
    tone(piezoPin, 1000, 200);
    delay (1000);
    }
  }
        else Serial.println("No card detected");
        delay (1000);
    }
  }
void show_serial_data()
    {
while (NFCserial.available() != 0)  //If data is available on serial port
Serial.print(NFCserial.read(), HEX);// Print character received on to the serial monitor in
    Serial.println("");
}
