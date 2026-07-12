# EEPROM Enum-Width ABI Evidence — 2026-07-12

## Scope

- Branch: `Development_AVR128DA48`
- Fix commit: `57d70a7` (`Stabilize EEPROM enum field widths`)
- Target: AVR128DA48 EEPROM layout shared by historical Debug and current Release builds

This slice was discovered during the final review before programming the connected FlexFox test unit. Programming was stopped before any flash, EEPROM, fuse, or lock-bit write occurred.

## Failure mechanism

The connected unit's flash is byte-identical to the tracked Debug image from commit `33e64e0`. That Debug build does not enable short enums, and its DWARF data records the persisted `Fox_t` and `Event_t` members as two bytes. Its `EE_prom` object is 274 bytes.

The checked-in Microchip Studio Release configuration enables `AllocateBytesNeededForEnum`, which the portable wrapper reproduces as `-fshort-enums`. Before this fix, six persisted members were declared directly as `Fox_t` or `Event_t`. Under the Release ABI they became one byte each. Source-layout evaluation therefore produced a 268-byte Release object and shifted all fields following the first enum-valued member relative to the deployed 274-byte Debug schema.

Flashing such a Release image while retaining deployed EEPROM could cause the firmware to read configuration, frequencies, and later fields at the wrong addresses. The build flag itself is a legitimate Release setting, so removing it globally would be a broader and less explicit ABI change.

## Red-green correction

The layout contract was first extended to require:

- five persisted fox-selection fields and the persisted event-selection field to use `uint16_t`;
- each following guard offset to advance by `sizeof(uint16_t)`.

Against the pre-fix declarations, the test failed red with twelve expected failures: six field-width failures and six dependent-offset failures.

Commit `57d70a7` then makes only the persisted representation explicit:

- all six enum-valued EEPROM fields are `uint16_t`;
- their offset expressions use `sizeof(uint16_t)`;
- an AVR compile-time assertion requires `sizeof(EE_prom) == 274`;
- runtime enum values, field order, low-byte reads and writes, defaults, and enum definitions remain unchanged.

The host layout contract passes green for all 65 fields and a total size of 274 bytes.

## Exact AVR Release evidence

The exact archived Mac toolchain was used:

- AVR-GCC 7.3.0;
- Atmel `AVR-Dx_DFP` 1.9.103;
- repository Release wrapper with `-fpack-struct -fshort-enums`.

The final linker map reports:

```text
.eeprom         0x0000000000810000      0x112
 .eeprom        0x0000000000810000      0x112 .../src/eeprommanager.o
                0x0000000000810000      EepromManager::ee_vars
                0x0000000000810112      __eeprom_end
```

`0x112` is 274 bytes. Two consecutive final-source builds produced byte-identical evidence and zero warnings:

```text
   text    data     bss     dec     hex
  40178    1106    1137   42421    a5b5
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `0ebc766cbc12365719669c9274da57e5c9307d192cdcc0e04965d17b870c4e2a` |
| `FlexFox80.hex` | `0ef047f859bc9d6aa2bcd8b6b717feaecbf98e77c8b73f70d8e836c6ac9dbe5a` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `0d9e313fad9a4434fb2f3186c03343515169b4fde7d5059b365fe77227b53a42` |
| `FlexFox80.lss` | `f9c1a71c500eacf11efd8c13507e0e11ce878cf49a18fe71c86189942cc0efea` |
| `FlexFox80.srec` | `aedcdf11fd904532d11495509929802094f6cb23644074001de2f098554167d1` |

The executable grew by eight text bytes from the prior exact Mac reference build. Data and BSS totals are unchanged.

## Release rule and remaining qualification

No AVR image may be flashed over retained deployed EEPROM unless its target build proves the 274-byte schema. The checked-in source contract, AVR `static_assert`, and linker-map size are complementary gates.

The fix is locally deterministic and schema-compatible, but the new executable is not yet hardware-qualified. Required follow-up is:

1. exact Windows reference builds from `57d70a7` or a documented descendant;
2. Mac UPDI programming using the preserved pre-test flash, EEPROM, and fuse captures as rollback evidence;
3. immediate post-program EEPROM and fuse comparison;
4. controlled functional testing with RF safety conditions established.
