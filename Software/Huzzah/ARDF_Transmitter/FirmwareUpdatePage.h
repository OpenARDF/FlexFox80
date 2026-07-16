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
 * Flash-resident, sketch-only firmware update page.
 */

#ifndef FIRMWARE_UPDATE_PAGE_H_
#define FIRMWARE_UPDATE_PAGE_H_

#include <Arduino.h>

static const char FIRMWARE_UPDATE_PAGE_HTML[] PROGMEM = R"FIRMWAREPAGE(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>FlexFox80 WiFi Firmware Update</title>
  <style>
    body { max-width: 760px; margin: 0 auto; padding: 20px; font-family: Verdana, sans-serif; color: #111; }
    h1 { font-size: 28px; }
    .panel { border: 1px solid #aaa; border-radius: 6px; padding: 16px; margin: 16px 0; }
    .warning { color: #842029; font-weight: bold; }
    #status { padding: 10px 12px; background: #fff3cd; color: #664d03; border-radius: 4px; }
    button { margin-top: 12px; padding: 10px 14px; font-size: 16px; }
  </style>
</head>
<body>
  <p><a href="/">[HOME]</a></p>
  <h1>Update WiFi firmware</h1>
  <p id="status">Reading device status...</p>
  <div class="panel">
    <p class="warning">Use only a FlexFox80 ESP8266 sketch .bin file. Do not remove power during the upload or reboot.</p>
    <p>This updates the WiFi sketch only. Event, identity, and configuration files in LittleFS are not modified.</p>
    <form id="firmwareForm" method="post" action="/firmware" enctype="multipart/form-data">
      <label for="firmwareFile">Firmware file:</label>
      <input id="firmwareFile" type="file" accept=".bin,application/octet-stream" name="firmware" required>
      <br>
      <button id="updateButton" type="submit">Validate and update firmware</button>
    </form>
  </div>
  <p id="message" role="status" aria-live="polite"></p>
  <script>
    (function() {
      "use strict";
      var form = document.getElementById("firmwareForm");
      var fileInput = document.getElementById("firmwareFile");
      var button = document.getElementById("updateButton");
      var status = document.getElementById("status");
      var message = document.getElementById("message");
      var socket = null;
      var heartbeat = 0;

      function crc32(bytes) {
        var crc = 0xffffffff;
        for (var index = 0; index < bytes.length; index += 1) {
          crc ^= bytes[index];
          for (var bit = 0; bit < 8; bit += 1) {
            crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
          }
        }
        return (crc ^ 0xffffffff) >>> 0;
      }

      function readFile(file) {
        if (file.arrayBuffer) return file.arrayBuffer();
        return new Promise(function(resolve, reject) {
          var reader = new FileReader();
          reader.onload = function() { resolve(reader.result); };
          reader.onerror = function() { reject(reader.error || new Error("Unable to read firmware file")); };
          reader.readAsArrayBuffer(file);
        });
      }

      function connectKeepAlive() {
        try {
          socket = new WebSocket("ws://" + location.hostname + ":81/");
          socket.onopen = function() {
            socket.send("!&");
            heartbeat = setInterval(function() {
              if (socket && socket.readyState === 1) socket.send("!&");
            }, 2000);
          };
        } catch (ignore) {}
      }

      fetch("/firmware/status", { cache: "no-store" }).then(function(response) {
        if (!response.ok) throw new Error("status unavailable");
        return response.json();
      }).then(function(info) {
        status.textContent = "Running WiFi firmware " + info.version +
          (info.cloneActive ? " - cloning is active; updates are blocked" : " - ready for a sketch-only update");
      }).catch(function() {
        status.textContent = "Device status unavailable. Do not update until the connection is stable.";
      });

      form.addEventListener("submit", function(event) {
        event.preventDefault();
        var file = fileInput.files && fileInput.files[0];
        if (!file) return;
        button.disabled = true;
        message.textContent = "Validating firmware file...";

        readFile(file).then(function(buffer) {
          var bytes = new Uint8Array(buffer);
          if (bytes.length < 4096 || bytes[0] !== 0xe9) {
            throw new Error("This is not an uncompressed ESP8266 sketch .bin file.");
          }
          var checksum = crc32(bytes).toString(16).padStart(8, "0");
          if (!window.confirm("Install " + file.name + " (" + bytes.length + " bytes) and reboot the ESP8266?")) {
            button.disabled = false;
            message.textContent = "Update cancelled.";
            return;
          }
          message.textContent = "Uploading. Keep power connected...";
          form.action = "/firmware?confirm=UPDATE&size=" + bytes.length + "&crc32=" + checksum;
          form.submit();
        }).catch(function(error) {
          button.disabled = false;
          message.textContent = error && error.message ? error.message : "Firmware validation failed.";
        });
      });

      window.addEventListener("pagehide", function() {
        if (heartbeat) clearInterval(heartbeat);
        if (socket) socket.close();
      });
      connectKeepAlive();
    })();
  </script>
</body>
</html>
)FIRMWAREPAGE";

static const char FIRMWARE_UPDATE_SUCCESS_HTML[] PROGMEM = R"FIRMWARESUCCESS(
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><meta http-equiv="refresh" content="20;url=/"><title>Update accepted</title></head><body><h1>Firmware validated</h1><p>The sketch was received completely and passed its integrity check. The ESP8266 is rebooting. This page will return home after 20 seconds.</p></body></html>
)FIRMWARESUCCESS";

#endif // FIRMWARE_UPDATE_PAGE_H_
