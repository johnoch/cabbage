<Cabbage>
form caption("Drone Instrunment"), size(700, 650), colour("black")
keyboard bounds(10, 540, 660, 80)
rslider caption("Trem Freq"), bounds(550, 10, 120, 90), colour("white"), channel("tremspeed")
rslider caption("Master"), bounds(550, 440, 120, 90), channel("mastervol"), colour("white"), range(0, 1, 1)

groupbox caption("Tremelo"), plant("trem1"), bounds(10, 10, 120, 180), colour("white"){
checkbox caption("On/Off"), bounds(20, 20, 70, 20), colour("white"), channel("onoff1")
checkbox caption("Invert"), bounds(20, 40, 70, 20), colour("white"), channel("invert1")
rslider bounds(10, 65, 100, 100), caption("Division"), channel("tremdiv1"), colour("orange"), range(0, 1, 0)
}

groupbox caption("Tremelo"), plant("trem2"), bounds(140, 10, 120, 180), colour("white"){
checkbox caption("On/Off"), bounds(20, 20, 70, 20), colour("white"), channel("onoff2")
checkbox caption("Invert"), bounds(20, 40, 70, 20), colour("white"), channel("invert2")
rslider bounds(10, 65, 100, 100), caption("Division"), channel("tremdiv2"), colour("orange"), range(0, 1, 0)
}

groupbox caption("Tremelo"), plant("trem3"), bounds(270, 10, 120, 180), colour("white"){
checkbox caption("On/Off"), bounds(20, 20, 70, 20), colour("white"), channel("onoff3")
checkbox caption("Invert"), bounds(20, 40, 70, 20), colour("white"), channel("invert3")
rslider bounds(10, 65, 100, 100), caption("Division"), channel("tremdiv3"), colour("orange"), range(0, 1, 0)
}

groupbox caption("Tremelo"), plant("trem4"), bounds(400, 10, 120, 180), colour("white"){
checkbox caption("On/Off"), bounds(20, 20, 70, 20), colour("white"), channel("onoff4")
checkbox caption("Invert"), bounds(20, 40, 70, 20), colour("white"), channel("invert4")
rslider bounds(10, 65, 100, 100), caption("Division"), channel("tremdiv4"), colour("orange"), range(0, 1, 0)
}

