// ===== Settings ===== //
bool appendSpaces = true; // makes all SSIDs 32 characters long to improve performance

// ===== Preset Definitions ===== //
// You can edit the names, SSIDs, channels, and WPA2 settings for each preset here.
// SSIDs should be separated by \n. Max 32 characters per SSID.

struct Preset {
  const char* name;
  const char* ssids;
  uint8_t ch[3];
  uint8_t chCount;
  bool wpa2;
};

Preset presets[10] = {
  {"TEST", "Test01\nTest02\nTest03\nTest04\nTest05\nTest06\nTest07\nTest08\nTest09\nTest10\n", {1, 6, 11}, 3, false},
  {"RICKROLL", "1 Never Gonna Give You Up\n2 Never Gonna Let You Down\n3 Never Gonna Run Around\n4 And Desert You\n5 Never Gonna Make You Cry\n6 Never Gonna Say Goodbye\n7 Never Gonna Tell A Lie\n8 And Hurt You\n", {1, 6, 11}, 3, false},
  {"GOETIA", "Bael\nAgares\nVassago\nSamigina\nMarbas\nValefor\nAmon\nBarbatos\nPaimon\nBuer\nGuison\nSitri\nBeleth\nLeraje\nEligos\nZepar\nBotis\nBathin\nSallos\nPurson\nMarax\nIpos\nAim\nNaberius\nGlasya-Labolas\nBune\nRonove\nBerith\nAstaroth\nForneus\nForas\nAsmoday\nGaap\nFurfur\nMarchosias\nStolas\nPhenex\nHalphas\nMalphas\nRaum\nFocalor\nVepar\nSabnock\nShax\nVine\nBifrons\nVual\nHaagenti\nCrocell\nFurcas\nBalam\nAlloces\nCaim\nMurmur\nOrobas\nGremory\nOse\nAmy\nOrias\nVapula\nZagan\nValac\nAndras\nHaures\nAndrealphus\nCimeies\nAmdusias\nBelial\nDecarabia\nSeere\nDantalion\nAndromalius\n", {1, 6, 11}, 3, false},
  {"GOLF", "A Good Walk Spoiled\nGeriatrics Only Losers Favoured\nTadger Woods\nSmug Bastards Gowf Club\nGrass Chasers\nThe Old Man Shuffle\nGolf Harris\nPutt Putt\nRory McKilljoy\nDonald Gump Plays Gowf\nDonald Gump Is A Paedo\nChasing A Little White Ball\nTeatime Torture\nPro Lawn Ornament Hunters\nGowf Is Bowff\nGrassy Puttocks\nLawn Tiddlywinks\nTake Me In The Bunker\n18 Holes Of Nothing\nBunker Bummers\nPutt It In My Hole\nCheck Out My Action Slacks\nTee Off Then Piss Off\nGrab My Putt Stick\nA Tuft Of Ass\nDirthole Digglers\nOne Glove Love\nTickle My Dimpled White Balls\n18 Dirtholes Of Man Joy\nA Pastime For Pensioners\nA Hole In Bum\nTony Jackshit\nFore!Skin\nA Putt In The Butt\nGrip The Shaft Tightly\nLawn Shufflers\nWiggle My Puttocks\nUp The Bunker\nSpank Me With Your Golf Shoe\n19th Gloryhole\nCaddyshat\nDonald Gump Gowf Club\nGolf Is For Losers\n", {1, 6, 11}, 3, false},
  {"SODROLL", "1 Kill Yourself, Kill Yourself\n2 Why Don't You Kill Yourself?\n3 Don't Rely On No One Else\n4 End It All, Kill Yourself\n5 Kill Yourself, Kill Yourself\n6 Why Don't You Kill Yourself?\n7 Don't Rely On No One Else\n8 End It All, Kill Yourself Now!\n", {1, 6, 11}, 3, false},
  {"HICKSROLL", "01 Today A Young Man\n02 On Acid Realized\n03 That All Matter\n04 Is Merely Energy Condensed\n05 To A Slow Vibration\n06 That We Are All One\n07 Consciousness\n08 Experiencing Itself\n09 Subjectively\n10 There Is No Such Thing\n11 As Death\n12 Life Is Only A Dream\n13 And We Are The Imagination\n14 Of Ourselves\n15 Heres Tom With The Weather\n", {1, 6, 11}, 3, false},
  {"CUSTOM 1", "Custom 1\n", {1, 6, 11}, 3, false},
  {"CUSTOM 2", "Custom 2\n", {1, 6, 11}, 3, false},
  {"CUSTOM 3", "Custom 3\n", {1, 6, 11}, 3, false},
  {"CUSTOM 4", "Custom 4\n", {1, 6, 11}, 3, false}
};

