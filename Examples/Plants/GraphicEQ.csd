<Cabbage>

PLANT 
form caption("Graphic EQ") size(400, 265), colour("black")
image plant("GEQ"), pos(0, 20), size(350, 200), colour("black"), shape("rounded"), outline("orange"), line(4){
vslider pos(50, 40), size(40, 145), min(-6), max(6), value(0), colour("cornflowerblue"), caption("")
vslider pos(95, 40), size(40, 145), min(-6), max(6), value(0), colour("cornflowerblue"), caption("")
vslider pos(140, 40), size(40, 145), min(-6), max(6), value(0), colour("cornflowerblue"), caption("")
vslider pos(185, 40), size(40, 145), min(-6), max(6), value(0), colour("cornflowerblue"), caption("")
vslider pos(230, 402), size(40, 145), min(-6), max(6), value(0), colour("cornflowerblue"), caption("")
vslider pos(275, 40), size(40, 145), min(-6), max(6), value(0), colour("cornflowerblue"), caption("")
image pos(45, 40), size(280, 60), file("eq.png")
image pos(45, 104), size(280, 60), file("eq.png")
label pos(5, 150), caption("-6db"), colour("white")
label pos(5, 90), caption("-0db"), colour("white")
label pos(5, 30), caption("+6db"), colour("white")
label pos(45, 10), caption("100Hz"), colour("white")
label pos(92, 10), caption("200Hz"), colour("white")
label pos(140, 10), caption("400Hz"), colour("white")
label pos(190, 10), caption("800Hz"), colour("white")
label pos(236, 10), caption("2kHz"), colour("white")
label pos(274, 10), caption("5kHz"), colour("white")
}



</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 --midi-key-cps=4 --midi-velocity-amp=5
;-+rtaudio=alsa -odac
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2


massign 0, 1

instr 1
kcf chnget "cf"
kres chnget "res"
klforate chnget "lfo_rate"
klfodepth chnget "lfo_depth"
 
asig vco p5, p4, 1
klfo lfo klfodepth, klforate, 5
aflt moogladder asig, kcf+klfo, kres
outs aflt, aflt
endin



</CsInstruments>
<CsScore>
f1 0 1024 10 1
f0 3600

</CsScore>
</CsoundSynthesizer>
