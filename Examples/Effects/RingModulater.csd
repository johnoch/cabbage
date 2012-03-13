<Cabbage>
form size(300, 350), caption("Ring Mod")
hslider pos(10, 0), channel("range"), caption("Hz Range"), colour("lightgrey"), range(0, 1000, 500)
xypad bounds(10, 60, 250, 250), channel("x", "y"), text("X:Freq / Y:Amp"), rangex(0, 1, .5), rangey(0, 1, .5)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

instr 1
k1 chnget "x"
k1 tonek k1, 10
k2 chnget "y"
k2 tonek k2, 10

krange chnget "range"
ain inch 1

aosc oscili k2, k1*krange, 1
apan jspline 1, 1, 5
outs apan*(ain*aosc), (1-apan)*(ain*aosc)

endin

</CsInstruments>
<CsScore>
f1 0 1024 10 1 
i1 0 3600
</CsScore>
</CsoundSynthesizer>