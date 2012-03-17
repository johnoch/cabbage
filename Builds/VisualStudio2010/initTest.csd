<Cabbage>
form size(410, 180)
checkbox channel("checkbox"), pos(10, 20), size(100, 70), value(0)
button channel("button"), pos(120, 20), size(100, 70), text("On","Off"), value(1)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>  
<CsInstruments> 
nchnls = 2

instr 1

kcheckbox	chnget	"checkbox"
kbutton		chnget	"button"
printks "checkbox: %d  button: %d%n", 0.5 , kcheckbox, kbutton

endin

</CsInstruments>

<CsScore>
i1 0 300
</CsScore>

</CsoundSynthesizer>