// https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/udp-examples.rst
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid     = "EARS_LANDER_2.4";
const char* password = "lunar-rover";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incomingPacket[255];
char replyPacekt[] = "R";

#define LED LED_BUILTIN

int timeSinceKeepAlive = -1;

void setup() {
	Serial.begin(115200);
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);

	pinMode(0, OUTPUT); // Motor A direction
	pinMode(2, OUTPUT); // Motor B direction
	pinMode(4, OUTPUT); // Motor B speed
	pinMode(5, OUTPUT); // Motor A speed

	delay(10);

	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	Udp.begin(localUdpPort);
	Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

typedef struct _GUID {
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];
} GUID_t;

void loop() {

// 	GUID_t FormatID0;
// 	GUID_t FormatID;
// 	FormatID0.Data1 = 0xCEAC9A68;
// 	FormatID0.Data2 = 0xE109;
// 	FormatID0.Data3 = 0x47B9;
// 	FormatID0.Data4[0] = 0xA1;
// 	FormatID0.Data4[1] = 0x34;
// 	FormatID0.Data4[2] = 0xF6;
// 	FormatID0.Data4[3] = 0xB7;
// 	FormatID0.Data4[4] = 0x8D;
// 	FormatID0.Data4[5] = 0xF8;
// 	FormatID0.Data4[6] = 0x26;
// 	FormatID0.Data4[7] = 0x31;

	int packetSize = Udp.parsePacket();
	if (packetSize) {
		// receive incoming UDP packets
		Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
		int len = Udp.read(incomingPacket, 255);
		if (len > 0) {
			incomingPacket[len] = 0;
		}
		Serial.printf("UDP packet contents: %s\n", incomingPacket);
		byte Command = (byte) incomingPacket;

		digitalWrite(LED, LOW);
		delay(1);
		digitalWrite(LED, HIGH);

		digitalWrite(0, Command & 1 ? HIGH : LOW);
		digitalWrite(2, Command & 2 ? HIGH : LOW);
		digitalWrite(4, Command & 4 ? HIGH : LOW);
		digitalWrite(5, Command & 8 ? HIGH : LOW);

		// send back a reply, to the IP address and port we got the packet from
		// Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
		// Udp.write(replyPacekt);
		// Udp.endPacket();
	}
} // End loop
