# ArduinoNetworkTester
Arduino Network Tester, extends the functionality of  the well-known Network Cable tester by adding DHCP, DNS and Internet connectivity tests using an Arduino Pro Mini and a W5500 Ethernet Shield.
<p align="center">
  <img src="/images/render.jpg">
</p>
## How it works

When turned on, the Power LED is ON and the Status LED is blinking, until you connect an RJ45 (Ethernet Cable) from an <a href="https://en.wikipedia.org/wiki/OSI_model">OSI Layer 3</a> device (e.g. router) to the W5500 ethernet port. After that, the device runs the tests one by one starting with DHCP where it requests an IP address. If it successfully receive one then the DHCP LED lights. Next it tries to resolve the url www.example.com to the appropriate IP, if it is successful then the DNS LED lights up. Finally it makes an HTTP request to the domain www.example.com and parse the result. If the returned <a href="https://httpstatuses.com/">HTTP Status Code</a> equals 200 then the Internet Connectivity LED turns on.

## Bill of Materials
1) Double Side Thru-Hole Prototype PCB Board (at least 70*90mm)
2) Arduino Pro Mini 3.3v 8MHz
3) W5500 Ethernet Shield
4) 5 LEDs (SMD 0805)
5) 5 330 Ohm Resistors (SMD 0805)
6) Battery Charger (TC4056A)
7) 3.7V 100mah LIPO battery with XH 2.5mm connector
8) B2B-XH-A 2 Pins connector
9) On-Off switch (SS-12F15G)
10) 2.54 mm Pin headers

## Prototype PCB Board Design
Arduino Network Tester consists of two double side thru-hole prototype PCB boards, the top with the LED Indicators and the bottom, with the rest circuit.
<p align="center">
    <img src="/images/prototype.jpg">
</p>
<a href="https://novamostra.com/2020/07/30/arduino-network-tester/">Read more at novamostra.com</a>
