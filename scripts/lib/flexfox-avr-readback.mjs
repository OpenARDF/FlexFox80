export function normalizeAvrdudeFlashReadback(rawFlash, flashSize) {
  if (!Buffer.isBuffer(rawFlash)) {
    throw new TypeError("avrdude flash readback must be a Buffer");
  }
  if (!Number.isInteger(flashSize) || flashSize <= 0) {
    throw new RangeError("flash size must be a positive integer");
  }
  if (rawFlash.length > flashSize) {
    throw new RangeError(
      `avrdude flash readback is ${rawFlash.length} bytes; target flash is ${flashSize} bytes`,
    );
  }

  /* avrdude's raw writer omits the erased 0xff tail even after a successful
   * full-device read. Preserve the raw evidence file, but restore that defined
   * erased tail in memory before exact bootloader/application comparisons. */
  const normalized = Buffer.alloc(flashSize, 0xff);
  rawFlash.copy(normalized);
  return normalized;
}
