;This example shows how xypads work. xypads output data on two channels, in this case a channel named 
;"index" and another channel named "carrier". Automation can be turned on by pressing the on button and 
;"throwing" the ball. The ball's speed can also be sped up or slowed down using the slider.  
<Cabbage>
form caption("Csound Output") size(460, 300)
csoundoutput bounds(10, 10, 420, 260), text("Csound Output")
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

instr 1
printks "\nCsound messages are useful when it comes to de-slugging instruments(debugging in Cabbage is called de-slugging!)\n", 1 
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>