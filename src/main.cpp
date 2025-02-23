/*
 * Created on April 24 2022
 *
 * Copyright (c) 2022 - Daniel Hajnal
 * hajnal.daniel96@gmail.com
 * This file is part of the Commander-API project.
 * Modified 2022.04.24
 *
 * This is a simple example sketch that shows how
 * to use Commander-API library.
*/

// Necessary includes
#include <WiFi.h>
#include "Commander-API.hpp"
#include "Commander-IO.hpp"
#include "Shellminator.hpp"
#include "Shellminator-IO.hpp"


// This example assumes that there is a LED on GPIO-2,
// like on ESP32 NodeMCU boards.
#define LED_PIN 2

// WiFi credentials.
const char* ssid     = "yss";
const char* password = "yssyss11";

#define SERVER_PORT 11000

// We have to create an object from Commander class.
WiFiServer server( SERVER_PORT );
Shellminator shell( &server );
Commander commander;

// We have to create the prototypes functions for our commands.
// The arguments have to be the same for all command functions.
void sendToS1( char *args, Stream *response );
void sum_func( char *args, Stream *response );
Commander::API_t API_tree[] = {
    apiElement( "s1", "Send command to Serial1.", sendToS1 ),
    apiElement( "sum", "Sum two numbers.", sum_func ),
    
};

// Create an instance of the server.
// It will be available on port 10001.


void sendToS1( char *args, Stream *response )
{
  Serial.println("sendToS1 called with args: ");
  Serial.println(args);
  Serial1.println(args);
  //commander.execute( args, &Serial1 );
 
}
void setup() {

  // Set the LED pin to output, and turn it off.
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, 0 );

  // In this example, we will use the Serial for communication,
  // so we have to initialize it.
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 36, 37);


  while( !Serial );
  shell.clear();
  Serial.println( "Shell start." );
  
  // Configuring WiFi for connection.
  Serial.println();
  Serial.print( "Connecting to: " );
  Serial.println( ssid );
  
  WiFi.mode( WIFI_STA );
  WiFi.begin( ssid, password );
  
  while( WiFi.status() != WL_CONNECTED ){
    delay( 500 );
    Serial.print( "." );
  }
  shell.beginServer();
  
  Serial.println();
  Serial.println( "Connected!" );  
  Serial.print( "Device IP: " );
  Serial.print( WiFi.localIP() );
  Serial.print( " at port: " );
  Serial.println( SERVER_PORT );

  
  //server.begin();
  commander.attachDebugChannel( &Serial );
  //commander.attachDebugChannel( &Serial1 );
  shell.begin("<>");
  
  commander.attachTree( API_tree );
  
  
  commander.init();

  shell.attachCommander( &commander );
  shell.begin("RS");
  // After we attached the API_tree, Commander has to initialize
  // itself for the fastest runtime possible. It creates a balanced
  // binary tree from the API_tree to boost the search speed.
  // This part uses some recursion, to make the code space small.
  // But recursion is a bit stack hungry, so please initialize
  // Commander at the beginning of your code to prevent stack-overlow.

  
  
}

// Continuous example.
// In the loop function, there is a simple example to
// read commands from Serial in runtime. You can use
// the Serial monitor to try it. Set the line ending
// to new-line and play with the commands.

// This is a buffer to hold the incoming command.
char commandFromSerial[ 20 ];

// This variable tracks the location of the next free
// space in the commandFromSerial buffer.
uint8_t commandIndex = 0;

void loop() {
    // Check if there is any data incoming.

  shell.update(); 
   // Give some time to the other tasks.
   delay( 20 );

}



/// This is an example function for the sum command
void sum_func(char *args, Stream *response )
{

  // These variables will hold the value of the
  // two numbers, that has to be summed.
  int a = 0;
  int b = 0;

  // This variable will hold the result of the
  // argument parser.
  int argResult;

  // This variable will hold the sum result.
  int sum = 0;

  argResult = sscanf( args, "%d %d", &a, &b );

  // We have to check that we parsed succesfully the two
  // numbers from the argument string.
  if( argResult != 2 ){

    // If we could not parse two numbers, we have an argument problem.
    // We print out the problem to the response channel.
    response -> print( "Argument error! Two numbers required, separated with a blank space.\r\n" );

    // Sadly we have to stop the command execution and return.
    return;

  }

  // Calculate the sum.
  sum = a + b;

  // Print out the result.
  response -> printf( "%d + %d = %d\r\n", a, b, sum );

}