<Cabbage>
form caption("Gain") size(200, 150)
rslider channel("gain"), pos(30, 5), caption("Gain"), range(0, 1, 0.5)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n 
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 1
nchnls = 2

instr 1
k1 chnget "gain"
a1 inch 1
outs a1*k1, a1*k1
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>