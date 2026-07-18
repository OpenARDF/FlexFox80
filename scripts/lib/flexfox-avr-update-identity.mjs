export function normalizeMacDerivedDeviceSsid(value) {
  if(typeof value !== "string" || !/^Tx_[0-9A-F]{8}$/i.test(value)) return null;
  return value.toUpperCase();
}

export function unattendedUpdateNeedsExpectedSsid(dryRun, stdinIsTty, expectedDeviceSsid) {
  return !dryRun && !stdinIsTty && !expectedDeviceSsid;
}
