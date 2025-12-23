# Chronos_v3 Uplink

> **CLASSIFICAÇÃO:** ULTRA-SECRETO // SOMENTE AUTORIZADOS
> **STATUS:** AGUARDANDO CHAVES DE CRIPTOGRAFIA
> **MISSÃO:** OPERAÇÃO BLACKOUT (Event)

![Build Status](https://img.shields.io/badge/SYSTEM-ONLINE-brightgreen) ![Security](https://img.shields.io/badge/SECURITY-BIOMETRIC-red) ![Platform](https://img.shields.io/badge/HARDWARE-ESP32-blue)

## ⚠️ AVISO DE INTERCEPTAÇÃO

Se você está lendo isso, você foi recrutado.

Este repositório contém o **firmware base** para o dispositivo de intrusão física **CHRONOS_V3**. Sua missão é montar este hardware, configurar a conexão de rede local e aguardar a transmissão das **Coordenadas XOR (Chaves de Acesso)** que serão reveladas no Discord.

QUER SE TORNAR UM AGENTE?
Link do Discord: 

O dispositivo simula um terminal de acesso seguro com verificação biométrica. Somente Operadores com o hardware correto e o código decifrado conseguirão extrair o **Payload (Voucher/Recompensa)**.

---

## 🛠️ O ARSENAL (Hardware Necessário)

Para participar da operação, você precisa ter em mãos o seguinte equipamento:

* **1x Módulo ESP32 WROOM 32bits WIFI** (NodeMCU, DevKit V1 ou similar).
* **1x Cabo Micro-USB** (Certifique-se que transfere dados, não apenas carga).
* **1x led vermelho**.
* **1x resistor** (10ohm).
* **1x Jumper macho-fêmea ou Fio Condutor** (Conectado ao GPIO 4).
* **3X Jumper fêmea-fêmea** (Conectado ao GPIO 2, led, resistor e GND)
* **1x Computador** com Arduino IDE configurada.

### OPCIONAIS

* **1x case para cyberdeck** (fita VHS, K7 velha, mala de ferramentas pequena, etc)
* **1x protoboard** (para interligar outros dispositivos).
* **1x arruela, moeda ou objeto metálico** (para sensor "biométrico").


### 🧬 Esquema de Montagem (Biometria Fake)

O sistema utiliza um sensor capacitivo para simular leitura de DNA/Digital.

1.  Conecte o fio/jumper na porta **GPIO 4** (D4).
2.  Deixe a outra ponta do fio exposta (você tocará nela para autenticar).
3.  O LED embutido (**GPIO 2**) servirá como feedback de processamento.

---

## 💾 PROTOCOLO DE INSTALAÇÃO

Siga os passos abaixo com extrema precisão. Falhas resultarão em erro de conexão.

### 1. Clonagem e Preparação
Baixe este repositório ou copie o código `chronos_v3.cpp`.

### 2. Configuração de Rede (CRÍTICO)
Abra o código e localize a seção de rede. Você **DEVE** inserir as credenciais do Wi-Fi da sua casa.

```
// ============================================================
// CONFIGURAÇÕES DE REDE (PREENCHA COM SUA WIFI DE CASA)
// ============================================================
const char* ssid = "SUA_REDE_WIFI_AQUI";     // <--- EDITE ISTO
const char* password = "SUA_SENHA_AQUI";     // <--- EDITE ISTO

```
### EXEMPLO DE CYBERDECK CUSTOMIZADO PARA INSPIRAÇÃO

<img width="1376" height="768" alt="cyberdeck_example" src="https://github.com/user-attachments/assets/2873928b-8665-4140-9e70-35b7b585b022" />

