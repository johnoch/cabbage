;This example shows how xypads work. xypads output data on two channels, in this case a channel named 
;"index" and another channel named "carrier". Automation can be turned on by pressing the on button and 
;"throwing" the ball. The ball's speed can also be sped up or slowed down using the slider.  
<Cabbage>
form caption("XYPad") size(280, 280)
xypad bounds(10, 10, 250, 250), channel("index", "carrier"), text("X:Index / Y:Carrier"), rangex(0, 5, 2), rangey(0, 500, 100)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -m0d
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

instr 1
kX chnget "index"	; get x-axis values
kX = int(kX)		; truncate the numbers to ints
kTrig changed kX	; see kX has changed value
if(kTrig==1) then	; if so, trigger note in instr 2 
	event "i", 2, 0, .5
endif
endin

instr 2
kindex chnget "index"
kcar chnget "carrier"
k1 expon .8, p3, 0.001 
;check the value of the combobox and do something.. 
kmod oscili kindex*(kcar/2), kcar/2, 1
aout oscili 1, kcar+kmod, 1  
outs aout*k1, aout*k1
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>