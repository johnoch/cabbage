;The image widget can be used to display an image or a shape
;labels can be used to display static text
;groupboxes can be used as layout containers for other widgets
<Cabbage>
form caption("Img") size(180, 200)
image bounds(10, 10, 140, 160), colour("black"), outline("white"), shape("sharp")
groupbox text("Cabbage"), bounds(20, 20, 120, 120)
image bounds(30, 30, 100, 100), file("cabbage.png")
label caption("Yummy......."), bounds(30, 140, 100, 20)
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

endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>