int currentPresetIndex = -1; // -1 means no preset is active
char activeSsids[4096]; // Increased buffer size to allow more SSIDs per preset now that AP is disabled

// ==================== //
// ===== Includes ===== //
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <string.h>

// ==================== //
WebServer server(80);

// run-time variables
uint8_t channels[3] = {1, 6, 11}; // Active channels (updated by preset)
uint8_t channelCount = 3;
bool wpa2 = false;
char emptySSID[32];
uint8_t channelIndex = 0;
uint8_t macAddr[6];
uint8_t wifi_channel = 1;
uint32_t currentTime = 0;
uint32_t packetSize = 0;
uint32_t packetCounter = 0;
uint32_t attackTime = 0;
uint32_t packetRateTime = 0;

// State tracking for Web UI / AP shutdown
bool apActive = true;
bool apShutDown = false;

// beacon frame definition
uint8_t beaconPacket[109] = {
  /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: management beacon frame
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
  /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
  /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source (BSSID)
  // Fixed parameters
  /* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
  /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  /* 32 - 33 */ 0xe8, 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
  /* 34 - 35 */ 0x31, 0x00, // capabilities Information
  // Tagged parameters
  // SSID parameters
  /* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
  /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // SSID
  // Supported Rates
  /* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
  /* 72 */ 0x82, // 1(B)
  /* 73 */ 0x84, // 2(B)
  /* 74 */ 0x8b, // 5.5(B)
  /* 75 */ 0x96, // 11(B)
  /* 76 */ 0x24, // 18
  /* 77 */ 0x30, // 24
  /* 78 */ 0x48, // 36
  /* 79 */ 0x6c, // 54
  // Current Channel
  /* 80 - 81 */ 0x03, 0x01, // Channel set, length
  /* 82 */ 0x01, // Current Channel
  // RSN information
  /* 83 - 84 */ 0x30, 0x18,
  /* 85 - 86 */ 0x01, 0x00,
  /* 87 - 90 */ 0x00, 0x0f, 0xac, 0x02,
  /* 91 - 92 */ 0x02, 0x00,
  /* 93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04,
  /* 101 - 102 */ 0x01, 0x00,
  /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
  /* 107 - 108 */ 0x00, 0x00
};

// Function to apply a preset from the code definitions
void applyPreset(int index) {
  if (index < 0 || index >= 10) return;
  currentPresetIndex = index;
  
  // Copy SSIDs to active buffer
  strncpy(activeSsids, presets[currentPresetIndex].ssids, sizeof(activeSsids) - 1);
  activeSsids[sizeof(activeSsids) - 1] = '\0';
  
  // Update channels
  channelCount = presets[currentPresetIndex].chCount;
  if (channelCount == 0) channelCount = 1;
  for (int i = 0; i < channelCount && i < 3; i++) {
    channels[i] = presets[currentPresetIndex].ch[i];
  }
  
  // Fill the rest with the last valid channel to prevent 0
  for (int i = channelCount; i < 3; i++) {
    channels[i] = channels[channelCount - 1];
  }
  
  // Update wpa2
  wpa2 = presets[currentPresetIndex].wpa2;
  
  // Update packet size and capabilities based on wpa2
  if (wpa2) {
    beaconPacket[34] = 0x31;
    packetSize = sizeof(beaconPacket);
  } else {
    beaconPacket[34] = 0x21;
    packetSize = sizeof(beaconPacket) - 26;
  }
  
  // Reset channel hopping to start fresh on the new preset
  channelIndex = 0;
  wifi_channel = channels[0];
  esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
  
  Serial.print("Switched to: ");
  Serial.println(presets[currentPresetIndex].name);
  Serial.println("Active SSIDs:");
  Serial.println(activeSsids);
}

