<Cabbage>
form size(220, 110), caption("SimpleDelay")
image bounds(0, 0, 200, 90), colour("black"), outline("white")
rslider bounds(10, 5, 80, 80), channel("delay"), caption("Delay"), colour("lightgrey") range(0, 10, 1)
checkbox pos(100, 15) text("On/Off"), channel("on_off")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n  
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2


instr 1
kDelay chnget "delay"
kMin chnget "main_in"
kOnOff chnget "on_off"
ainL inch 1
adelay vdelay ainL, kDelay*1000, 10000 
outs (adelay+ainL)*kOnOff, (adelay+ainL)*kOnOff
endin




</CsInstruments>
<CsScore>
f1 0 1024 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>