#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include "wserial.h"

const char *hostName = "esp32name";
const char *apSsid = "ESP32-Roteador";
const char *apPassword = "12345678";

WebServer server(80);

void paginaInicial() {
  String html =
      "<!DOCTYPE html>"
      "<html lang='pt-br'>"
      "<head>"
      "<meta charset='utf-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1'>"
      "<title>ESP32</title>"
      "<style>"
      "*{box-sizing:border-box}"
      "body{margin:0;font-family:Arial,Helvetica,sans-serif;background:#101418;color:#eef3f7}"
      "main{min-height:100vh;padding:24px;display:flex;align-items:center;justify-content:center}"
      ".painel{width:min(920px,100%);background:#171d23;border:1px solid #2c3844;border-radius:8px;padding:22px;box-shadow:0 18px 50px #0008}"
      ".topo{display:flex;gap:16px;align-items:flex-start;justify-content:space-between;margin-bottom:18px;flex-wrap:wrap}"
      "h1{font-size:28px;margin:0 0 8px;color:#ffffff}"
      "p{margin:0;color:#aebbc7;line-height:1.45}"
      ".status{display:flex;gap:10px;align-items:center;color:#9fffc2;background:#102117;border:1px solid #245b38;border-radius:6px;padding:10px 12px;font-weight:bold}"
      ".led{width:10px;height:10px;border-radius:50%;background:#35ff78;box-shadow:0 0 16px #35ff78}"
      ".scope{background:#050806;border:1px solid #35533d;border-radius:8px;padding:12px}"
      "canvas{width:100%;height:360px;display:block;background:#061008;border-radius:4px}"
      ".rodape{display:grid;grid-template-columns:repeat(4,1fr);gap:10px;margin-top:14px}"
      ".medida{background:#101820;border:1px solid #293946;border-radius:6px;padding:10px}"
      ".rotulo{display:block;color:#8da1b2;font-size:12px;margin-bottom:4px}"
      ".valor{font-size:18px;color:#f5fbff;font-weight:bold}"
      "@media(max-width:640px){main{padding:12px}.painel{padding:16px}h1{font-size:22px}canvas{height:260px}.rodape{grid-template-columns:repeat(2,1fr)}}"
      "</style>"
      "</head>"
      "<body>"
      "<main>"
      "<section class='painel'>"
      "<div class='topo'>"
      "<div>"
      "<h1>ESP32 Osciloscopio</h1>"
      "<p>Pagina inicial em esp32name.local servida pela propria ESP32.</p>"
      "</div>"
      "<div class='status'><span class='led'></span>ONLINE</div>"
      "</div>"
      "<div class='scope'>"
      "<canvas id='scope' width='900' height='360'></canvas>"
      "</div>"
      "<div class='rodape'>"
      "<div class='medida'><span class='rotulo'>Canal</span><span class='valor'>CH1</span></div>"
      "<div class='medida'><span class='rotulo'>Escala V</span><span class='valor'>1 V/div</span></div>"
      "<div class='medida'><span class='rotulo'>Tempo</span><span class='valor'>5 ms/div</span></div>"
      "<div class='medida'><span class='rotulo'>Sinal</span><span class='valor'>Seno</span></div>"
      "</div>"
      "</section>"
      "</main>"
      "<script>"
      "const canvas=document.getElementById('scope');"
      "const ctx=canvas.getContext('2d');"
      "let fase=0;"
      "function grade(){"
      "ctx.clearRect(0,0,canvas.width,canvas.height);"
      "ctx.fillStyle='#061008';ctx.fillRect(0,0,canvas.width,canvas.height);"
      "ctx.strokeStyle='#16361f';ctx.lineWidth=1;"
      "for(let x=0;x<=canvas.width;x+=45){ctx.beginPath();ctx.moveTo(x,0);ctx.lineTo(x,canvas.height);ctx.stroke();}"
      "for(let y=0;y<=canvas.height;y+=36){ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(canvas.width,y);ctx.stroke();}"
      "ctx.strokeStyle='#2d6b3c';ctx.lineWidth=2;"
      "ctx.beginPath();ctx.moveTo(0,canvas.height/2);ctx.lineTo(canvas.width,canvas.height/2);ctx.stroke();"
      "ctx.beginPath();ctx.moveTo(canvas.width/2,0);ctx.lineTo(canvas.width/2,canvas.height);ctx.stroke();"
      "}"
      "function onda(){"
      "ctx.strokeStyle='#60ff8b';ctx.lineWidth=3;ctx.shadowColor='#60ff8b';ctx.shadowBlur=12;"
      "ctx.beginPath();"
      "for(let x=0;x<canvas.width;x++){"
      "let ruido=Math.sin((x+fase)*0.19)*4;"
      "let y=canvas.height/2+Math.sin((x+fase)*0.035)*90+Math.sin((x+fase)*0.011)*22+ruido;"
      "if(x==0)ctx.moveTo(x,y);else ctx.lineTo(x,y);"
      "}"
      "ctx.stroke();ctx.shadowBlur=0;"
      "}"
      "function texto(){"
      "ctx.fillStyle='#bfffd0';ctx.font='16px Arial';"
      "ctx.fillText('CH1  1.00V/div   5.00ms/div',18,28);"
      "ctx.fillText('Trig: Auto',canvas.width-120,28);"
      "}"
      "function animar(){grade();onda();texto();fase+=3;requestAnimationFrame(animar);}"
      "animar();"
      "</script>"
      "</body>"
      "</html>";

  server.send(200, "text/html", html);
}

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);

  wserial::setup(115200, 47268UL);
  wserial::onInputReceived([](std::string str) { wserial::println(str.c_str()); });

  wserial::println("[AP] rede: " + String(apSsid));
  wserial::println("[AP] senha: " + String(apPassword));
  wserial::println("[IP] is " + WiFi.softAPIP().toString());

  if (MDNS.begin(hostName)) {
    MDNS.addService("http", "tcp", 80);
    wserial::println("[mDNS] acesse http://" + String(hostName) + ".local/");
  } else {
    wserial::println("[mDNS] falhou");
  }

  server.on("/", paginaInicial);
  server.begin();
  wserial::println("[HTTP] servidor iniciado");
}

void loop() {
  server.handleClient();
  wserial::loop();
}
