<Cabbage>
form caption("Cabbage Karplus Wrong!"), size(550, 200), plugin("Cab0")
keyboard bounds(0, 110, 530, 50)

groupbox text("Source and Filter"), bounds(0, 0, 120, 100), plant("source"){
rslider text("NoiseBW"), bounds(.05, .3, .55), colour("dodgerblue"), range(1, 22050, 6000), channel("noiseBW")
vline bounds(.5, .3, .01, .5)
rslider text("Dist.."), bounds(.50, .3, .55), colour("dodgerblue"), range(1, 50, 1), channel("earlyDist")
}

groupbox bounds(130, 0, 220, 100), text("Filter Cut-off Envelope"), plant("filter-env"){
rslider bounds(.03, .32, .55), text("Att.."), colour("white"), channel("att"), range(0.01,3, .5)
rslider bounds(.27, .32, .55), text("Dec.."), colour("white"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .32, .55), text("Sus.."), colour("white"), channel("sus"), range(0,1,.8)
rslider bounds(.74, .32, .55), text("Rel.."), colour("white"), channel("rel"), range(0.01,3, .2)
image bounds(0.03, .20, .95, .7), colour(145, 145, 155, 50)
vline bounds(.27, .3, .01, .5)
vline bounds(.5, .3, .01, .5)
vline bounds(.74, .3, .01, .5)
}

groupbox text("Lowpass Filter"), bounds(360, 0, 170, 100), plant("lpfilter"){
rslider bounds(.05, .3, .55), colour("cornflowerblue"), text("Centre.."), range(0, 2000, 1000), range(1, 22050, 2000), channel("lowpassCF")
vline bounds(.35, .3, .01, .5)
rslider bounds(.35, .3, .55), colour("cornflowerblue"), text("Res.."), range(0, 1, .1), channel("reosnance")
vline bounds(.65, .3, .01, .5)
rslider bounds(.65, .3, .55), colour("cornflowerblue"), text("Dist.."), range(0, 5, .1), channel("distortion")
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

iFilterA chnget "att"
iFilterD chnget "dec"
iFilterS chnget "sus"
iFilterR chnget "rel"

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



kadsr madsr iFilterA, iFilterD, iFilterS, iFilterR
alft lpf18 adelay, 1+(kLPFcf*kadsr), kRes, kDis

aadsr madsr iFilterA, iFilterD, iFilterS, iFilterR
abal balance alft, aadsr*p5

afltClip clip abal, 2, p5

kDeClicker linenr 1, 0.1, iFilterR, 0.01 
outs afltClip*kDeClicker, afltClip*kDeClicker 

endin

</CsInstruments>
<CsScore>
f1 0 1024 10 1 0 .1 .5
f0 3600

</CsScore>
</CsoundSynthesizer>