;The following instrument will retrieve the current BPM value
;from the host. Changing the value in the host will cause the metro
;to change.  
<Cabbage>
form caption("BPM Metro") size(280, 150)
hostbpm channel("bpm")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n 
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 1
nchnls = 2
0dbfs = 1

instr 1
k1 chnget "bpm"
kmetro metro k1/60
if(kmetro==1) then
event "i", 2, 0, 1
endif
endin

instr 2
k1 expon 1, p3, 0.001
a1 oscil k1, 440, 1
out a1, a1
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>