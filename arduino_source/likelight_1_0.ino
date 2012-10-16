
/* Updated LikeLight code for arduino 1.0.
 *
 * uses the 1.0 updates to the ethernet libs to connect via DHCP, plus other small method changes/updates.
 *
 * Orginal code by  redpepper, updated by tom wilderspin
 */

//some includes
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0x35, 0x1C }; // if your shield comes with a mac address on a sticker add it here.
byte ip[] = { 192,168,0,80 }; //only required if first connection attempt fails

//byte gateway[] = { 192,168,0,1 }; // router
//byte subnet[] = { 255, 255, 255, 0 }; //subnet mask of the network 
//byte server[] = { 98,129,229,36 }; // Google 74.125.67.99 RPlab 98.129.229.36 -- removed & changed for server name below

// Define the LED Pins
const int ledPin =  13; // diagnostics pin
const int ledPinLight =  5; // light led 1
const int ledPinLightTwo =  6; // light led 2
const int ledPinLightThree =  7; // light led 3
const int ledPinLightFour =  4; // light led 4

// Make variables? Yes please!
String serverData = String();


char serverName[] = "YOUR_HOST_HERE"; //replace this with the host of your php script


int ledState = LOW;  // init leds to low
int isTransferring = 0;
long likeDifference = 0;
long likeCount = 0;
long likeNumber = 0;
long lastLikeCount = 0;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// SETUP
// THIS IS ONE OF THE TWO MAIN FUNCTION
void setup() {
  
  Serial.println("start code run");
  
  // Set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPinLight, OUTPUT);
  pinMode(ledPinLightTwo, OUTPUT);
  pinMode(ledPinLightThree, OUTPUT);
  pinMode(ledPinLightFour, OUTPUT);
  
  // Start the serial library:
  Serial.begin(9600);
    
  // Start the Ethernet connection:
  if(Ethernet.begin(mac)==0){
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac,ip);
  }else{
    Serial.println("connected!");
  }
  
    
  // Give the Ethernet shield a second to initialize:
  delay(1000);
  
  Serial.println("working...");
  
}

// LOOP
void loop() {
  
  // If we get a connection, report back via serial:
  if(isTransferring == 0){
    if (client.connect(serverName, 80)) {
  
      // Make a HTTP request:
      client.println("GET /YOUR_PATH_HERE/ HTTP/1.1"); //replace with your php script path
      client.println("Host: YOUR_HOST_HERE"); //replaced with your php script host
      client.println();
      
      isTransferring = 1;
    } else {
      isTransferring = 0; 
    }
  }
  
  // Loop until the client is available
  while (client.available()) {
    
    // Collect client response data
    char c = client.read();
    
    // Append to our returned server data to parse later
    serverData += c;

  }
  
  // During transfer if the connection stops kill the connection
  // and process the response
  if(isTransferring == 1){
    
    // if the server's disconnected, stop the client:
    if (!client.connected()) {
	
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
      
      isTransferring = 0;
      
      // Process the collected response from the server
      // look for the ?= then process get what is after that, that's our like number
      
      //debug* raw response from the webserver useful if any connection issues 
      
      /*Serial.println("server return::");
       *Serial.println(serverData);
       */
       
      long likeLine = long(serverData.indexOf("?="));
      Serial.println("like line");
      Serial.println(likeLine);
      
      //substr out the count number
      String likeCount = String(serverData.substring(likeLine+2, likeLine+17));
      //remove white space
      likeCount.trim();
      
      Serial.println("like count");
      Serial.println(likeCount);
      //convert
      likeNumber = stringToLong(likeCount);
      
      Serial.println("likeNumber:");
      Serial.println(likeNumber);
            
      Serial.println("lastLikeCount:");
      Serial.println(lastLikeCount);
            
      // How many likes since last time?
      likeDifference = likeNumber - lastLikeCount;
      
      // Save the new like value
      lastLikeCount = likeNumber;
        
      Serial.println("likeDifference:");
      Serial.println(likeDifference);

	  // If there are more likes than last time we checked do some blinkin
      if(likeDifference > 0 && likeDifference != lastLikeCount){
        
        // Blink the differnece
        for(int i=0;i<likeDifference;i++){
            
            Serial.println("Blinking...");
            Serial.println(i);
          
            // Get your blink on!!!!
            ledState = HIGH;
            digitalWrite(ledPinLight, ledState); 
            digitalWrite(ledPinLightTwo, ledState); 
            digitalWrite(ledPinLightThree, ledState);
            digitalWrite(ledPinLightFour, ledState);
            delay(3000); // Leave a light on for me, I'll be... for 3 seconds
            
            ledState = LOW;
            digitalWrite(ledPinLight, ledState);
            digitalWrite(ledPinLightTwo, ledState);
            digitalWrite(ledPinLightThree, ledState);
            digitalWrite(ledPinLightFour, ledState);  
            delay(1000); // Did you note the Belinda Carlisle ref above?
        }
      }
            
      // Reset response data
      serverData = String();
      
      // Wait X more seconds before checking again. 5000 is 5 sec
      delay(5000);  
    }
  }
}

// Helper function to turn a string into a long int
long stringToLong(String s) {
    char arr[12];
    s.toCharArray(arr, sizeof(arr));
    return atol(arr);
}