// Shift out channels one by one
void nextChannel() {
  if (sizeof(channels) > 1) {
    uint8_t ch = channels[channelIndex];
    channelIndex++;
    if (channelIndex >= sizeof(channels)) channelIndex = 0;
    if (ch != wifi_channel && ch >= 1 && ch <= 14) {
      wifi_channel = ch;
      esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
    }
  }
}

// Random MAC generator
void randomMac() {
  for (int i = 0; i < 6; i++) {
    macAddr[i] = random(256);
  }
  macAddr[0] = (macAddr[0] & 0xFE) | 0x02;
}

// ==========================================
// ===== RETRO TERMINAL WEB UI ===== //
// ==========================================
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP32 Beacon Spammer</title>";
  html += "<style>";
  html += "body { background-color: #000000; color: #00FF00; font-family: 'Courier New', Courier, monospace; text-align: center; margin: 0; padding: 20px; box-sizing: border-box; min-height: 100vh; }";
  html += "h1, h2, p, select, button { text-shadow: 0 0 5px #00FF00; }";
  html += "h1 { font-size: 24px; margin-bottom: 10px; letter-spacing: 2px; }";
  html += "h2 { font-size: 18px; margin-top: 30px; }";
  html += "select { background-color: #000000; color: #00FF00; border: 2px solid #00FF00; padding: 10px; font-size: 16px; font-family: 'Courier New', Courier, monospace; margin: 10px; cursor: pointer; outline: none; }";
  html += "select:focus { border-color: #33FF33; box-shadow: 0 0 10px #00FF00; }";
  html += "button { background-color: #000000; color: #00FF00; border: 2px solid #00FF00; padding: 10px 20px; font-size: 16px; font-family: 'Courier New', Courier, monospace; cursor: pointer; margin-top: 10px; transition: all 0.2s; }";
  html += "button:hover { background-color: #00FF00; color: #000000; text-shadow: none; box-shadow: 0 0 15px #00FF00; }";
  html += ".current { margin-top: 40px; font-size: 18px; border-top: 1px solid #00FF00; padding-top: 20px; display: inline-block; }";
  html += ".blink { animation: blinker 1s step-end infinite; }";
  html += "@keyframes blinker { 50% { opacity: 0; } }";
  html += "</style></head><body>";
  html += "<h1>ESP32 BEACON SPAMMER</h1>";
  html += "<h2>[ SELECT TARGET PRESET ]</h2>";
  html += "<form action='/apply' method='POST'>";
  html += "<select name='preset'>";
  for (int i = 0; i < 10; i++) {
    html += "<option value='" + String(i) + "'" + (i == currentPresetIndex ? " selected" : "") + ">" + String(presets[i].name) + "</option>";
  }
  html += "</select><br>";
  html += "<button type='submit'>[ EXECUTE ]</button>";
  html += "</form>";
  
  // Safely check if a preset is active before displaying it
  if (currentPresetIndex >= 0) {
    html += "<p class='current'>ACTIVE PRESET: <strong>" + String(presets[currentPresetIndex].name) + "</strong></p>";
  } else {
    html += "<p class='current'>ACTIVE PRESET: <strong>NONE</strong></p>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleApply() {
  if (server.hasArg("preset")) {
    int presetIndex = server.arg("preset").toInt();
    applyPreset(presetIndex);
  }
  
  // Send a final response before shutting down the network stack
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='5;url=/'></head>";
  html += "<body style='background-color:#000;color:#00FF00;font-family:\"Courier New\",Courier,monospace;text-align:center;padding:50px;'>";
  html += "<h1>[ PRESET APPLIED ]</h1>";
  html += "<p>SHUTTING DOWN WEB UI AND ACCESS POINT TO MAXIMISE BEACON SPAM PERFORMANCE</p>";
  html += "<p>DEVICE WILL CONTINUE SPAMMING IN THE BACKGROUND</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
  
  // Trigger AP shutdown on next loop iteration to ensure HTTP response is fully sent
  apActive = false;
}

