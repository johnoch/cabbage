<Cabbage>
form size(700, 600), caption("Untitled"), pluginID("plu1")
keyboard bounds(10, 10, 360, 100)
csoundoutput bounds(10, 150, 600, 400)
rslider bounds(10, 500, 60, 60), range(1, 100), channel("test")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=null 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

instr 10
endin

</CsInstruments>  
<CsScore>
f1 0 1024 10 1
i10 0 1000
</CsScore>
</CsoundSynthesizer>
