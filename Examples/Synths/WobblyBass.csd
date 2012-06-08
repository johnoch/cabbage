<Cabbage>
form size(480, 310), caption("Presets"), pluginID("plu1")

image bounds(2, 2, 475, 275), colour(100, 100, 100, 100)

image bounds(190, 115, 280, 30), colour(164, 164, 164, 110), line(0)
snapshot bounds(200, 120, 265, 20), items("Mellow", "Thrash 1", "Thrash 2", "Murky", "Overtone", "Overdrive"), value(0), preset("pre")
;line bounds(190, 150, 280, 3), colour("black")

groupbox bounds(12, 10, 170, 100), text("Presence -k-rate-"),  preset("pre"), plant("presence"){ 
rslider bounds(.01, .3, .6, .6), text("Depth"), colour(100, 149, 237, 170), channel("depth"), range(0, 10, 0.2)
rslider bounds(.32, .3, .6, .6), text("Rate"), colour(100, 149, 237, 170), channel("rate"), range(0, 10, 0.2)
rslider bounds(.64, .3, .6, .6), text("Diff"), colour(100, 149, 237, 170), channel("diff"), range(0, 1, .8)
}

groupbox bounds(190, 10, 280, 100), text("Filter Envelope -i-rate-"), preset("pre"), plant("adsr"){ 
rslider bounds(.0, .3, .6, .6), text("Att."), colour(100, 149, 237, 170), channel("att"), range(0.01,3, .25)
rslider bounds(.2, .3, .6, .6), text("Dec."), colour(100, 149, 237, 170), channel("dec"), range(0,1, .15)
rslider bounds(.4, .3, .6, .6), text("Sus."), colour(100, 149, 237, 170), channel("sus"), range(0,1,.12)
rslider bounds(.59, .3, .6, .6), text("Rel."), colour(100, 149, 237, 170), channel("rel"), range(0.01,3, .20)
rslider bounds(.78, .3, .6, .6), text("Freq."), colour(100, 149, 237, 170), channel("freq"), range(0.01,22050, 10000)
}

groupbox bounds(12, 115, 170, 100), text("Output Filter -k-rate-"),  preset("pre"), plant("filter"){ 
rslider bounds(.01, .3, .6, .6), text("Freq"), colour(100, 149, 237, 100), channel("filtFreq"), range(0, 22050, 10000)
rslider bounds(.32, .3, .6, .6), text("Res"), colour(100, 149, 237, 100), channel("filtRes"), range(0, 1, 0)
rslider bounds(.64, .3, .6, .6), text("Dist"), colour(100, 149, 237, 100), channel("filtDist"), range(0, 4, 0)
}

image bounds(190, 150, 280, 60), colour(64, 64, 64, 164), line(1)
hslider bounds(195, 150, 260, 30), text("LFO Freq"), channel("lfoFreq"), range(0, 3, 0), textbox(1)
combobox bounds(270, 183, 105, 20), channel("waveform"), items("Sine wave", "Expon", "Worble"), value(1)
checkbox bounds(380, 183, 90, 20), channel("lfoGo"), text("LFO Active"), shape("square"), colour("yellow") 
label bounds(195, 186, 80, 13), text("Waveshape") 

keyboard bounds(10, 220, 460, 50)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-n -d -+rtmidi=NULL -M0 -m0d --midi-key-cps=4 --midi-velocity-amp=5
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs=1

ga1	init	0
ga2	init	0

instr 1
iatt	chnget	"att"
idec	chnget	"dec"
isus	chnget	"sus"
irel	chnget	"rel"
iFreq   chnget "freq"
iamp = p5/2
kenv madsr 0.01, .5, .8, irel
kfilterEnv madsr iatt, idec, isus, irel
a1 vco2 iamp*kenv, p4, 10
a2 vco2 iamp*kenv/2, p4*2.01, 10
aflt lpf18 (a1+a2), 1+(iFreq*kfilterEnv), .8, 4
;outs aflt, aflt
ga1 = aflt
ga2 = aflt
endin

instr	10
kdepth chnget "depth"
krate chnget "rate"
kdiff chnget "diff"
kdirt chnget "dirt"

asigr = ga1
asigl = ga2
;Based on UDO by Giorgio Zucco (2012)
k1ch  randi       kdepth/2,krate,1
ar1   vdelay asigr,kdepth/2+k1ch,10
k2ch  randi       kdepth/2,krate*0.9,.2
ar2   vdelay asigr,kdepth/2+k2ch,10
k3ch  randi       kdepth/2,krate*1.1,.2
ar3   vdelay asigr,kdepth/2+k3ch,10
k4ch  randi       kdepth/2,krate*1.3,.1
ar4   vdelay asigr,kdepth/2+k4ch,10

kdepthl = kdepth*kdiff
krate1 = krate*kdiff

k1chl  randi       kdepthl/2,krate1,1
ar1l   vdelay asigl,kdepthl/2+k1chl,10
k2chl  randi       kdepthl/2,krate1*0.9,.2
ar2l   vdelay asigl,kdepthl/2+k2chl,10
k3chl  randi       kdepthl/2,krate1*1.1,.2
ar3l   vdelay asigl,kdepthl/2+k3chl,10
k4chl  randi       kdepthl/2,krate1*1.3,.1
ar4l   vdelay asigl,kdepthl/2+k4chl,10


aL   =    (asigr+ar1+ar2+ar3+ar4)/2
aR  =    (asigl+ar1l+ar2l+ar3l+ar4l)/2

ga1 = aL
ga2 = aR
;outs aL, aR
endin

instr 20
kFiltFreq chnget "filtFreq"
kFiltRes chnget "filtRes"
kFiltDist chnget "filtDist"
afltL lpf18 ga1, kFiltFreq, kFiltRes, kFiltDist
afltR lpf18 ga2, kFiltFreq, kFiltRes, kFiltDist
abalL balance afltL, ga1
abalR balance afltR, ga2
outs abalL, abalR
ga1 = 0
ga2 = 0
endin

;automation
instr 30
kNoteFilterFreq   chnget "freq"
kLFOFreq chnget "lfoFreq"
kLFOOn chnget "lfoGo"
kWave chnget "waveform"

if(kLFOOn==1) then
kenv oscilikt kNoteFilterFreq, kLFOFreq, kWave
chnset abs(kenv)+500, "filtFreq"
endif

endin

</CsInstruments>  
<CsScore>
f1 0 1024 10 1
f2 0 1024 10 5 0.01 10 1 1014 0.001
f3 0 1024 10 1 0 .5 .3 0 .01
f0 3600
i10 0 3600
i20 0 3600
i30 0 3600
</CsScore>
</CsoundSynthesizer>