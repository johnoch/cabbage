<Cabbage>
form caption("Drone Labs Emulator"), size(570, 660), colour("black")
keyboard bounds(10, 550, 530, 80)


rslider caption("Trem Freq"), bounds(450, 10, 90, 90), colour("white"), channel("tremspeed")
rslider caption("Master"), bounds(450, 350, 90, 70), channel("mastervol"), range(0, 1, 1)

hslider caption("Number of harmonics per oscillator"), textbox(1), bounds(10, 430, 530, 60), channel("harms"), range(1, 30, 1)
hslider caption("Harmonic attenuation"), bounds(10, 480, 530, 60), channel("atten"), range(0, 1, 0), textbox(1)

groupbox text("Tremelo A"), plant("trem1"), bounds(10, 10, 100, 150){
checkbox text("On/Off"), bounds(20, 25, 70, 20), channel("onoff1")
checkbox text("Invert"), bounds(20, 45, 70, 20), channel("invert1")
rslider bounds(10, 55, 80, 80), text("Division"), channel("tremdiv1"), colour("brown"), range(0, 1, 0)
}

groupbox text("Tremelo B"), plant("trem2"), bounds(120, 10, 100, 150){
checkbox text("On/Off"), bounds(20, 25, 70, 20), channel("onoff2")
checkbox text("Invert"), bounds(20, 45, 70, 20), channel("invert2")
rslider bounds(10, 55, 80, 80), text("Division"), channel("tremdiv2"), colour("orange"), range(0, 1, 0)
}

groupbox text("Tremelo C"), plant("trem3"), bounds(230, 10, 100, 150){
checkbox text("On/Off"), bounds(20, 25, 70, 20), channel("onoff3")
checkbox text("Invert"), bounds(20, 45, 70, 20), channel("invert3")
rslider bounds(10, 55, 80, 80), text("Division"), channel("tremdiv3"), colour("cadetblue"), range(0, 1, 0)
}

groupbox text("Tremelo D"), plant("trem4"), bounds(340, 10, 100, 150){
checkbox text("On/Off"), bounds(20, 25, 70, 20), channel("onoff4")
checkbox text("Invert"), bounds(20, 45, 70, 20), channel("invert4")
rslider bounds(10, 55, 80, 80), text("Division"), channel("tremdiv4"), colour("chocolate"), range(0, 1, 0)
}

groupbox text("Osc. A"), plant("OSC1"), bounds(10, 170, 100, 250), colour("white"){
rslider bounds(10, 15, 80, 80), text("Coarse"), channel("coarse1"), colour("brown"), range(0, 1000, 0) 
rslider bounds(10, 90, 80, 80), text("Fine"), channel("fine1"), colour("brown"), range(0, 100, 0)
rslider bounds(10, 165, 80, 80), text("Volume"), channel("volume1"), colour("brown"), range(0, 1, 0)
}
groupbox text("Osc. B"), plant("OSC2"), bounds(120, 170, 100, 250), colour("white"){
rslider bounds(10, 15, 80, 80), text("Coarse"), channel("coarse2"), colour("orange"), range(0, 1000, 0)
rslider bounds(10, 90, 80, 80), text("Fine"), channel("fine2"), colour("orange"), range(0, 100, 0)
rslider bounds(10, 165, 80, 80), text("Volume"), channel("volume2"), colour("orange"), range(0, 1, 0)
}
groupbox text("Osc. C"), plant("OSC3"), bounds(230, 170, 100, 250), colour("white"){
rslider bounds(10, 15, 80, 80), text("Coarse"), channel("coarse3"), colour("cadetblue"), range(0, 1000, 0)
rslider bounds(10, 90, 80, 80), text("Fine"), channel("fine3"), colour("cadetblue"), range(0, 100, 0)
rslider bounds(10, 165, 80, 80), text("Volume"), channel("volume3"), colour("cadetblue"), range(0, 1, 0)
}
groupbox text("Osc. D"), plant("OSC4"), bounds(340, 170, 100, 250 ), colour("white"){
rslider bounds(10, 15, 80, 80), text("Coarse"), channel("coarse4"), colour("chocolate"), range(0, 1000, 0)
rslider bounds(10, 90, 80, 80), text("Fine"), channel("fine4"), colour("chocolate"), range(0, 100, 0)
rslider bounds(10, 165, 80, 80), text("Volume"), channel("volume4"), colour("chocolate"), range(0, 1, 0)
}