void setup() {
  // create empty SSID
  for (int i = 0; i < 32; i++) {
    emptySSID[i] = ' ';
  }
  
  randomSeed(esp_random());
  randomMac();
  
  Serial.begin(115200);
  Serial.println();
  
  // Ensure no preset is active on boot
  currentPresetIndex = -1;
  activeSsids[0] = '\0'; // Clear the active SSID buffer

  // start WiFi in AP+STA mode to allow Web Server while keeping STA for beacon spam
  WiFi.mode(WIFI_MODE_APSTA);
  
  // Start Access Point for Web UI
  WiFi.softAP("•°|°•", "p5yb3rn4ut");
  Serial.println("Web UI AP started. Connect to '•°|°•' and visit:");
  Serial.println(WiFi.softAPIP());
  
  // Setup Web Server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/apply", HTTP_POST, handleApply);
  server.begin();
  Serial.println("Web server started");
  
  // Enable promiscuous mode to ensure raw frame transmission works reliably on ESP32
  esp_wifi_set_promiscuous(true);
  
  // Set to default WiFi channel
  esp_wifi_set_channel(channels[0], WIFI_SECOND_CHAN_NONE);
  
  Serial.println("Started o/");
  Serial.println();
}

void loop() {
  // Handle AP and Web Server shutdown if requested
  if (!apActive && !apShutDown) {
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_MODE_STA); // Switch to STA only to free up AP resources
    apShutDown = true;
    Serial.println("AP and Web UI shut down. Maximum resources allocated to beacon spamming.");
  }

  // Handle Web Server requests ONLY if AP is still active
  if (apActive) {
    server.handleClient();
  }

  currentTime = millis();

  // send out SSIDs ONLY if a preset is currently active
  if (currentPresetIndex >= 0 && currentTime - attackTime > 100) {
    attackTime = currentTime;

    int i = 0;
    int j = 0;
    int ssidNum = 1;
    char tmp;
    int ssidsLen = strlen(activeSsids);

    // Go to next channel
    nextChannel();

    while (i < ssidsLen) {
      // Get the next SSID
      j = 0;
      do {
        tmp = activeSsids[i + j];
        j++;
      } while (tmp != '\n' && j <= 32 && i + j < ssidsLen);

      uint8_t ssidLen = j - 1;

      // set MAC address
      macAddr[5] = ssidNum;
      ssidNum++;

      // write MAC address into beacon frame
      memcpy(&beaconPacket[10], macAddr, 6);
      memcpy(&beaconPacket[16], macAddr, 6);

      // reset SSID
      memcpy(&beaconPacket[38], emptySSID, 32);

      // write new SSID into beacon frame
      memcpy(&beaconPacket[38], &activeSsids[i], ssidLen);

      // set channel for beacon frame
      beaconPacket[82] = wifi_channel;

      // send packet
      if (appendSpaces) {
        for (int k = 0; k < 3; k++) {
          if (esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, packetSize, false) == ESP_OK) {
            packetCounter++;
          }
          delay(1);
        }
      } else {
        uint16_t tmpPacketSize = (packetSize - 32) + ssidLen; // calc size
        uint8_t tmpPacket[128]; // stack buffer is safe here since max size is ~109 bytes
        
        memcpy(&tmpPacket[0], &beaconPacket[0], 38 + ssidLen); // copy first half of packet into buffer
        tmpPacket[37] = ssidLen; // update SSID length byte
        memcpy(&tmpPacket[38 + ssidLen], &beaconPacket[70], wpa2 ? 39 : 13); // copy second half of packet into buffer

        // send packet
        for (int k = 0; k < 3; k++) {
          if (esp_wifi_80211_tx(WIFI_IF_STA, tmpPacket, tmpPacketSize, false) == ESP_OK) {
            packetCounter++;
          }
          delay(1);
        }
      }

      i += j;
    }
  }

  // show packet-rate each second
  if (currentTime - packetRateTime > 1000) {
    packetRateTime = currentTime;
    Serial.print("Packets/s: ");
    Serial.println(packetCounter);
    packetCounter = 0;
  }
}
