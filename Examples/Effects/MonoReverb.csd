<Cabbage>
form caption("Reverb") size(230, 130)
rslider channel("size"), caption("Reverb"), pos(5, 5), range(0, 1, 0.2)
rslider channel("fco"), caption("Cut-off"), pos(105, 5), range(0, 22000, 10000)
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

instr 1
kfdback chnget "size"
kfco chnget "fco"
ain inch 1
aoutL, aoutR reverbsc ain, ain, kfdback, kfco 
outs aoutL, aoutR
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>
