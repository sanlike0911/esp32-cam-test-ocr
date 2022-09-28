/*
  ESP32-CAM Text recognition (Tesseract.js)
*/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp32-cam-html.h"

// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER  // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD

#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "*********";
const char* password = "*********";

String Feedback = "";
String Command = "", cmd = "", P1 = "", P2 = "", P3 = "", P4 = "", P5 = "", P6 = "", P7 = "", P8 = "", P9 = "";
byte ReceiveState = 0, cmdState = 1, strState = 1, questionstate = 0, equalstate = 0, semicolonstate = 0;

WiFiServer server(80);

void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Video configuration settings
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  ledcAttachPin(4, 4);
  ledcSetup(4, 5000, 8);

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  Serial.print("Connecting to ");
  Serial.println(ssid);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  server.begin();
}

void loop() {

  Feedback="";Command="";cmd="";P1="";P2="";P3="";P4="";P5="";P6="";P7="";P8="";P9="";
  ReceiveState=0,cmdState=1,strState=1,questionstate=0,equalstate=0,semicolonstate=0;

  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        getCommand(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {

            if (cmd == "getstill") {
              camera_fb_t* fb = NULL;
              fb = esp_camera_fb_get();
              if (!fb) {
                Serial.println("Camera capture failed");
                delay(1000);
                ESP.restart();
              }

              client.println("HTTP/1.1 200 OK");
              client.println("Access-Control-Allow-Origin: *");
              client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
              client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
              client.println("Content-Type: image/jpeg");
              client.println("Content-Disposition: form-data; name=\"imageFile\"; filename=\"picture.jpg\"");
              client.println("Content-Length: " + String(fb->len));
              client.println("Connection: close");
              client.println();

              uint8_t* fbBuf = fb->buf;
              size_t fbLen = fb->len;
              for (size_t n = 0; n < fbLen; n = n + 1024) {
                if (n + 1024 < fbLen) {
                  client.write(fbBuf, 1024);
                  fbBuf += 1024;
                } else if (fbLen % 1024 > 0) {
                  size_t remainder = fbLen % 1024;
                  client.write(fbBuf, remainder);
                }
              }

              esp_camera_fb_return(fb);

              pinMode(4, OUTPUT);
              digitalWrite(4, LOW);
            } else {
              client.println("HTTP/1.1 200 OK");
              client.println("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
              client.println("Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS");
              client.println("Content-Type: text/html; charset=utf-8");
              client.println("Access-Control-Allow-Origin: *");
              client.println("Connection: close");
              client.println();

              String Data = "";
              if (cmd != "")
                Data = Feedback;
              else {
                Data = String((const char*)INDEX_HTML);
              }
              int Index;
              for (Index = 0; Index < Data.length(); Index = Index + 1000) {
                client.print(Data.substring(Index, Index + 1000));
              }

              client.println();
            }

            Feedback = "";
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if ((currentLine.indexOf("/?") != -1) && (currentLine.indexOf(" HTTP") != -1)) {
          if (Command.indexOf("stop") != -1) {
            client.println();
            client.println();
            client.stop();
          }
          currentLine = "";
          Feedback = "";
          ExecuteCommand();
        }
      }
    }
    delay(1);
    client.stop();
  }
}

