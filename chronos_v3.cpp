#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char* ssid = "REDE_WIFI";
const char* password = "SENHA_WIFI";

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
  String p = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>CHRONOS_V3 UPLINK</title>
  <style>
    /* --- ESTILO GERAL --- */
    body {
      margin: 0;
      padding: 0;
      background: linear-gradient(180deg, #000000 0%, #1c1c1c 100%);
      color: #00ff41;
      font-family: 'Courier New', Courier, monospace;
      height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      overflow: hidden;
    }

    /* --- EFEITO SCANLINE (Monitor CRT) --- */
    .scanline {
      width: 100%;
      height: 100px;
      z-index: 10;
      background: linear-gradient(0deg, rgba(0,0,0,0) 0%, rgba(0, 255, 65, 0.04) 50%, rgba(0,0,0,0) 100%);
      opacity: 0.1;
      position: absolute;
      bottom: 100%;
      animation: scanline 10s linear infinite;
      pointer-events: none;
    }
    @keyframes scanline {
      0% { bottom: 100%; }
      80% { bottom: -100%; }
      100% { bottom: -100%; }
    }

    /* --- O CARTÃO PRINCIPAL --- */
    .terminal {
      background: rgba(10, 10, 10, 0.9);
      border: 1px solid #333;
      border-top: 2px solid #00ff41;
      border-bottom: 2px solid #00ff41;
      padding: 30px;
      width: 90%;
      max-width: 400px;
      box-shadow: 0 0 20px rgba(0, 255, 65, 0.15);
      position: relative;
      z-index: 2;
      text-align: center;
    }

    /* --- TIPOGRAFIA --- */
    h1 {
      font-size: 22px;
      margin-bottom: 5px;
      letter-spacing: 2px;
      text-transform: uppercase;
      text-shadow: 0 0 5px #00ff41;
    }
    .subtitle {
      font-size: 10px;
      color: #666;
      margin-bottom: 25px;
      border-bottom: 1px dashed #333;
      padding-bottom: 10px;
    }
    p { font-size: 14px; margin-bottom: 15px; }

    /* --- INPUTS --- */
    input {
      background: #050505;
      border: 1px solid #444;
      color: #fff;
      padding: 12px;
      width: 80%;
      font-family: inherit;
      font-size: 16px;
      text-align: center;
      margin-bottom: 20px;
      outline: none;
      transition: 0.3s;
    }
    input:focus {
      border-color: #00ff41;
      box-shadow: 0 0 8px rgba(0, 255, 65, 0.3);
    }

    /* --- BOTÕES --- */
    button {
      background: transparent;
      color: #00ff41;
      border: 1px solid #00ff41;
      padding: 12px 30px;
      font-size: 14px;
      font-weight: bold;
      cursor: pointer;
      text-transform: uppercase;
      letter-spacing: 1px;
      transition: all 0.2s ease-in-out;
    }
    button:hover {
      background: #00ff41;
      color: #000;
      box-shadow: 0 0 15px #00ff41;
    }

    /* --- ALERTAS E ESTADOS --- */
    .hidden { display: none; }
    
    .blink {
      animation: blinker 1s cubic-bezier(0.4, 0, 0.6, 1) infinite;
      color: #ff3333;
      font-weight: bold;
      font-size: 16px;
      margin-top: 20px;
    }
    @keyframes blinker { 50% { opacity: 0.3; } }

    .status-box {
      border: 1px solid #444;
      padding: 15px;
      margin-top: 10px;
      background: #111;
      font-size: 12px;
      color: #888;
    }
    .success { color: #00ff41; }
    .error { color: #ff3333; }

  </style>
</head>
<body>

  <div class="scanline"></div>

  <div class="terminal">
    <h1>CHRONOS_V3</h1>
    <div class="subtitle">SECURE UPLINK PROTOCOL</div>

    <div id="screen1">
      <p>IDENTIFICAÇÃO REQUERIDA:</p>
      <input type="email" id="email" placeholder="AGENTE@EMAIL.COM" autocomplete="off">
      <br>
      <button onclick="iniciarProcesso()">[ INICIAR CONEXÃO ]</button>
    </div>

    <div id="screen2" class="hidden">
      <p>AGUARDANDO VALIDAÇÃO FÍSICA</p>
      <div class="status-box">
        <span class="blink">⚠ TOQUE NO SENSOR ⚠</span>
        <br><br>
        <span style="font-size:10px;">LENDO DADOS DO GPIO_04...</span>
      </div>
    </div>

    <div id="screen3" class="hidden">
      <h2 id="titulo_final">PROCESSANDO</h2>
      <div id="msg_final" class="status-box"></div>
      <br>
      <button onclick="location.reload()">REINICIAR SISTEMA</button>
    </div>

  </div>

  <script>
    function iniciarProcesso() {
      var e = document.getElementById('email').value;
      if(!e) { alert("ERRO: CAMPO VAZIO"); return; }
      
      // Envia email e troca de tela
      fetch('/set_email?e=' + e).then(() => {
        document.getElementById('screen1').classList.add('hidden');
        document.getElementById('screen2').classList.remove('hidden');
        monitorarStatus();
      });
    }

    // Polling para saber se o ESP32 já processou o toque
    function monitorarStatus() {
      const interval = setInterval(() => {
        fetch('/status').then(r => r.text()).then(res => {
          
          if(res !== 'WAIT') {
            clearInterval(interval);
            document.getElementById('screen2').classList.add('hidden');
            document.getElementById('screen3').classList.remove('hidden');
            
            const msgBox = document.getElementById('msg_final');
            const titulo = document.getElementById('titulo_final');

            if(res === 'OFFLINE') {
               titulo.innerText = "FALHA DE UPLINK";
               titulo.style.color = "#ff3333";
               msgBox.innerHTML = "<span class='error'>ERRO CRÍTICO: CHAVES DE CRIPTOGRAFIA AUSENTES.</span><br>AGUARDE O INÍCIO DO EVENTO.";
            } else {
               titulo.innerText = "ACESSO CONCEDIDO";
               titulo.style.color = "#00ff41";
               msgBox.innerHTML = "<span class='success'>PAYLOAD ENVIADO COM SUCESSO.</span><br>VERIFIQUE SUA CAIXA DE ENTRADA.";
            }
          }
        });
      }, 1000);
    }
  </script>

</body>
</html>
)rawliteral";
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
    Serial.println(">> CHRONOS: Email recebido -> " + emailCache);
    aguardandoBiometria = true;
    envioConcluido = false;
    server.send(200, "text/plain", "RECEIVED");
  }
}

