;This example shows the various types of sliders available
<Cabbage>
form caption("Sliders") size(280, 450)
rslider channel("rslider1"), pos(10, 5), caption("Rotary"), range(0, 1000, 0)
rslider channel("rslider2"), bounds(150, 15, 60, 60), range(0, 1000, 0) , colour("red")

hslider channel("hslider1"), bounds(0, 120, 260, 60),  caption("Horizontal"), range(0, 1000, 0)
hslider channel("hslider2"), pos(10, 180), range(0, 1000, 0), colour("yellow")

vslider channel("hslider1"), bounds(30, 240, 70, 180), caption("Vertical"), range(0, 1000, 0)
vslider channel("hslider2"), pos(160, 240), range(0, 1000, 0), colour("black")
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
kf1 chnget "rslider1"
a1 oscil 5000, kf1, 1

kf2 chnget "rslider2"
a2 oscil 5000, kf2, 1

kf3 chnget "hslider1"
a3 oscil 5000, kf3, 1

kf4 chnget "hslider2"
a4 oscil 5000, kf4, 1

kf5 chnget "vslider1"
a5 oscil 5000, kf5, 1

kf6 chnget "vslider2"
a6 oscil 5000, kf6, 1

amix = a1+a2+a3+a4+a5+a6

;outs amix, amix
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>
