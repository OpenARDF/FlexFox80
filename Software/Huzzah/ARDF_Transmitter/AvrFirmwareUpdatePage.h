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
 * Flash-resident operator page for staged and SSID-authorized AVR updates.
 */
#ifndef FLEXFOX_AVR_FIRMWARE_UPDATE_PAGE_H_
#define FLEXFOX_AVR_FIRMWARE_UPDATE_PAGE_H_

#include <Arduino.h>

static const char AVR_FIRMWARE_UPDATE_PAGE_HTML[] PROGMEM = R"AVRPAGE(
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>FlexFox80 AVR Update</title><style>body{max-width:760px;margin:auto;padding:20px;font-family:Verdana,sans-serif}.panel{border:1px solid #aaa;border-radius:6px;padding:16px;margin:16px 0}.warning{color:#842029;font-weight:bold}button{padding:10px 14px;font-size:16px}</style></head><body>
<p><a href="/">[HOME]</a></p><h1>Update AVR firmware</h1><p id="status">Reading recovery state...</p>
<div class="panel"><p class="warning">Use only the page-aligned FlexFox80 AVR update .bin. Keep the transmitter powered until it reports the new AVR version.</p><form id="stage" method="post" action="/avr-update" enctype="multipart/form-data"><input id="file" type="file" accept=".bin,application/octet-stream" name="firmware" required><p><label>Target version <input id="version" value="0.210" required></label></p><button type="submit">Validate and stage</button></form></div>
<div class="panel"><p>After staging, make sure no event is active. This ESP's unique device SSID is <strong id="deviceSsid">being read...</strong>. Enter its final four characters to authorize bootloader entry.</p><p><label>SSID suffix <input id="unlock" maxlength="4" size="6" autocomplete="off" required></label></p><button id="start" disabled>Start staged update</button></div><p id="message"></p>
<script>(function(){"use strict";var status=document.getElementById("status"),message=document.getElementById("message"),start=document.getElementById("start"),form=document.getElementById("stage"),input=document.getElementById("file"),unlock=document.getElementById("unlock"),deviceSsid=document.getElementById("deviceSsid");function crc32(bytes){var crc=0xffffffff;for(var i=0;i<bytes.length;i++){crc^=bytes[i];for(var b=0;b<8;b++)crc=(crc>>>1)^((crc&1)?0xedb88320:0)}return(crc^0xffffffff)>>>0}function refresh(){fetch("/avr-update/status",{cache:"no-store"}).then(function(r){return r.json()}).then(function(info){var fs=typeof info.filesystemFreeBytes==="number"?", filesystem free "+Math.floor(info.filesystemFreeBytes/1024)+" KiB":"";status.textContent="State: "+info.phase+(info.targetVersion?", target "+info.targetVersion:"")+fs;deviceSsid.textContent=info.deviceSsid||"unavailable";start.disabled=info.phase!=="staged"})}form.addEventListener("submit",function(e){e.preventDefault();var file=input.files&&input.files[0];if(!file)return;file.arrayBuffer().then(function(buffer){var bytes=new Uint8Array(buffer);if(!bytes.length||bytes.length%512||bytes.length>114688||bytes[0]===255&&bytes[1]===255)throw new Error("File is not a valid page-aligned FlexFox80 AVR image");var crc=crc32(bytes).toString(16).padStart(8,"0"),version=document.getElementById("version").value;if(!confirm("Stage "+file.name+" for AVR "+version+"?"))return;form.action="/avr-update?confirm=STAGE&size="+bytes.length+"&crc32="+crc+"&version="+encodeURIComponent(version);form.submit()}).catch(function(err){message.textContent=err.message})});start.onclick=function(){var suffix=unlock.value.trim().toUpperCase();if(!/^[0-9A-F]{4}$/.test(suffix)){message.textContent="Enter the final four characters of the unique Tx_ SSID";return}if(!confirm("Authorize this staged update for "+deviceSsid.textContent+"? Do not remove power."))return;start.disabled=true;message.textContent="Requesting bootloader entry. The WiFi connection will drop during programming.";fetch("/avr-update/start",{method:"POST",headers:{"Content-Type":"application/x-www-form-urlencoded"},body:"confirm=START&unlock="+encodeURIComponent(suffix)}).then(function(r){return r.text().then(function(t){if(!r.ok)throw new Error(t);message.textContent=t})}).catch(function(err){message.textContent=err.message;setTimeout(refresh,1500)})};refresh()})();</script></body></html>
)AVRPAGE";

#endif
