;This simple example illustrates the use of buttons and groupboxes. Buttons and checkboxes 
;always alternate between 0 and 1. If you pass the same caption for on and off the button's 
;text will not change but the values will still jump from 0 to 1 on each press. If you want buttons 
;to trigger something on each press they are best used in conjunction with the changed opcode
<Cabbage>
form caption("Buttons") size(260, 250)
groupbox text("Buttons!"), bounds(10, 10, 210, 180)
button channel("button"), pos(30, 40), text("Play note"), size(160, 80)
checkbox channel("checkbox"), text("Mute"), bounds(40, 130, 160, 30), value(0)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2

instr 1
;each time the user hits the button
;k1 will change values, alternating
;between 1 and 0. Each change will
;cause the changed opcode to output
;a trigger value of 1 
k1 chnget "button"
ktrig changed k1
if(ktrig==1) then
kfreq rand 1000, 10
event "i", 2, 0, 10, kfreq
endif
endin

instr 2
kmute chnget "checkbox"
kenv expon 10000, p3, 0.001
a1 oscil kenv, p4, 1
;checkboxes send 1 when checked and 0 when unchecked
outs a1*(1-kmute), a1*(1-kmute)
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>
<MacOptions>
Version: 3
Render: Real
Ask: Yes
Functions: ioObject
Listing: Window
WindowBounds: 72 179 400 200
CurrentView: io
IOViewEdit: On
Options:
</MacOptions>

<MacGUI>
ioView nobackground {59367, 11822, 65535}
ioSlider {5, 5} {20, 100} 0.000000 1.000000 0.000000 slider1
</MacGUI>

<EventPanel name="" tempo="60.00000000" loop="8.00000000" x="0" y="0" width="596" height="322"> 








</EventPanel>