groupbox text("Low-pass"), plant("filter"), bounds(450, 110, 90, 230), colour("white"), line(4), outline("orange"){
rslider bounds(5, 15, 80, 70), text("Cut-off"), channel("cutoff"), colour("black"), range(1, 5000, 1500)
rslider bounds(5, 80, 80, 70), text("Rez."), channel("rez"), colour("black"), range(0, 1, 0)
rslider bounds(5, 145, 80, 70), text("Dist."), channel("dist"), colour("black"), range(0, 10, 0)
}


</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 --midi-key-cps=4 --midi-velocity-amp=5 
</CsOptions>
<CsInstruments>
; DRONELAB V2
; -------------
; Csound implementation by Patrick McGlynn
; Based on a design from www.casperelectronics.com
;
; 4 oscillator noise instrument, with tremolo, low-pass filter,
; distortion, band-pass filter and mic input
;
; Updated and modified for Cabbage by Rory Walsh
;

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1


instr 1
kMasterEnv linenr p5, .1, 1, 0.001

knh chnget "harms"
katten chnget "atten"

kTremSpeed chnget "tremspeed"
kAtremInvert chnget "invert1"
kAtremSwitch chnget "onoff1"
kAcoarse chnget "coarse1"
kAvol chnget "volume1"
kAfine chnget "fine1"
kAtremDivision chnget "tremdiv1"
ktremA	oscili	0.4, kTremSpeed/((kAtremInvert+1)*kAtremDivision), 1	
;aoscA	vco2	kAvol*((0.50 + ktremA*kAtremSwitch)*0dbfs), kAcoarse+kAfine, 0
aoscA gbuzz kAvol*((0.50 + ktremA*kAtremSwitch)), (kAcoarse+kAfine)+p4, knh, 1, katten, 1

kBtremInvert chnget "invert2"
kBtremSwitch chnget "onoff2"
kBcoarse chnget "coarse2"
kBvol chnget "volume2"
kBfine chnget "fine2"
kBtremDivision chnget "tremdiv2"
ktremB	oscili	0.4, kTremSpeed/((kBtremInvert+1)*kBtremDivision), 1	
;aoscB	vco2	kBvol*((0.50 + ktremB*kBtremSwitch)*0dbfs), kBcoarse+kAfine, 0
aoscB gbuzz kBvol*((0.50 + ktremB*kBtremSwitch)), (kBcoarse+kBfine)+p4, knh, 1, katten, 1

kCtremInvert chnget "invert3"
kCtremSwitch chnget "onoff3"
kCcoarse chnget "coarse3"
kCvol chnget "volume3"
kCfine chnget "fine3"
kCtremDivision chnget "tremdiv3"
ktremC	oscili	0.4, kTremSpeed/((kCtremInvert+1)*kCtremDivision), 1	
;aoscC	vco2	kCvol*((0.50 + ktremC*kCtremSwitch)*0dbfs), kCcoarse+kAfine, 0
aoscC gbuzz kCvol*((0.50 + ktremC*kCtremSwitch)), (kCcoarse+kCfine)+p4, knh, 1, katten, 1

kDtremInvert chnget "invert4"
kDtremSwitch chnget "onoff4"
kDcoarse chnget "coarse4"
kDvol chnget "volume4"
kDfine chnget "fine4"
kDtremDivision chnget "tremdiv4"
ktremD	oscili	0.4, kTremSpeed/((kDtremInvert+1)*kDtremDivision), 1	
;aoscD	vco2	kDvol*((0.50 + ktremD*kDtremSwitch)*0dbfs), kDcoarse+kAfine, 0
aoscD gbuzz kDvol*((0.50 + ktremD*kDtremSwitch)), (kDcoarse+kDfine)+p4, knh, 1, katten, 1

kfco chnget "cutoff"
kres chnget "rez"
kdist chnget "dist"


amixer = aoscA+aoscB+aoscC+aoscD
alft lpf18 amixer/4, kfco, kres, kdist 
abal balance alft, amixer/4

kmasterVol chnget "mastervol"
printk2 kmasterVol


kspace jspline 1, .25, 2
aLeft = kMasterEnv*(abal*abs(kspace))*kmasterVol
aRight = kMasterEnv*(abal*(1-abs(kspace)))*kmasterVol 
outs aLeft, aRight 

kLeft rms aLeft
kRight rms aRight

chnset kLeft, "left"
chnset kRight, "right"

endin


</CsInstruments>
<CsScore>
f1 0 1024 10 1
f0 3600
;i1 0 1000 50 1
</CsScore>
</CsoundSynthesizer>