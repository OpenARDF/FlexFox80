<h2>Receiver Development Platform</h2>
<p>
	This project contains source code for the&nbsp;<a href="https://groups.google.com/forum/#!forum/receiver-development-platform">Receiver Development Platform</a>&nbsp;and&nbsp;<a href="http://openardf.org/index.php/ardf-open-equipment-project/">Open ARDF Equipment Project</a>. When this software is built using Atmel Studio 7 it will generate object code to run on the processors utilized in that project. The following project components utilize this software:
</p>
<p>
	<strong>Control Head&nbsp;</strong>- A user interface device capable of communicating over a serial bus (Linkbus)
</p>
<p>
	<strong>Digital Interface&nbsp;</strong>- A small board that provides a digital interface between a serial bus (Linkbus) to a radio receiver (or other device) to which it is connected.
</p>
<p>This project is written in the C programming language, using the Atmel Studio 7 integrated development environment, and is built to run on an ATmega328P microprocessor.</p>
<p>Due to the limited memory resources of the target processor, all software in this project was written to minimize the size of the object file. Driver files are written to contain only the functionality required for this project, so they do not necessarily support all the features of the devices for which they are written. This is an embedded project, with limited opportunities for the user to enter illegal values or perform illegal actions, so error checking is often omitted in order to improve performance and reduce object file size.</p>
<p>Main program entry and all interrupt service routines (ISRs) are contained in main.c. Most program control pre-compiler definitions are contained in defs.h. Certain major features are held in their own file modules, including:</p>
<ul>
	<li>
		<strong>Linkbus</strong> (linkbus.c) - a simple serial inter-processor communication protocol
	</li>
	<li>
		<strong>Receiver</strong> (receiver.c) - support specific to the dual-band ARDF receiver board
	</li>
</ul>
<p>Minimalist driver support is contained in files named for the supported hardware device:</p>
<ul>
	<li>
		<a href="http://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf">Si5351</a>: I2C configurable clock generator
	</li>
	<li>
		<a href="http://www.nxp.com/documents/data_sheet/PCF8574_PCF8574A.pdf">PCF8574</a>: General-purpose remote I/O expansion via I2C-bus
	</li>
	<li>
		<a href="http://datasheets.maximintegrated.com/en/ds/DS3231-DS3231S.pdf">DS3231</a>: Extremely Accurate I2C-Integrated RTC/TCXO/Crystal
	</li>
	<li>
		<a href="http://www.analog.com/media/en/technical-documentation/data-sheets/AD5245.pdf">AD5245</a>: 256-Position I2C-Compatible Digital Potentiometer
	</li>
	<li>
		<a href="http://www.newhavendisplay.com/app_notes/ST7036.pdf">ST7036</a>: LCD controller with I2C - with support for the <a href="http://www.newhavendisplay.com/specs/NHD-C0220BiZ-FSW-FBW-3V3M.pdf">NHD‐C0220BiZ‐FSW‐FBW‐3V3M</a>&nbsp;2x20 character LCD&nbsp;
	</li>
	<li>
		TODO: Add <a href="https://www.adafruit.com/product/1120">LSM303DLHC</a>&nbsp;compass module support
	</li>
	<li>
		TODO: Add <a href="http://www.ti.com/lit/ds/symlink/dac081c081.pdf">DAC081C085</a>&nbsp;support
	</li>
	<li>
		TODO: Add <a href="http://datasheets.maximintegrated.com/en/ds/MAX5477-MAX5479.pdf">MAX5478EUD+</a>&nbsp;support
	</li>
	<li>
		TODO: Add <a href="http://www.atmel.com/Images/Atmel-8815-SEEPROM-AT24CS01-02-Datasheet.pdf">AT24CS01-STUM</a>&nbsp;support
	</li>
</ul>
<p>
	This project builds with a minimum of effort under the <a href="http://www.atmel.com/microsite/atmel-studio/">Atmel Studio 7 Integrated Development Environment</a>.
</p>
<p>TODO: Configuration information and import instructions for using this code under Atmel Studio 7:</p>
<p>
	All original source code is released under the "<a href="https://opensource.org/licenses/MIT">MIT License</a>"
</p>