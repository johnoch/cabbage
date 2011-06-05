<Cabbage>
form caption("LFO Envelopes") size(360, 225), colour("black")
image plant("LFO1"), bounds(10, 10, 100, 120), colour("black"), outline("orange"), line(4){
rslider channel("LFOfreq"), bounds(10, 5, 80, 80), caption("LFO1"), min(0), max(1000), colour("white")
combobox channel("LFOwave"), bounds(10, 90, 80, 20), items("Sin", "Tri", "Sqr Bi", "Sqr Uni", "Saw", "Saw Down"), value(1), colour("black"), textcolour("white")
}

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

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
outs a1, a1
endin




</CsInstruments>
<CsScore>
f1 0 1024 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>