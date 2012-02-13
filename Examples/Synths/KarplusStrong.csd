<Cabbage>
form caption("Cabbage Karplus Wrong!"), size(550, 190)
keyboard bounds(0, 110, 530, 50)

groupbox text("Source and Filter"), bounds(0, 0, 120, 100), plant("source"){
rslider text("NoiseBW"), bounds(0, 15, 70, 70), colour("dodgerblue"), range(1, 22050, 6000), channel("noiseBW")
rslider text("Dist.."), bounds(50, 15, 70, 70), colour("dodgerblue"), range(1, 5, 1), channel("earlyDist")
}

groupbox text("Filter Cut-off Envelope"), bounds(130, 0, 220, 100), plant("filter-env"){
rslider bounds(0, 15, 70, 70), text("Att.."), range(0.2, 2, .5), channel("filterAttack")
rslider bounds(50, 15, 70, 70), text("Dec.."), range(0.01, 2, .1), channel("filterDecay")
rslider bounds(100, 15, 70, 70), text("Sus.."), range(0.01, 1, .6), channel("filterSustain")
rslider bounds(150, 15, 70, 70), text("Rel.."), range(0.01, 2, .5), channel("filterRelease")
}

groupbox text("Lowpass Filter"), bounds(360, 0, 170, 100), plant("lpfilter"){
rslider bounds(0, 15, 70, 70), text("Centre.."), range(0, 2000, 1000), range(1, 22050, 2000), channel("lowpassCF")
rslider bounds(50, 15, 70, 70), text("Res.."), range(0, 1, .1), channel("reosnance")
rslider bounds(100, 15, 70, 70), text("Dist.."), range(0, 5, .1), channel("distortion")
}


</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 --midi-key-cps=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 1
nchnls = 2
0dbfs = 1

giFT1 ftgen 0, 0, 4096, 10, 1

instr 1
kNoiseBW chnget "noiseBW"
kEarlyDist chnget "earlyDist"
kLPFcf chnget "lowpassCF" 
kRes chnget "resonance"
kDis chnget "distortion"

kFilterA chnget "filterAttack"
kFilterD chnget "filterDecay"
kFilterS chnget "filterSustain"
kFilterR chnget "filterRelease"

idel = 1/p4
alp init 0
aprev init 0
kcount line 0, 10, 10

if( kcount > idel) then 
	aexc = 0
else
	aexc randi p5, kNoiseBW
endif

adelay delay (aexc+alp)*kEarlyDist, idel
adelay clip adelay, 2, p5
alp  = (adelay + aprev)*0.5
aprev = adelay



kadsr madsr i(kFilterA), i(kFilterD), i(kFilterS), i(kFilterR)
alft lpf18 adelay, 1+(kLPFcf*kadsr), kRes, kDis

aadsr madsr i(kFilterA), i(kFilterD), i(kFilterS), i(kFilterR)
abal balance alft, aadsr*p5

afltClip clip abal, 2, p5

kDeClicker linenr 1, 0.1, i(kFilterR), 0.01 
outs afltClip*kDeClicker, afltClip*kDeClicker 

endin

</CsInstruments>
<CsScore>
f1 0 1024 10 1 0 .1 .5
f0 3600

</CsScore>
</CsoundSynthesizer>