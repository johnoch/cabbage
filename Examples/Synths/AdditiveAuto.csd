<Cabbage>
form size(430, 440), caption("Automated Additive Synth")

groupbox bounds(260, 20, 150, 100), colour(160, 160, 160, 20), fontcolour("cornflowerblue"), text("Automation"), plant("auto"){
rslider bounds(.53, .3, .6), text("Freq"), channel("auto_freq"), fontcolour(colour(0, 255, 0, 170)), colour("cornflowerblue"), range(0, 5, .1)
checkbox bounds(.1, .35, .4, .2), channel("auto_on"),  text("Auto"), shape("square")
combobox bounds(.1, .6, .4, 0.2), channel("auto_wave"), items("Randi", "Randh", "Waves"), value(1)
}

groupbox bounds(20, 20, 240, 100), colour(160, 160, 160, 20), fontcolour("cornflowerblue"), text("Note Envelope"), plant("adsr"){ 
rslider bounds(.0, .3, .6, .6), text("Att."), fontcolour("cornflowerblue"), colour(0, 255, 0, 170), channel("att"), range(0.01, 1, .1)
rslider bounds(.25, .3, .6, .6), text("Dec."), fontcolour("cornflowerblue"),colour(0, 255, 0, 170), channel("dec"), range(0, 2, .25)
rslider bounds(.5, .3, .6, .6), text("Sus."), fontcolour("cornflowerblue"),colour(0, 255, 0, 170), channel("sus"), range(0, 2, 1.86)
rslider bounds(.75, .3, .6, .6), text("Rel."), fontcolour("cornflowerblue"),colour(0, 255, 0, 170), channel("rel"), range(0.01, 1, .20)
}

snapshot bounds(214, 122, 200, 20), preset("eq"), items("Sound1", "Sound2", "Sound3", "Sound4", "Sound5") 

image bounds(10, 10, 410, 335), colour(160, 160, 160, 20), preset("eq"), plant("filters"){
vslider bounds(.0, .4, .1, .58), text("P.1"), colour("cornflowerblue"), channel("partial1"), range(0, 1, 1), textbox(1)
vslider bounds(.1, .4, .1, .58), text("P.2"), channel("partial2"), range(0, 1, .5), textbox(1), colour("cornflowerblue")
vslider bounds(.2, .4, .1, .58), text("P.3"), channel("partial3"), range(0, 1, .25), textbox(1), colour("cornflowerblue")
vslider bounds(.3, .4, .1, .58), text("P.4"), channel("partial4"), range(0, 1, .17), textbox(1), colour("cornflowerblue")
vslider bounds(.4, .4, .1, .58), text("P.5"), channel("partial5"), range(0, 1, .08), textbox(1), colour("cornflowerblue")
vslider bounds(.5, .4, .1, .58), text("P.6"), channel("partial6"), range(0, 1, .04), textbox(1), colour("cornflowerblue")
vslider bounds(.6, .4, .1, .58), text("P.7"), channel("partial7"), range(0, 1, .02), textbox(1), colour("cornflowerblue")
vslider bounds(.7, .4, .1, .58), text("P.8"), channel("partial8"), range(0, 1, .01), textbox(1), colour("cornflowerblue")
vslider bounds(.8, .4, .1, .58), text("P.9"), channel("partial9"), range(0, 1, .005), textbox(1), colour("cornflowerblue")
vslider bounds(.9, .4, .1, .58), text("P.10"), channel("partial10"), range(0, 1, 0), textbox(1), colour("cornflowerblue")
}


keyboard bounds(15, 350, 400, 50)

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 -m0d --midi-key-cps=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

gaSigL init 0
gaSigR init 0

instr 1
kpan randi 1, .1, 2
kp1 chnget "partial1"
kp2 chnget "partial2"
kp3 chnget "partial3"
kp4 chnget "partial4"
kp5 chnget "partial5"
kp6 chnget "partial6"
kp7 chnget "partial7"
kp8 chnget "partial8"
kp9 chnget "partial9"
kp10 chnget "partial10"

iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"

kamp madsr iatt, idec, isus, irel

a1 oscili kamp*kp1, p4, 1
a2 oscili kamp*kp2, p4*2, 1
a3 oscili kamp*kp3, p4*3, 1
a4 oscili kamp*kp4, p4*4, 1
a5 oscili kamp*kp5, p4*5, 1
a6 oscili kamp*kp6, p4*6, 1
a7 oscili kamp*kp7, p4*7, 1
a8 oscili kamp*kp8, p4*8, 1
a9 oscili kamp*kp9, p4*9, 1
a10 oscili kamp*kp10, p4*10, 1

amix = (a1+a2+a3+a4+a5+a6+a7+a8+a9+a10)/10


outs amix*(abs(kpan)), amix*(1-abs(kpan))

;gaSigL = gaSigL + aL
;gaSigR = gaSigR + aR

endin

instr 100
kAutoOnOff chnget "auto_on"
kFreq chnget "auto_freq"
kWaveform chnget "auto_wave"
if(kAutoOnOff==1) then
	if(kWaveform==1) then
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial1"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial2"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial3"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial4"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial5"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial6"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial7"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial8"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial9"
		k1 randi 1, kFreq, 2
		chnset abs(k1), "partial10"
	elseif(kWaveform==2) then
		chnset abs(k1), "partial1"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial2"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial3"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial4"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial5"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial6"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial7"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial8"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial9"
		k1 randh 1, kFreq, 2
		chnset abs(k1), "partial10"
	elseif(kWaveform==3) then
		k1 oscil 1, kFreq, 2
		chnset abs(k1), "partial1"
		k1 oscil 1, kFreq, 2, 0.1
		chnset abs(k1), "partial2"
		k1 oscil 1, kFreq, 2, 0.2
		chnset abs(k1), "partial3"
		k1 oscil 1, kFreq, 2, 0.3
		chnset abs(k1), "partial4"
		k1 oscil 1, kFreq, 2, 0.4
		chnset abs(k1), "partial5"
		k1 oscil 1, kFreq, 2, 0.5
		chnset abs(k1), "partial6"
		k1 oscil 1, kFreq, 2, 0.6
		chnset abs(k1), "partial7"
		k1 oscil 1, kFreq, 2, 0.7
		chnset abs(k1), "partial8"
		k1 oscil 1, kFreq, 2, 0.8
		chnset abs(k1), "partial9"
		k1 oscil 1, kFreq, 2, 0.9
		chnset abs(k1), "partial10"
	endif
endif
endin

;master output
instr 1000
k1 active 1
outs gaSigL*(1/k1), gaSigR*(1/k1),
gaSigL = 0
gaSigR = 0
endin

</CsInstruments>
<CsScore>
f1 0 1024 10 1 .5 .25 .17 0 0 .001 .002
f2 0 1024 10 1
f0 3600
i100 0 3600
;i1000 0 3600
</CsScore>
</CsoundSynthesizer>