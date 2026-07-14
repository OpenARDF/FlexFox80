# Windows Accumulated Hardening Verification - 2026-07-12

Source commit tested: `3bc10a5` (`Development_AVR128DA48`)

This verification was run after fast-forwarding `Development_AVR128DA48` to mailbox request commit `3048d1e`, which includes the accumulated EEPROM-width, bounded text-send, and Linkbus receive bounds hardening.

## Build environment

- Host: Windows 11 ARM64 VM, PowerShell 7 shell.
- AVR toolchain root: `C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain`
- Compiler: `avr-g++.exe (AVR_8_bit_GNU_Toolchain_3.7.0_1796) 7.3.0`
- `avr-g++.exe -dumpversion`: `7.3.0`
- Device pack root: `C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.9.103`
- Wrapper result for both runs: `reference-version-match`

Build command:

```powershell
$env:AVR_TOOLCHAIN_ROOT='C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain'
$env:AVR_DFP_ROOT='C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.9.103'
node .\scripts\build-avr-release.mjs
```

Each run was a clean AVR Release wrapper build. Outputs were copied to ignored snapshots under `Software\AVR128DA48\tmp\accumulated-hardening-verification` only for local comparison and were not committed.

## Warning result

Both clean builds completed with zero warnings. No remaining or new warnings were reported by the wrapper.

## EEPROM layout evidence

The linker map for both runs reports `.eeprom` size `0x112`.

Run 1 map excerpt:

```text
.eeprom        0x0000000000810000      0x112
 .eeprom       0x0000000000810000      0x112 ...\src\eeprommanager.o
               0x0000000000810000                _ZN13EepromManager7ee_varsE
               0x0000000000810112                __eeprom_end = .
```

Run 2 reports the same addresses and size. `EepromManager::ee_vars` starts at `0x810000` and `__eeprom_end` is `0x810112`, so the span is exactly `0x112` bytes, or 274 bytes.

## `avr-size` output

Both runs produced:

```text
   text    data     bss     dec     hex filename
  40256    1106    1137   42499    a603 C:\Users\charl\Documents\GitHub\FlexFox80\Software\AVR128DA48\tmp\avr-release\FlexFox80.elf
```

This matches Mac's reported resource totals for the same source: `text=40256`, `data=1106`, `bss=1137`, `dec=42499`, `hex=a603`.

## Artifact hashes

| File | Run 1 SHA-256 | Run 2 SHA-256 | Run hashes match | Mac same-source comparison |
| --- | --- | --- | --- | --- |
| `FlexFox80.elf` | `18fcedd99b99cf1cd5366bc4a1cd18f0528a728fe93c8dbfe6cda46e12a1b61f` | `18fcedd99b99cf1cd5366bc4a1cd18f0528a728fe93c8dbfe6cda46e12a1b61f` | Yes | Differs from Mac `21aa2ff7628b7a2fd60bbcdf65af7f1a81f08e58bbb83be3f51ce87135263306` |
| `FlexFox80.hex` | `e02ea26c5182f0b272e5c8888fb73b4cb93fdad3fbee6670d27ea4a9ae8fd3e5` | `e02ea26c5182f0b272e5c8888fb73b4cb93fdad3fbee6670d27ea4a9ae8fd3e5` | Yes | Matches Mac |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` | Yes | Matches Mac |
| `FlexFox80.lss` | `6a2cd1feee55f2afedc621c5ba738359317e933ef7a2b697e88e5fd47ea774cb` | `6a2cd1feee55f2afedc621c5ba738359317e933ef7a2b697e88e5fd47ea774cb` | Yes | Differs from Mac `2203fb65df77962b0c26f7ad5ebe2e17e0b3d5e70fda53d1fed1b21a86890e68` |
| `FlexFox80.map` | `0f6ded5b319f3173be07a7802083c39da44c422fda9b84db014a24af29849857` | `0f6ded5b319f3173be07a7802083c39da44c422fda9b84db014a24af29849857` | Yes | Differs from Mac `05ba9bcd83e5e9ff3bd3f6681486a68265082a12cb4140d619e9c37b7a34c0b2` |
| `FlexFox80.srec` | `4774aef79639c5856585079eeca91a5f8fb2d4ea3e0922ce1644e872728d1ea0` | `4774aef79639c5856585079eeca91a5f8fb2d4ea3e0922ce1644e872728d1ea0` | Yes | Differs from Mac `ce801cc55bea3ac366d3643ca90a2cbeb4722f84b494d5dd716bbc4ad548ebb0` |

The Windows runs are internally deterministic for every requested artifact. The Intel HEX and EEPROM outputs match Mac byte-for-byte, confirming the same flash payload and EEPROM initializer for the source snapshot. ELF, MAP, and LSS are expected to be host-sensitive because they include local paths and tool-output formatting. SREC is deterministic on Windows but does not match Mac; because the same-source HEX matches exactly and the resource totals match, this appears limited to generated S-record representation rather than a firmware payload difference.

## `just check`

`just check` passed with the documented Windows shims and sanitizer setting:

```powershell
$env:PATH="Software\AVR128DA48\tmp\check-shims;C:\Program Files\LLVM\bin;C:\Program Files\Git\bin;C:\Program Files\Git\usr\bin;$env:PATH"
$env:HOST_TEST_SANITIZERS='0'
just check
```

The run included the requested host tests and source contracts:

```text
PASS initial_state_is_empty
PASS fifo_storage_is_uppercased
PASS full_buffer_overwrites_oldest_entry
PASS indices_wrap_without_changing_fifo_order
PASS reset_clears_data_and_busy_state
PASS pop_returns_entries_in_lifo_order
All AVR circular buffer characterization tests passed
PASS percent_characters_are_copied_as_text
PASS maximum_length_text_is_null_terminated
PASS oversized_text_is_rejected_without_partial_frame
PASS invalid_copy_arguments_are_rejected
All AVR bounded text copy tests passed
PASS last_payload_byte_fits_first_and_last_fields
PASS field_reserves_space_for_null_terminator
PASS append_rejects_message_id_and_excess_field_indices
PASS termination_stays_within_last_field
PASS fourth_field_is_rejected
PASS zero_capacity_is_rejected
All AVR Linkbus receive bounds tests passed
PASS g_fox declaration covers every Event_t value
PASS I2C failure count writes preserve the uint16_t EEPROM width
PASS RF power writes preserve the uint16_t EEPROM width
PASS generated driver include paths are host-portable
PASS WiFi smoke probe remains read-only
PASS text send helpers copy literal data within destination bounds
PASS Linkbus receive parser guards field count and field length
PASS Linkbus live test remains limited to malformed ZZZ frames and read-only recovery
PASS all 65 EEPROM field offsets match the 274-byte AVR layout
```

## Conclusion

The accumulated hardening snapshot builds warning-free in the Windows AVR reference environment, keeps the EEPROM layout at exactly 274 bytes, produces deterministic Windows artifacts, matches Mac's same-source HEX and EEP hashes, and passes the requested Windows host and source-contract checks.
