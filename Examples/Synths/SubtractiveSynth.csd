<Cabbage>
form caption("Cabbage Subtractive Synth") size(572, 265), colour("black")
rslider bounds(15, 10, 100, 100) channel("cf"), range(1, 5000, 500), caption("Centre Frequency"), colour("white"), midictrl(1, 1)
rslider bounds(120, 10, 100, 100) channel("res"), range(0, 1, 0.5), caption("Resonance"), colour("white")
rslider bounds(225, 10, 100, 100) channel("lfo_rate"), range(0, 10, 5), caption("LFO Rate"), colour("white")
rslider bounds(330, 10, 100, 100) channel("lfo_depth"), range(0, 1000, 10), caption("LFO Depth"), colour("white")
rslider bounds(435, 10, 100, 100) channel("disto"), range(0, 3, .5), caption("Dist."), colour("white")
keyboard pos(1, 140), size(550, 100)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 --midi-key-cps=4 --midi-velocity-amp=5 -m0d
</CsOptions>
<CsInstruments>
; Initialize the global variables. 
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1



instr 1
kcf chnget "cf"
kres chnget "res"
klforate chnget "lfo_rate"
klfodepth chnget "lfo_depth" 
kdist chnget "disto" 
kenv linenr 1, 0.1, 1, 0.01
asig vco2 p5, p4
klfo lfo klfodepth, klforate, 5
aflt lpf18 asig, kcf+klfo, kres, kdist
outs aflt*kenv, aflt*kenv
endin



</CsInstruments>
<CsScore>
f1 0 1024 10 1
f0 3600
</CsScore>
</CsoundSynthesizer>
