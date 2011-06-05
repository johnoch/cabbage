<Cabbage>
form caption("Filter"), size(360, 225), colour("white")

image plant("LOWPASS"), pos(0, 10), size(330, 125), colour("white"), outline("black"), line(4){
rslider channel("fco"), bounds(10, 10, 100, 100), caption("FCO"), min(0), max(10000), colour("black")
rslider channel("res"), bounds(115, 10, 100, 100), caption("Res"), min(0), max(1), colour("black")
rslider channel("dist"), bounds(220, 10, 100, 100), caption("Dist"), min(0), max(1), colour("black")
}

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

instr LOWPASS
ain inleta "in"
kfco chnget "fco"
kres chnget "fco"
kdist chnget "fco"
aout lpf18 ain, kfco, kres, kdist
outleta "lpout", aout
;outs aout, aout
endin


</CsInstruments>
<CsScore>
f1 0 1024 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>
