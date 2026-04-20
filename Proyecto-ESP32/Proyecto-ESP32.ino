#include <WiFi.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// WiFi
const char* ssid = "ESP32_Control";
const char* password = "12345678";

// UART
HardwareSerial miUART(2);

// Matriz MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1

#define DATA_PIN 23
#define CLK_PIN 18
#define CS_PIN 5

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

WiFiServer server(80);

const uint64_t NIVELES[] = { 
  0x40404060490d0b09, // N1 
  0xe020e080e90d0b09, // N2 
  0xe080e080e90d0b09  // N3 
};


const uint64_t PAUSA_BITMAP = 0x0051517355770000;
const uint64_t RUN_BITMAP = 0x0075157315770000;


int nivel = 0;
bool pausado = false;

byte font[36][8] = {
  {0x3C,0x42,0x42,0x7E,0x42,0x42,0x42,0x00}, // A
  {0x7C,0x42,0x42,0x7C,0x42,0x42,0x7C,0x00}, // B
  {0x3C,0x42,0x40,0x40,0x40,0x42,0x3C,0x00}, // C
  {0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x00}, // D
  {0x7E,0x40,0x40,0x7C,0x40,0x40,0x7E,0x00}, // E
  {0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x00}, // F
  {0x3C,0x42,0x40,0x4E,0x42,0x42,0x3C,0x00}, // G
  {0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x00}, // H
  {0x3C,0x08,0x08,0x08,0x08,0x08,0x3C,0x00}, // I
  {0x1E,0x04,0x04,0x04,0x44,0x44,0x38,0x00}, // J
  {0x42,0x44,0x48,0x70,0x48,0x44,0x42,0x00}, // K
  {0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00}, // L
  {0x42,0x66,0x5A,0x5A,0x42,0x42,0x42,0x00}, // M
  {0x42,0x62,0x52,0x4A,0x46,0x42,0x42,0x00}, // N
  {0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // O
  {0x7C,0x42,0x42,0x7C,0x40,0x40,0x40,0x00}, // P
  {0x3C,0x42,0x42,0x42,0x4A,0x44,0x3A,0x00}, // Q
  {0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x00}, // R
  {0x3C,0x42,0x40,0x3C,0x02,0x42,0x3C,0x00}, // S
  {0x7F,0x08,0x08,0x08,0x08,0x08,0x08,0x00}, // T
  {0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // U
  {0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00}, // V
  {0x42,0x42,0x42,0x5A,0x5A,0x66,0x42,0x00}, // W
  {0x42,0x24,0x18,0x18,0x18,0x24,0x42,0x00}, // X
  {0x42,0x24,0x18,0x08,0x08,0x08,0x08,0x00}, // Y
  {0x7E,0x04,0x08,0x10,0x20,0x40,0x7E,0x00}, // Z
  {0x3C,0x46,0x4A,0x52,0x62,0x46,0x3C,0x00}, // 0
  {0x08,0x18,0x08,0x08,0x08,0x08,0x3E,0x00}, // 1
  {0x3C,0x42,0x02,0x0C,0x30,0x40,0x7E,0x00}, // 2
  {0x3C,0x42,0x02,0x1C,0x02,0x42,0x3C,0x00}, // 3
  {0x0C,0x14,0x24,0x44,0x7E,0x04,0x04,0x00}, // 4
  {0x7E,0x40,0x7C,0x02,0x02,0x42,0x3C,0x00}, // 5
  {0x3C,0x40,0x7C,0x42,0x42,0x42,0x3C,0x00}, // 6
  {0x7E,0x02,0x04,0x08,0x10,0x10,0x10,0x00}, // 7
  {0x3C,0x42,0x42,0x3C,0x42,0x42,0x3C,0x00}, // 8
  {0x3C,0x42,0x42,0x3E,0x02,0x04,0x38,0x00}  // 9
};

byte corazon[8] = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};
byte magico[8] = {
  0b11010010,
  0b10010010,
  0b11011010,
  0b10000000,
  0b11001010,
  0b00011111,
  0b00001110,
  0b00000100
};

void mostrarBitmap(uint64_t bitmap) {
  mx.clear();

  for (int row = 0; row < 8; row++) {
    byte fila = (bitmap >> (row * 8)) & 0xFF;

    for (int col = 0; col < 8; col++) {
      bool pixel = bitRead(fila, col);
      mx.setPoint(row, 7 - col, pixel);
    }
  }
}
void mostrarCaracter(char c) {
  int index = -1;
  if (c == ',') {
    mx.clear();
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        bool pixel = bitRead(corazon[row], col);
        mx.setPoint(row, col, pixel);
      }
    }
    return;
  }

    if (c == '.') {
    mx.clear();
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        bool pixel = bitRead(magico[row], col);
        mx.setPoint(row, col, pixel);
      }
    }
    return;
  }


  if (c >= 'A' && c <= 'Z') index = c - 'A';
  else if (c >= '0' && c <= '9') index = (c - '0') + 26;

  if (index != -1) {
    mx.clear();
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        bool pixel = bitRead(font[index][row], col);
        mx.setPoint(row, col, pixel);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  miUART.begin(115200, SERIAL_8N1, 16, 17);

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 8);
  mx.clear();

  WiFi.softAP(ssid, password);
  server.begin();
  mostrarBitmap(NIVELES[nivel]);
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\n');
    client.flush();

    if (request.indexOf("/SUBIR") != -1){ miUART.write('A'); Serial.println("Mandando A");}
    if (request.indexOf("/BAJAR") != -1) {miUART.write('B'); Serial.println("Mandando B");}
    if (request.indexOf("/PAUSA") != -1){ miUART.write('C'); Serial.println("Mandando C");}
    if (request.indexOf("/ENVIAR?char=") != -1) {
    int inicio = request.indexOf("char=") + 5;
  
  if (inicio < request.length()) {
    char c = request.charAt(inicio);

    if (isalnum(c) || c == ',' || c == '.'){
      if (isalpha(c)) {
        c = toupper(c);
      }

      Serial.print("Caracter desde web: ");
      Serial.println(c);

      mostrarCaracter(c);
        }
      }   
    }
    
    if (request.indexOf("/CORAZON") != -1) {
      Serial.println("Mostrando corazon");
      mostrarCaracter(',');  // 👈 reutilizas tu lógica
      }
    
    if (request.indexOf("/MAGICO") != -1) {
      Serial.println("Mostrando corazon");
      mostrarCaracter('.');  // 👈 reutilizas tu lógica
      }


    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html\n");

       client.println("<!DOCTYPE html><html>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");

    client.println("<style>");
    client.println("body { text-align:center; font-family:Arial; background:#111; color:white; }");
    client.println("h1 { font-size:40px; }");

    client.println(".btn { display:block; width:80%; margin:20px auto; padding:25px; font-size:30px; border:none; border-radius:15px; }");

    client.println(".subir { background:#00c853; color:white; }");   // VERDE
    client.println(".bajar { background:#d50000; color:white; }");   // ROJO
    client.println(".pausa { background:#ffd600; color:black; }");   // AMARILLO

    client.println("button:active { transform: scale(0.95); }");
    client.println("</style>");

    client.println("</head>");
    client.println("<body>");

    client.println("<h1>Control ESP32</h1>");

    client.println("<a href=\"/SUBIR\"><button class='btn subir'>SUBIR</button></a>");
    client.println("<a href=\"/PAUSA\"><button class='btn pausa'>PAUSA</button></a>");
    client.println("<a href=\"/BAJAR\"><button class='btn bajar'>BAJAR</button></a>");
    client.println("<form action=\"/ENVIAR\" method=\"GET\">");
    client.println("<input type='text' name='char' maxlength='1' style='font-size:30px; text-align:center;'>");
    client.println("<br><br>");
    client.println("<input type='submit' value='Mostrar' class='btn'>");
    client.println("</form>");
    client.println("<a href=\"/CORAZON\"><button class='btn' style='background:#ff4081;'>...</button></a>");
    client.println("<a href=\"/MAGICO\"><button class='btn' style='background:#58CCE8;'>Magico</button></a>");

    client.println("</body></html>");

    client.stop();
  }

  //  LEER SERIAL DEL MONITOR (Arduino IDE)
if (miUART.available()) {
  char c = miUART.read();

  if (c == '\n' || c == '\r') return;

  if (isalpha(c)) {
    c = toupper(c);
  }

  Serial.print("Recibido: ");
  Serial.println(c);

  // 🔹 NUEVA LÓGICA DE NIVELES
// 🔹 SUBIR
if (c == '+') {
  if (nivel < 2) {
    nivel++;
    if (!pausado) mostrarBitmap(NIVELES[nivel]);
  }
}

// 🔹 BAJAR
else if (c == '-') {
  if (nivel > 0) {
    nivel--;
    if (!pausado) mostrarBitmap(NIVELES[nivel]);
  }
}

// 🔹 PAUSA
else if (c == '/') {
  pausado = true;
  mostrarBitmap(PAUSA_BITMAP);
}

// 🔹 RUN
else if (c == '<') {

  if (pausado) {
    pausado = false;

    mostrarBitmap(PAUSA_BITMAP);
    delay(300);

    mostrarBitmap(RUN_BITMAP);
    delay(300);
  }

  mostrarBitmap(NIVELES[nivel]);
}
  else {
    mostrarCaracter(c);
  }
}
}