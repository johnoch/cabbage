<Cabbage>
form caption("Sub-Sandwich") size(465, 300), colour("black")
image bounds(0, 0, 445, 185), colour(250, 150, 150, 30)
rslider bounds(35, 10, 70), channel("cf"), range(1, 5000, 500), caption("Centre Frequency"), colour("white")
rslider bounds(110, 10, 70), channel("res"), range(0, 1, 0.5), caption("Resonance"), colour("white")
rslider bounds(185, 10, 70), channel("lfo_rate"), range(0, 10, 5), caption("LFO Rate"), colour("white")
rslider bounds(260, 10, 70), channel("lfo_depth"), range(0, 10000, 10), caption("LFO Depth"), colour("white")
rslider bounds(335, 10, 70), channel("disto"), range(0, 3, .5), caption("Dist."), colour("white")

groupbox bounds(223, 85, 210, 90), text("Amplitude Envelope"), plant("filter-env"){
rslider bounds(.03, .32, .55), text("Att.."), colour("red"), channel("att"), range(.01,3, .5)
rslider bounds(.27, .32, .55), text("Att.."), colour("red"), channel("dec"), range(0,1, .5)
rslider bounds(.5, .32, .55), text("Att.."), colour("red"), channel("sus"), range(0,1,.8)
rslider bounds(.74, .32, .55), text("Att.."), colour("red"), channel("rel"), range(0.01,3, .2)
;vline bounds(.27, .3, .01, .5)
;vline bounds(.5, .3, .01, .5)
;vline bounds(.74, .3, .01, .5)
}

groupbox bounds(10, 85, 210, 90), text("Delay"), plant("stero_delay"){
rslider bounds(.03, .32, .55), text("Rev."), colour("orange"), channel("revtime"), range(0, 10, 1)
rslider bounds(.27, .32, .55), text("LoopL"), colour("orange"), channel("looptimeL"), range(0,3, .5)
rslider bounds(.5, .32, .55), text("LoopR"), colour("orange"), channel("looptimeR"), range(0,3, .5)
rslider bounds(.74, .32, .55), text("Gain"), colour("orange"), channel("gain"), range(0, 1, 0)
vline bounds(.27, .3, .01, .5)
vline bounds(.5, .3, .01, .5)
vline bounds(.74, .3, .01, .5)
}
keyboard bounds(0, 190, 440, 70)

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



opcode AudioTrigger, a, a
aSig xin
setksmps(1)
kSig downsamp aSig
kgo changed kSig
if(kgo==1) then
	if(kSig==int(1)) then
	event "i", 100, 0, .1
	elseif(kSig==int(2)) then
	event "i", 200, 0, .1
	endif
endif
xout aSig
endop

instr 1
ain inch 1
a1 AudioTrigger ain
endin


instr 100
kcf chnget "cf"
kres chnget "res"
klforate chnget "lfo_rate"
klfodepth chnget "lfo_depth" 
kdist chnget "disto" 
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"

kadsr	mxadsr	iatt,idec,isus,irel

asig vco2 p5, p4
klfo lfo klfodepth, klforate, 5
aflt lpf18 asig, kcf+klfo, kres, kdist
outs aflt*kadsr, aflt*kadsr
chnset aflt*kadsr, "signal"
endin

instr 1000
krev chnget "revtime"
kloopL chnget "looptimeL"
kloopR chnget "looptimeR"
kgain chnget "gain"

asig chnget "signal"

aL vcomb asig, krev, kloopL, 10
aR vcomb asig, krev, kloopR, 10
outs aL*kgain, aR*kgain
endin



</CsInstruments>
<CsScore>
f1 0 1024 10 1
;f0 3600
i1 0 1000
i1000 0 3600
</CsScore>
</CsoundSynthesizer>
