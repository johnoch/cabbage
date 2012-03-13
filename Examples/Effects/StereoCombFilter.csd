<Cabbage>
form size(420, 130), caption("StereoComb")

groupbox text("Left"), bounds(0, 5, 140, 100), plant("left"){
rslider bounds(5, 15, 70, 70), text("Delay"), colour("white"), channel("combDelay1"), range(0, 10, .68)
rslider bounds(60, 15, 70, 70), text("Decay"), colour("white"), channel("combRev1"), range(0, 3, .37)
}

groupbox text("Right"), bounds(150, 5, 140, 100), plant("right"){
rslider bounds(5, 15, 70, 70), text("Delay"), colour("white"), channel("combDelay2"), range(0, 10, .68)
rslider bounds(60, 15, 70, 70), text("Decay"), colour("white"), channel("combRev2"), range(0, 3, .37)
}

rslider bounds(300, 0, 100, 100), text("Dry/Wet"), range(0, 1, 0), channel("drywet"), colour("dodgerblue")




</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2

;takes a mono input and outputs stero signal

instr StereoCombFilters
kdel1 chnget "combDelay1"
krev1 chnget "combRev1"
kdel2 chnget "combDelay2"
krev2 chnget "combRev2"
kMout chnget "main_out"
kMin chnget "main_in"
kOnOff chnget "on_off"


ain inch 1
ain=ain*kMin

aL vcomb ain, krev1, kdel1/5, 3
aR vcomb ain, krev2, kdel2/5, 3

outs (aL*kMout)*kOnOff, (aR*kMout)*kOnOff

endin

</CsInstruments>

<CsScore>
i1 0 1000
</CsScore>

</CsoundSynthesizer>