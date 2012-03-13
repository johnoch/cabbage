<Cabbage>
form caption("PVS Blur") size(330, 80)
hslider pos(30, 1), channel("blur"), range(0, 1, 0.5), caption("Blur time")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs=1


instr 1
kblurtime chnget "blur"
asig inch 1                               
fsig  pvsanal   asig, 1024, 256, 1024, 1 
ftps  pvsblur   fsig, kblurtime, 10         
atps  pvsynth   ftps  
apan jspline 1, 1, 3
outs atps*apan, atps*(1-apan)
endin
        
</CsInstruments>
<CsScore>
f1 0 1024 10 1
i1 0 3600
</CsScore>
</CsoundSynthesizer>     
