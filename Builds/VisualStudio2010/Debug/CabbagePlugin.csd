<Cabbage>
form size(500, 550)
csoundoutput pos(0, 0), text("Csound Output")

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 --midi-key-cps=4 
</CsOptions>
<CsInstruments>
nchnls = 2
0dbfs = 1

instr 1


endin

</CsInstruments>
<CsScore>
f1 0 8 -2 1 1 0 2 0 3 0 2		;tables using GEN02 to hold a list of 8 notes. 
f99 0 1024 10 1 1 1				;the shape of this composite waveform determines the filter cut-off 
f0 3600
</CsScore>
</CsoundSynthesizer>