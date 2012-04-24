;Comboboxes can be used to hold a list of items. Each item when clicked will send a corresponding value to a Csound channel.
; The first item is always 1, the second 2, and so forth.
<Cabbage>
form caption("Combo") size(280, 120)
combobox channel("combobox"), caption("Combo"), bounds(0, 5, 260, 80), value(1), items("50Hz Sine Wave"", 100Hz Sine Wave", "300Hz Sine Wave", "450Hz Sine Wave", "600Hz Sine Wave"), 
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
k1 chnget "combobox"

;check the value of the combobox and do something.. 
if(k1==1) then
k1 = 50
elseif(k1==2) then
k1 = 100
elseif(k1==3) then
k1 = 300
elseif(k1==4) then
k1 = 450
elseif(k1==5) then
k1 = 600
endif

ain oscil 1, k1, 1
outs ain, ain
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>