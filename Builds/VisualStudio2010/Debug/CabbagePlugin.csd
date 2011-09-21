<Cabbage>
form caption("PVS Blur"), size(450, 80), colour("black")
hslider pos(1, 1), size(430, 50) channel("blur"), min(0), max(1), value(0), caption("Blur time"), colour("white")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2

alpass

instr 1
kblurtime chnget "blur"
asig init 0;inch 1                                
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
