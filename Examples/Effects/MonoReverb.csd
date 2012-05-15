<Cabbage>
form caption("Reverb") size(220, 145)
groupbox text("Stereo Reverb"), bounds(10, 10, 200, 100)
rslider channel("size"), bounds(10, 35, 70, 70), text("Size"), range(0, 2, 0.2)
rslider channel("fco"), bounds(80, 35, 70, 70), text("Cut-off"), range(0, 22000, 10000)
rslider channel("gain"), bounds(140, 35, 70, 70), text("Gain"), range(0, 1, 0.5)
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
kgain chnget "gain"
ain, ainR ins
aoutL, aoutR reverbsc ain, ain, kfdback, kfco 
outs aoutL*kgain, aoutR*kgain
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>
