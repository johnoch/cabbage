<Cabbage>
form caption("VU Meters") size(460, 300)
vumeter  bounds(0, 0, 100, 200), config(1, 1), channel("left", "right")
vumeter  bounds(150, 0, 100, 200), config(1, 1), channel("mono1", "mono2")

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1


instr 1
a1 randi 1, 10, 2
a2 randi 1, 4, 2
a3 randi 1, 6, 2
a4 randi 1, 1, 2
k1 rms a1
k2 rms a2
k3 rms a3
k4 rms a4
chnset k1, "left"
chnset k2, "right"
chnset k3, "mono1"
chnset k4, "mono2"

endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>