image caption("Low-pass"), plant("filter"), bounds(550, 110, 120, 320), colour("white"), line(4), outline("orange"){
rslider bounds(10, 10, 100, 100), caption("Cut-off"), channel("cutoff"), colour("black"), range(1, 5000, 1500)
rslider bounds(10, 110, 100, 100), caption("Rez."), channel("rez"), colour("black"), range(0, 1, 0)
rslider bounds(10, 210, 100, 100), caption("Dist."), channel("dist"), colour("black"), range(0, 3, 0)
}
groupbox caption("Osc. A"), plant("OSC1"), bounds(10, 200, 120, 330), colour("white"){
rslider bounds(10, 15, 100, 100), caption("Coarse"), channel("coarse1"), colour("orange"), range(0, 1000, 0) 
rslider bounds(10, 115, 100, 100), caption("Fine"), channel("fine1"), colour("orange"), range(0, 100, 0)
rslider bounds(10, 215, 100, 100), caption("Volume"), channel("volume1"), colour("orange"), range(0, 1, 0)
}
groupbox caption("Osc. B"), plant("OSC2"), bounds(140, 200, 120, 330), colour("white"){
rslider bounds(10, 15, 100, 100), caption("Coarse"), channel("coarse2"), colour("orange"), range(0, 1000, 0)
rslider bounds(10, 115, 100, 100), caption("Fine"), channel("fine2"), colour("orange"), range(0, 100, 0)
rslider bounds(10, 215, 100, 100), caption("Volume"), channel("volume2"), colour("orange"), range(0, 1, 0)
}
groupbox caption("Osc. C"), plant("OSC3"), bounds(270, 200, 120, 330), colour("white"){
rslider bounds(10, 15, 100, 100), caption("Coarse"), channel("coarse3"), colour("orange"), range(0, 1000, 0)
rslider bounds(10, 115, 100, 100), caption("Fine"), channel("fine3"), colour("orange"), range(0, 100, 0)
rslider bounds(10, 215, 100, 100), caption("Volume"), channel("volume3"), colour("orange"), range(0, 1, 0)
}
groupbox caption("Osc. D"), plant("OSC4"), bounds(400, 200, 120, 330), colour("white"){
rslider bounds(10, 15, 100, 100), caption("Coarse"), channel("coarse4"), colour("orange"), range(0, 1000, 0)
rslider bounds(10, 115, 100, 100), caption("Fine"), channel("fine4"), colour("orange"), range(0, 100, 0)
rslider bounds(10, 215, 100, 100), caption("Volume"), channel("volume4"), colour("orange"), range(0, 1, 0)
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
; Updated to Cabbage by Rory Walsh
;

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1


instr 1
kTremSpeed chnget "tremspeed"
kAtremInvert chnget "invert1"
kAtremSwitch chnget "onoff1"
kAcoarse chnget "coarse1"
kAvol chnget "volume1"
kAfine chnget "fine1"
kAtremDivision chnget "tremdiv1"
ktremA	oscili	0.4, kTremSpeed/((kAtremInvert+1)*kAtremDivision), 1	
;aoscA	vco2	kAvol*((0.50 + ktremA*kAtremSwitch)*0dbfs), kAcoarse+kAfine, 0
aoscA gbuzz kAvol*((0.50 + ktremA*kAtremSwitch)*0dbfs), (kAcoarse+kAfine)+p4, 20, 0, 0.9, 1

kBtremInvert chnget "invert2"
kBtremSwitch chnget "onoff2"
kBcoarse chnget "coarse2"
kBvol chnget "volume2"
kBfine chnget "fine2"
kBtremDivision chnget "tremdiv2"
ktremB	oscili	0.4, kTremSpeed/((kBtremInvert+1)*kBtremDivision), 1	
;aoscB	vco2	kBvol*((0.50 + ktremB*kBtremSwitch)*0dbfs), kBcoarse+kAfine, 0
aoscB gbuzz kBvol*((0.50 + ktremB*kBtremSwitch)*0dbfs), (kBcoarse+kBfine)+p4, 20, 0, 0.9, 1

kCtremInvert chnget "invert3"
kCtremSwitch chnget "onoff3"
kCcoarse chnget "coarse3"
kCvol chnget "volume3"
kCfine chnget "fine3"
kCtremDivision chnget "tremdiv3"
ktremC	oscili	0.4, kTremSpeed/((kCtremInvert+1)*kCtremDivision), 1	
;aoscC	vco2	kCvol*((0.50 + ktremC*kCtremSwitch)*0dbfs), kCcoarse+kAfine, 0
aoscC gbuzz kCvol*((0.50 + ktremC*kCtremSwitch)*0dbfs), (kCcoarse+kCfine)+p4, 20, 0, 0.9, 1

kDtremInvert chnget "invert4"
kDtremSwitch chnget "onoff4"
kDcoarse chnget "coarse4"
kDvol chnget "volume4"
kDfine chnget "fine4"
kDtremDivision chnget "tremdiv4"
ktremD	oscili	0.4, kTremSpeed/((kDtremInvert+1)*kDtremDivision), 1	
;aoscD	vco2	kDvol*((0.50 + ktremD*kDtremSwitch)*0dbfs), kDcoarse+kAfine, 0
aoscD gbuzz kDvol*((0.50 + ktremD*kDtremSwitch)*0dbfs), (kDcoarse+kDfine)+p4, 20, 0, 0.9, 1

kfco chnget "cutoff"
kres chnget "rez"
kdist chnget "dist"


amixer = aoscA+aoscB+aoscC+aoscD
alft lpf18 amixer/4, kfco, kres, kdist 
;alft lpf18 amixer/4, 10000, kres, kdist 
abal balance alft, amixer/4

kmasterVol chnget "mastervol"
kmasterVol = kmasterVol+p5

kspace jspline 1, 1, 3

outs (abal*abs(kspace))*kmasterVol, (abal*(1-abs(kspace)))*kmasterVol
;outs amixer*kmasterVol, amixer*kmasterVol
endin


</CsInstruments>
<CsScore>
f1 0 1024 10 1 1 1
f0 3600
</CsScore>
</CsoundSynthesizer>