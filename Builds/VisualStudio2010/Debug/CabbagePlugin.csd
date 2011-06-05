<Cabbage>
form caption("Host info"), size(200, 100), colour("white")
hostbpm channel("bpm")
hosttime channel("time")
hostppqpos channel("ppq")
hostrecording channel("recording")
hostplaying channel("playing")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1


opcode dataRW, k, Ski  
Sname, kvalue, iRW      xin
kout init 0
setksmps 1
       if (iRW==0) then 
              fprintks Sname, "%2.1f\n", kvalue 
       else 
              kres readk Sname, 8, 0              
              kout = kres        
       endif
       xout kout                  
endop

instr 1
ktime chnget "time"
kbpm chnget "bpm"
kplay chnget "playing"
kmet metro kbpm/60
if(kmet==1) then
event "i", 100, 0, 1, 200
endif
endin

instr 100
aenv expon 1, p3, 0.001
a1 oscil aenv, p4, 1
outs a1, a1
endin


</CsInstruments>
<CsScore>
f1 0 1024 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>