void ExecuteCommand()
{
  //Serial.println("");
  //Serial.println("Command: "+Command);
  if (cmd!="getstill") {
    Serial.println("cmd= "+cmd+" ,P1= "+P1+" ,P2= "+P2+" ,P3= "+P3+" ,P4= "+P4+" ,P5= "+P5+" ,P6= "+P6+" ,P7= "+P7+" ,P8= "+P8+" ,P9= "+P9);
    Serial.println("");
  }

  if (cmd=="your cmd") {
    // You can do anything.
    // Feedback="<font color=\"red\">Hello World</font>";
  }
  else if (cmd=="ip") {
    Feedback+="STA IP: "+WiFi.localIP().toString();
  }
  else if (cmd=="mac") {
    Feedback="STA MAC: "+WiFi.macAddress();
  }
  else if (cmd=="resetwifi") {
    WiFi.begin(P1.c_str(), P2.c_str());
    Serial.print("Connecting to ");
    Serial.println(P1);
    long int StartTime=millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        if ((StartTime+5000) < millis()) break;
    }
    Serial.println("");
    Serial.println("STAIP: "+WiFi.localIP().toString());
    Feedback="STAIP: "+WiFi.localIP().toString();
  }
  else if (cmd=="restart") {
    ESP.restart();
  }
  else if (cmd=="digitalwrite") {
    ledcDetachPin(P1.toInt());
    pinMode(P1.toInt(), OUTPUT);
    digitalWrite(P1.toInt(), P2.toInt());
  }
  else if (cmd=="analogwrite") {
    Serial.println(P2);
    if (P1="4") {
      ledcAttachPin(4, 4);
      ledcSetup(4, 5000, 8);
      ledcWrite(4,P2.toInt());
    }
    else {
      ledcAttachPin(P1.toInt(), 5);
      ledcSetup(5, 5000, 8);
      ledcWrite(5,P2.toInt());
    }
  }
  else if (cmd=="flash") {
    ledcAttachPin(4, 4);
    ledcSetup(4, 5000, 8);

    int val = P1.toInt();
    ledcWrite(4,val);
  }
  else if (cmd=="framesize") {
    sensor_t * s = esp_camera_sensor_get();
    if (P1=="QQVGA")
      s->set_framesize(s, FRAMESIZE_QQVGA);
    else if (P1=="HQVGA")
      s->set_framesize(s, FRAMESIZE_HQVGA);
    else if (P1=="QVGA")
      s->set_framesize(s, FRAMESIZE_QVGA);
    else if (P1=="CIF")
      s->set_framesize(s, FRAMESIZE_CIF);
    else if (P1=="VGA")
      s->set_framesize(s, FRAMESIZE_VGA);
    else if (P1=="SVGA")
      s->set_framesize(s, FRAMESIZE_SVGA);
    else if (P1=="XGA")
      s->set_framesize(s, FRAMESIZE_XGA);
    else if (P1=="SXGA")
      s->set_framesize(s, FRAMESIZE_SXGA);
    else if (P1=="UXGA")
      s->set_framesize(s, FRAMESIZE_UXGA);
    else
      s->set_framesize(s, FRAMESIZE_QVGA);
  }
  else if (cmd=="quality") {
    sensor_t * s = esp_camera_sensor_get();
    int val = P1.toInt();
    s->set_quality(s, val);
  }
  else if (cmd=="contrast") {
    sensor_t * s = esp_camera_sensor_get();
    int val = P1.toInt();
    s->set_contrast(s, val);
  }
  else if (cmd=="brightness") {
    sensor_t * s = esp_camera_sensor_get();
    int val = P1.toInt();
    s->set_brightness(s, val);
  }
  else {
    Feedback="Command is not defined.";
  }
  if (Feedback=="") Feedback=Command;
}

void getCommand(char c) {
  if (c == '?') ReceiveState = 1;
  if ((c == ' ') || (c == '\r') || (c == '\n')) ReceiveState = 0;

  if (ReceiveState == 1) {
    Command = Command + String(c);

    if (c == '=') cmdState = 0;
    if (c == ';') strState++;

    if ((cmdState == 1) && ((c != '?') || (questionstate == 1))) cmd = cmd + String(c);
    if ((cmdState == 0) && (strState == 1) && ((c != '=') || (equalstate == 1))) P1 = P1 + String(c);
    if ((cmdState == 0) && (strState == 2) && (c != ';')) P2 = P2 + String(c);
    if ((cmdState == 0) && (strState == 3) && (c != ';')) P3 = P3 + String(c);
    if ((cmdState == 0) && (strState == 4) && (c != ';')) P4 = P4 + String(c);
    if ((cmdState == 0) && (strState == 5) && (c != ';')) P5 = P5 + String(c);
    if ((cmdState == 0) && (strState == 6) && (c != ';')) P6 = P6 + String(c);
    if ((cmdState == 0) && (strState == 7) && (c != ';')) P7 = P7 + String(c);
    if ((cmdState == 0) && (strState == 8) && (c != ';')) P8 = P8 + String(c);
    if ((cmdState == 0) && (strState >= 9) && ((c != ';') || (semicolonstate == 1))) P9 = P9 + String(c);

    if (c == '?') questionstate = 1;
    if (c == '=') equalstate = 1;
    if ((strState >= 9) && (c == ';')) semicolonstate = 1;
  }
}