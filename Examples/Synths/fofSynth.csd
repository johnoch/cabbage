<Cabbage>
form caption("Cabbage Formant Synth"), size(520, 335), pluginID("fof1") 

xypad bounds(0, 0, 250, 210), channel("form1", "form2"), text("Formant Frequency  1/2"), rangex(50, 800, 100), rangey(600, 1200, 600)
  
groupbox bounds(260, 0, 240, 100), line(0), text("Formant Parameters One"), plant("formant1"){ 
hline bounds(0.1, .2, .8, .03)
rslider bounds(.03, .3, .6), text("Amp"), colour("brown"), channel("amp1"), range(0, .5, .5)
vline bounds(.27, .3, .01, .5)
rslider bounds(.27, .3, .6), text("BW"), colour("brown"), channel("bw1"), range(10, 120, 60)
vline bounds(.5, .3, .01, .5)
rslider bounds(.5, .3, .6), text("Rise"), colour("brown"), channel("rise1"), range(0.001, 0.01, 0.001, 1, 0.001)
vline bounds(.73, .3, .01, .5)
rslider bounds(.73, .3, .6), text("Decay"), colour("brown"), channel("dec1"), range(0.001, 0.01, 0.001, 1, 0.001)
image bounds(.03, .02, .95, .95), colour(200, 200, 255, 50)
}

groupbox bounds(260, 105, 240, 100), line(0), text("Formant Parameters Two"), plant("formant2"){ 
hline bounds(0.1, .2, .8, .03)
rslider bounds(.03, .3, .6), text("Amp"), colour("brown"), channel("amp2"), range(0, .5, .5)
vline bounds(.27, .3, .01, .5)
rslider bounds(.27, .3, .6), text("BW"), colour("brown"), channel("bw2"), range(10, 120, 60)
vline bounds(.5, .3, .01, .5)
rslider bounds(.5, .3, .6), text("Rise"), colour("brown"), channel("rise2"), range(0.001, 0.01, 0.001, 1, 0.001)
vline bounds(.73, .3, .01, .5)
rslider bounds(.73, .3, .6), text("Decay"), colour("brown"), channel("dec2"), range(0.001, 0.01, 0.001, 1, 0.001)
image bounds(.03, .02, .95, .95), colour(200, 200, 255, 50)
}

keyboard bounds(0, 220, 500, 70)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -m0d -+rtmidi=NULL -M0 --midi-key-cps=4  --midi-velocity-amp=5 
</CsOptions>
<CsInstruments>
sr = 44100 
ksmps = 32 
nchnls = 2 
0dbfs  = 1 

;Author: Rory Walsh (2012)

instr 1
aEnv madsr 0.01, p5*.5, .6, .1	;master envelope

kFunkItUp madsr 0.01, .2, .4, .4

idur   = 100			;<<--- hack attack!
kform1 chnget "form1"		;formant frequency
kbw1 chnget "bw1"	;bandwidth
krise1 chnget "rise1"	;longer the rise - the narrower the skirtwidth. 
kdec1 chnget "dec1"	;longer the decay - sharper the resonance peak
kdur1 = krise1+kdec1
kamp1 chnget "amp1"		

kform2 chnget "form2"	;formant frequency
kbw2 chnget "bw2"	;bandwidth
krise2 chnget "rise2"	;longer the rise - the narrower the skirtwidth. 
kdec2 chnget "dec2"	;longer the decay - sharper the resonance peak
kdur2 = krise2+kdec2
kamp2 chnget "amp2"

ifq    = p4/6	;only so I can play bass notes with the kyboard while changing params

kvibrato = 1;randh   100, 10;, 1	;add a lttle vibtrato 
 
;two formants  
a1 fof      kamp1, ifq+kvibrato,  kform1*kFunkItUp, 0, kbw1, krise1, kdur1, kdec1, 20, 1,2, idur
a2 fof      kamp2, ifq+kvibrato,  kform2*kFunkItUp, 0, kbw2, krise2, kdur2, kdec2, 20, 1,2, idur

amix = a1 + a2 

outs     amix*aEnv, amix*aEnv
endin


</CsInstruments>
<CsScore> 
f1 0 4096 10 1 0 [1/3] 0 [1/7]
;f2 0 1024 19 0.5 0.5 270 0.5
f2 0 1024 7 0 1024 1
f0 3600
</CsScore>
</CsoundSynthesizer>