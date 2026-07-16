/*
 *  MIT License
 *
 *  Copyright (c) 2026 Digital Confections LLC
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*
 * Flash-resident ESP8266 root page with read-only keep-alive and reconnect logic.
 */

#ifndef _ROOT_PAGE_H_
#define _ROOT_PAGE_H_

#include <Arduino.h>

static const char ROOT_PAGE_HTML[] PROGMEM = R"ROOTPAGE(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>FlexFox80 Options</title>
  <style>
    body { max-width: 760px; margin: 0 auto; padding: 20px; font-family: Verdana, sans-serif; color: #111; }
    h1 { font-size: 30px; }
    nav a { display: block; padding: 10px 0; font-size: 20px; }
    #connectionStatus { padding: 10px 12px; border-radius: 4px; font-weight: bold; }
    .connecting { background: #fff3cd; color: #664d03; }
    .connected { background: #d1e7dd; color: #0f5132; }
    .disconnected { background: #f8d7da; color: #842029; }
    .warning { color: #842029; }
  </style>
</head>
<body>
  <h1>FlexFox80 Options</h1>
  <p id="connectionStatus" class="connecting">Connecting...</p>
  <nav>
    <a href="/events.html">Configure events</a>
    <a href="/upload.html">Upload a file</a>
    <a href="/download.html">Download a file</a>
    <a href="/delete.html">Delete a file</a>
    <a href="/test.html">Testing support</a>
    <a href="/radio.html">Radio mode</a>
    <a href="/firmware">Update WiFi firmware</a>
  </nav>
  <p class="warning">Use file deletion with caution.</p>
  <p><small>ESP8266 control page with automatic keep-alive.</small></p>
  <script>
    (function() {
      "use strict";
      var socket = null;
      var heartbeatTimer = 0;
      var reconnectTimer = 0;
      var stopping = false;
      var status = document.getElementById("connectionStatus");

      function setStatus(text, className) {
        status.textContent = text;
        status.className = className;
      }

      function clearHeartbeat() {
        if (heartbeatTimer) {
          clearInterval(heartbeatTimer);
          heartbeatTimer = 0;
        }
      }

      function clearReconnect() {
        if (reconnectTimer) {
          clearTimeout(reconnectTimer);
          reconnectTimer = 0;
        }
      }

      function scheduleReconnect() {
        if (stopping || reconnectTimer) return;
        reconnectTimer = setTimeout(function() {
          reconnectTimer = 0;
          connect();
        }, 2000);
      }

      function sendHeartbeat() {
        if (socket && socket.readyState === 1) {
          try {
            socket.send("!&");
          } catch (err) {
            socket.close();
          }
        }
      }

      function connect() {
        if (stopping || (socket && (socket.readyState === 0 || socket.readyState === 1))) return;
        clearReconnect();
        setStatus("Connecting...", "connecting");

        try {
          socket = new WebSocket("ws://" + location.hostname + ":81/");
        } catch (err) {
          socket = null;
          setStatus("Disconnected - retrying...", "disconnected");
          scheduleReconnect();
          return;
        }

        socket.onopen = function() {
          clearReconnect();
          setStatus("Connected - keeping transmitter awake", "connected");
          sendHeartbeat();
          clearHeartbeat();
          heartbeatTimer = setInterval(sendHeartbeat, 2000);
        };

        socket.onclose = function() {
          socket = null;
          clearHeartbeat();
          if (!stopping) {
            setStatus("Disconnected - retrying...", "disconnected");
            scheduleReconnect();
          }
        };

        socket.onerror = function() {
          setStatus("Connection error - retrying...", "disconnected");
          if (socket) socket.close();
        };
      }

      window.addEventListener("pagehide", function() {
        stopping = true;
        clearHeartbeat();
        clearReconnect();
        if (socket) socket.close();
        socket = null;
      });

      connect();
    })();
  </script>
</body>
</html>
)ROOTPAGE";

#endif // _ROOT_PAGE_H_
