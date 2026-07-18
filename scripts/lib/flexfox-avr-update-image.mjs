import { Buffer } from "node:buffer";

export const FLEXFOX_AVR_IMAGE_FORMAT = "flexfox-avr-update-v2";
export const FLEXFOX_AVR_IMAGE_FORMAT_VERSION = 2;
export const FLEXFOX_AVR_APP_START = 0x4000;
export const FLEXFOX_AVR_PAGE_SIZE = 512;
export const FLEXFOX_AVR_FLASH_SIZE = 131072;
export const FLEXFOX_AVR_TRAILER_MAGIC = "FF80UPD2";
export const FLEXFOX_AVR_TRAILER_VERSION_OFFSET = 24;
export const FLEXFOX_AVR_TRAILER_VERSION_BYTES = 16;

const crc32Table = Array.from({ length: 256 }, (_, value) => {
  let crc = value;
  for (let bit = 0; bit < 8; bit++) crc = (crc & 1) ? (0xedb88320 ^ (crc >>> 1)) : (crc >>> 1);
  return crc >>> 0;
});

export function flexFoxAvrCrc32(bytes) {
  let crc = 0xffffffff;
  for (const byte of bytes) crc = crc32Table[(crc ^ byte) & 0xff] ^ (crc >>> 8);
  return (crc ^ 0xffffffff) >>> 0;
}

export function createFlexFoxAvrUpdateImage(payload, applicationVersion) {
  if (!Buffer.isBuffer(payload) || !payload.length || payload.length % FLEXFOX_AVR_PAGE_SIZE) {
    throw new Error("FlexFox AVR application payload must contain complete nonempty pages");
  }
  if (payload.length + FLEXFOX_AVR_PAGE_SIZE > FLEXFOX_AVR_FLASH_SIZE - FLEXFOX_AVR_APP_START) {
    throw new Error("FlexFox AVR update image exceeds the application section");
  }
  const versionBytes = Buffer.from(applicationVersion, "ascii");
  if (!versionBytes.length || versionBytes.length >= FLEXFOX_AVR_TRAILER_VERSION_BYTES) {
    throw new Error("FlexFox AVR application version does not fit the update trailer");
  }
  const trailer = Buffer.alloc(FLEXFOX_AVR_PAGE_SIZE, 0xff);
  trailer.write(FLEXFOX_AVR_TRAILER_MAGIC, 0, "ascii");
  trailer.writeUInt16LE(FLEXFOX_AVR_IMAGE_FORMAT_VERSION, 8);
  trailer.writeUInt16LE(FLEXFOX_AVR_PAGE_SIZE, 10);
  trailer.writeUInt32LE(FLEXFOX_AVR_APP_START, 12);
  trailer.writeUInt32LE(payload.length, 16);
  trailer.writeUInt32LE(flexFoxAvrCrc32(payload), 20);
  versionBytes.copy(trailer, FLEXFOX_AVR_TRAILER_VERSION_OFFSET);
  trailer[FLEXFOX_AVR_TRAILER_VERSION_OFFSET + versionBytes.length] = 0;
  return Buffer.concat([payload, trailer]);
}

export function inspectFlexFoxAvrUpdateImage(image) {
  if (!Buffer.isBuffer(image) || image.length < 2 * FLEXFOX_AVR_PAGE_SIZE ||
      image.length % FLEXFOX_AVR_PAGE_SIZE ||
      image.length > FLEXFOX_AVR_FLASH_SIZE - FLEXFOX_AVR_APP_START) {
    throw new Error("FlexFox AVR update image size or alignment is invalid");
  }
  const payloadBytes = image.length - FLEXFOX_AVR_PAGE_SIZE;
  const trailer = image.subarray(payloadBytes);
  if (trailer.subarray(0, 8).toString("ascii") !== FLEXFOX_AVR_TRAILER_MAGIC ||
      trailer.readUInt16LE(8) !== FLEXFOX_AVR_IMAGE_FORMAT_VERSION ||
      trailer.readUInt16LE(10) !== FLEXFOX_AVR_PAGE_SIZE ||
      trailer.readUInt32LE(12) !== FLEXFOX_AVR_APP_START ||
      trailer.readUInt32LE(16) !== payloadBytes) {
    throw new Error("FlexFox AVR update product trailer is invalid");
  }
  const nul = trailer.indexOf(0, FLEXFOX_AVR_TRAILER_VERSION_OFFSET);
  if (nul < FLEXFOX_AVR_TRAILER_VERSION_OFFSET ||
      nul >= FLEXFOX_AVR_TRAILER_VERSION_OFFSET + FLEXFOX_AVR_TRAILER_VERSION_BYTES) {
    throw new Error("FlexFox AVR update trailer version is invalid");
  }
  const applicationVersion = trailer.subarray(FLEXFOX_AVR_TRAILER_VERSION_OFFSET, nul).toString("ascii");
  const payload = image.subarray(0, payloadBytes);
  const applicationPayloadCrc32 = flexFoxAvrCrc32(payload);
  if (applicationPayloadCrc32 !== trailer.readUInt32LE(20)) {
    throw new Error("FlexFox AVR update payload CRC does not match its product trailer");
  }
  return {
    applicationVersion,
    applicationPayloadBytes: payloadBytes,
    applicationPayloadCrc32,
    imageBytes: image.length,
    imageCrc32: flexFoxAvrCrc32(image),
  };
}
