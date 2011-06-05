<Cabbage>
form caption("Test"), size(560, 225), colour("black")
image plant("LFO1"), bounds(10, 10, 100, 120), colour("black"), outline("orange"), line(4){
rslider channel("LFOfreq"), bounds(10, 5, 80, 80), caption("LFO"), min(0), max(1000), colour("white")
combobox channel("LFOwave"), bounds(10, 90, 80, 20), items("Sin", "Tri", "Sqr Bi", "Sqr Uni", "Saw", "Saw Down"), value(1), colour("black"), textcolour("white")
}

image plant("LOWPASS"), pos(130, 10), size(330, 125), colour("black"), outline("orange"), line(4){
rslider channel("fco"), bounds(10, 10, 100, 100), caption("FCO"), min(0), max(10000), colour("white")
rslider channel("res"), bounds(115, 10, 100, 100), caption("Res"), min(0), max(1), colour("white")
rslider channel("dist"), bounds(220, 10, 100, 100), caption("Dist"), min(0), max(2), colour("white")
}
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

; Connect up the instruments to create a signal flow graph.
connect "LFO",   "LFOout",     "LOWPASS",     	"in"

alwayson "LOWPASS"
alwayson "LFO"

instr LFO
kfreq chnget "LFOfreq"
kwave chnget "LFOwave"
if(kwave==1) then
a1 lfo 1, kfreq, 0 ;itype = 0 - sine
elseif(kwave==2) then
a1 lfo 1, kfreq, 1 ;itype = 1 - triangles
elseif(kwave==3) then
a1 lfo 1, kfreq, 2 ;itype = 2 - square (bipolar)
elseif(kwave==4) then
a1 lfo 1, kfreq, 3 ;itype = 3 - square (unipolar)
elseif(kwave==5) then
a1 lfo 1, kfreq, 4 ;itype = 4 - saw-tooth
elseif(kwave==6) then
a1 lfo 1, kfreq, 5 ;itype = 5 - saw-tooth(down)
endif
outleta "LFOout", a1
;outs a1, a1
endin

instr LOWPASS
ain inleta "in"
kfco chnget "fco"
kres chnget "res"
kdist chnget "dist"
aout lpf18 ain, kfco, kres, kdist
outleta "lpout", aout
outs aout, aout
endin

</CsInstruments>
<CsScore>
f1 0 1024 10 1

</CsScore>
</CsoundSynthesizer>
