#include <ESP8266HTTPUpdateServer.h>
#include "HostedPages.h"

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer server(80); //Server on port 80

const String APHostname = "CelestronESPWifi";

boolean isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}


/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean CaptivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(APHostname) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void CloseAP()
{
  WiFi.softAPdisconnect(false);
  WiFi.enableAP(false);
}

void ServePage(String page_name)
{
  Serial.print("Serving web page page: ");
  Serial.println(page_name);
  server.send(200, GetPageMime(page_name), GetPageContents(page_name));
}

void handleRoot() {
  ServePage("index");
}

void handleCSS() {
  ServePage("style");
}

void handleJS() {
  ServePage("script");
}

void handleRestart() {
    CloseAP();
    ESP.restart();
}

void handleNotFound() {
  if (CaptivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++) {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}

void StartupHostHTTPServer()
{
  server.on("/", handleRoot);
  server.on("/style.css", handleCSS);
  server.on("/script.js", handleJS);
  server.on("/restart", handleRestart);
  server.onNotFound(handleNotFound);
  httpUpdater.setup(&server);
  server.begin();
}