void handleStatus() {
  if (!envioConcluido) {
    server.send(200, "text/plain", "WAIT");
  } else {
    server.send(200, "text/plain", (xorKey == 0x00) ? "OFFLINE" : "OK");
  }
}

void attemptUplink() {

  Serial.println(">> CHRONOS: Biometria detectada. Processando...");
  for(int i=0; i<4; i++) { 
    digitalWrite(pinoLed, HIGH); delay(80); digitalWrite(pinoLed, LOW); delay(80); 
  }

  String realURL = decryptURL();

  if (realURL == "") {
    Serial.println(">> ALERTA: MODO TREINAMENTO. Hardware OK, mas sem link.");
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
    
    if(code > 0) Serial.println(">> SUCESSO: Dados enviados para o servidor.");
    else Serial.println(">> ERRO HTTP: " + String(code));
    
    envioConcluido = true; 
  }
  aguardandoBiometria = false;
}

void setup() {
  Serial.begin(115200);
  pinMode(pinoTouch, INPUT);
  pinMode(pinoLed, OUTPUT);

  Serial.println("\n--- INICIALIZANDO CHRONOS_V3 SYSTEM ---");
  WiFi.begin(ssid, password);
  
  Serial.print("Estabelecendo conexão");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\nCONECTADO COM SUCESSO.");
  Serial.println("---------------------------------------------");
  Serial.print(">> ACESSE O TERMINAL EM: http://");
  Serial.println(WiFi.localIP());
  Serial.println("---------------------------------------------");

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