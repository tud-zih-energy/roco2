# P9 Highlow
Helper to create aliasing inside P9 OCC.

Switches between 50% busy wait and 50% sleep per cycle.

## Frequencies
"1 Hz" denotes one full cycle of 500ms busywait and 500ms sleep per second.
To set frequencies compile with `P9_HIGHLOW_FREQS` set to a frequency list as follows:

any of those separated by commas:
- direct frequency (`0.2`)
- range (`78-80`, only integers)
- range with plusminus (`2000+-5`, only integers)

spaces are ignored
