#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h> 

const char* ssid = "NOME_WIFI";     
const char* password = "SENHA_WIFI"; 

// ============================================================
// ⚠ ÁREA DE SEGURANÇA - CLASSIFICADA ⚠
// (VOCÊ RECEBERÁ OS CÓDIGOS PARA SUBSTIRUIR OS ATUAIS NA ÁREA RESTRITA)
// ============================================================

// --- INICIO DA ÁREA RESTRITA ---
const byte xorKey = 0x00; 
const int urlLen = 1;     
const byte urlCipher[] = { 0x00 }; 
// --- FIM DA ÁREA RESTRITA ---


const int pinoTouch = 4; 
const int pinoLed = 2;   

WebServer server(80);
String emailCache = "";
bool aguardandoBiometria = false;
bool envioConcluido = false;


String getPage() {

  String p = "<!DOCTYPE html><html><body style='background:black;color:#0f0;font-family:Courier;text-align:center;'>";
  p += "<h1>NEXODER UPLINK (HOME EDITION)</h1>";
  p += "<div id='screen1'><p>INSIRA EMAIL:</p><input id='email'><button onclick='start()'>ENVIAR</button></div>";
  p += "<div id='screen2' style='display:none'><h1>TOQUE NO SENSOR AGORA</h1></div>";
  p += "<div id='screen3' style='display:none'><h1>PROCESSADO</h1><p id='msg'></p></div>";
  p += "<script>function start(){var e=document.getElementById('email').value;fetch('/set_email?e='+e).then(()=>{document.getElementById('screen1').style.display='none';document.getElementById('screen2').style.display='block';check();});}";
  p += "function check(){setInterval(()=>{fetch('/status').then(r=>r.text()).then(res=>{if(res!='WAIT'){document.getElementById('screen2').style.display='none';document.getElementById('screen3').style.display='block';document.getElementById('msg').innerText=(res=='OFFLINE'?'ERRO: FALTAM CHAVES DO EVENTO':'SUCESSO: EMAIL ENVIADO');}})},1000);}</script>";
  p += "</body></html>";
  return p;
}


String decryptURL() {
  String url = "";
  if (xorKey == 0x00) return ""; 
  for (int i = 0; i < urlLen; i++) {
    char c = (char)(urlCipher[i] ^ xorKey);
    url += c;
  }
  return url;
}

void handleRoot() { server.send(200, "text/html", getPage()); }

void handleSetEmail() {
  if (server.hasArg("e")) {
    emailCache = server.arg("e");
    Serial.println(">> Email: " + emailCache);
    aguardandoBiometria = true;
    envioConcluido = false;
    server.send(200, "text/plain", "RECEIVED");
  }
}

void handleStatus() {
  if (!envioConcluido) server.send(200, "text/plain", "WAIT");
  else server.send(200, "text/plain", (xorKey == 0x00) ? "OFFLINE" : "OK");
}

void attemptUplink() {

  for(int i=0; i<3; i++) { digitalWrite(pinoLed, HIGH); delay(50); digitalWrite(pinoLed, LOW); delay(50); }
  
  String realURL = decryptURL();

  if (realURL == "") {
    Serial.println(">> MODO DE TREINAMENTO: HARDWARE OK. AGUARDANDO EVENTO.");
    envioConcluido = true; 
    aguardandoBiometria = false;
    return;
  }

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(realURL);
    http.addHeader("Content-Type", "application/json");
    String json = "{\"email\":\"" + emailCache + "\"}";
    int code = http.POST(json);
    if(code > 0) Serial.println(">> SUCESSO! UPLINK COMPLETO.");
    else Serial.println(">> ERRO DE REDE: " + String(code));
    envioConcluido = true; 
  }
  aguardandoBiometria = false;
}

void setup() {
  Serial.begin(115200);
  pinMode(pinoTouch, INPUT);
  pinMode(pinoLed, OUTPUT);


  WiFi.begin(ssid, password);
  Serial.print("Conectando na sua rede");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nCONECTADO!");


  if (!MDNS.begin("nexoder")) { 
    Serial.println("Erro ao configurar mDNS!");
  } else {
    Serial.println("mDNS iniciado! Acesse no navegador: http://nexoder.local");
  }

  Serial.print("Ou acesse pelo IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set_email", handleSetEmail);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  delay(2); 

  if (aguardandoBiometria && !envioConcluido) {

    if (touchRead(pinoTouch) < 30) {
      attemptUplink();
      delay(2000); 
    }
  }
}