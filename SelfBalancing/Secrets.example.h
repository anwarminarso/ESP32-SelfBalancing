#ifndef _SECRETS_H_
#define _SECRETS_H_

// -----------------------------------------------------------------------------
// Template for Secrets.h. Copy this file to "Secrets.h" (same folder) and fill
// in your own values. Secrets.h is git-ignored so your credentials stay local.
// -----------------------------------------------------------------------------

// WiFi station (your existing network) credentials
#define WIFI_STA_SSID       "YOUR_WIFI_SSID"
#define WIFI_STA_PASSWORD   "YOUR_WIFI_PASSWORD"

// Soft-AP (the robot's own hotspot) credentials
#define WIFI_AP_SSID        "Self Balancing Robot"
#define WIFI_AP_PASSWORD    "CHANGE_ME_AP_PASSWORD"

// HTTP / WebSocket basic-auth credentials.
// Protects the web UI, REST API and the control WebSocket.
#define HTTP_AUTH_USER      "admin"
#define HTTP_AUTH_PASS      "CHANGE_ME_HTTP_PASSWORD"

#endif
