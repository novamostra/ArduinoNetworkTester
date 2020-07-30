/*
  Arduino Network Tester

  Checks DHCP, DNS and Internet connectivity.

  Arduino Network Tester, extends the functionality of  the well-known
  Network Cable tester by adding DHCP, DNS and Internet connectivity tests,
  using an Arduino Pro Mini 3.3V 8Mhz with optiboot_atmega328 bootloader
  and a W5500 Ethernet Module. For DNS and Internet Availability the domain
  example.com is used.

  With simple code changes it could be used to check the configuration of
  isolated local networks, using local Web Server/Router page instead of
  example.com and/or Static DHCP configuration (uncomment the appropriate
  line) in the case of a local network without DHCP Server.

  Additionally the ability to set custom mac address could be used to check
  MAC filtering configuration of the network.
  
  Read more about the implementation at:
  https://novamostra.com/2020/07/30/arduino-network-tester/

  created 30 July 2020
  by Novamostra
*/
#include <SPI.h>
#include <Ethernet.h>
#include <Dns.h>
#include <avr/wdt.h>

#define PWR_LED 6
#define DHCP_LED 5
#define DNS_LED 4
#define INT_LED 3
#define STATUS_LED 2

//Here you can set your own mac address
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

DNSClient dns_client;
EthernetClient eth_client;

char server_url[] = "www.example.com";
// www.example.com resolves to 93.184.216.34
// if you change server_url you must also change server_ip
byte server_ip[] = { 93, 184, 216, 34 };

//the resolved ip from the DNS query
IPAddress server_ip_resolved;

boolean dns_resolved = false;
boolean http_responded = false;

long last_blink = 0;

void init_leds() {
  pinMode(PWR_LED, OUTPUT);
  pinMode(DHCP_LED, OUTPUT);
  pinMode(DNS_LED, OUTPUT);
  pinMode(INT_LED, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(PWR_LED, HIGH);
  digitalWrite(DHCP_LED, HIGH);
  digitalWrite(DNS_LED, HIGH);
  digitalWrite(INT_LED, HIGH);
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(DHCP_LED, LOW);
  digitalWrite(DNS_LED, LOW);
  digitalWrite(INT_LED, LOW);
  digitalWrite(STATUS_LED, LOW);
}

void reset_leds() {
  digitalWrite(DHCP_LED, LOW);
  digitalWrite(DNS_LED, LOW);
  digitalWrite(INT_LED, LOW);
  digitalWrite(STATUS_LED, LOW);
}

void flash_led(int led, int blink_delay) {
  if (millis() - blink_delay > last_blink) {
    if (digitalRead(led) == HIGH) {
      digitalWrite(led, LOW);
    } else {
      digitalWrite(led, HIGH);
    }
    last_blink = millis();
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  init_leds();

  //while no cable is connected wait
  while (Ethernet.linkStatus() != LinkON) {
    flash_led(STATUS_LED, 1000);
  }
  //A cable has been connected, stop flashing Status LED
  digitalWrite(STATUS_LED, LOW);
  // Obtain an IP using DHCP
  if (Ethernet.begin(mac) == 0) {
  // Static Configuration: comment the previous line, 
  // uncomment the following line and set appropriate values:
  // if (Ethernet.begin(mac, ip, dns, gateway, subnet)==0) {
    while (true) {
      //Failed to configure Ethernet using DHCP      
      flash_led(DHCP_LED, 1000);
      digitalWrite(STATUS_LED, HIGH);
    }
  } else {
    //DHCP Configuration completed 
    digitalWrite(DHCP_LED, HIGH);
    // give the Ethernet shield a second to initialize:
    delay(1000);
    
    //Check DNS
    dns_client.begin(Ethernet.dnsServerIP());
    if (dns_client.getHostByName(server_url, server_ip_resolved) == 1 && server_ip_resolved == server_ip) {
      //DNS is working
      digitalWrite(DNS_LED, HIGH);
      dns_resolved = true;
    } else {
      //DNS failed
    }
 
    //Check Internet Connectivity
    if (eth_client.connect(server_url, 80)) {
      http_responded = false;
      eth_client.println("GET http://" + String(server_url) + " HTTP/1.1");
      eth_client.println("Host: " + String(server_url));
      eth_client.println("Connection: close");
      eth_client.println(); // end HTTP request header      
    } else {
      //Unable to establish an http connection
      digitalWrite(STATUS_LED, HIGH);
    }
  }
}

void reset_WDT() {
  wdt_enable(WDTO_15MS);
  while (1) {};
}

void loop() {
  //If cable unplugged reset
  if (Ethernet.linkStatus() != LinkON) {
    reset_WDT();
  }

  // Handle HTTP Client response
  String response = "";
  while (eth_client.available() && !http_responded) {
    char c = eth_client.read();
    response = response + c;
    //Find the first carriage return, this line returns the status code of the HTTP server
    if (c == '\r') {
      http_responded = true;
      eth_client.stop();
      //if page loaded then internet connectivity exists
      if (response.indexOf("200") > 0) {
        digitalWrite(INT_LED, HIGH);
        digitalWrite(STATUS_LED, HIGH);
      } else {
        //Failed to load the page, Internet connectivity does not exist
        digitalWrite(STATUS_LED, HIGH);
      }
      break;
    }
  }
}
