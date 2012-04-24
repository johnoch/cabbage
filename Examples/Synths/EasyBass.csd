<Cabbage>
form caption("Cabbage Easy Bass") size(440, 160), colour("black"), pluginID("ezb1")
rslider pos(0, 0), size(80, 80) channel("cf"), range(1, 2000, 380), caption("Centre Freq"), colour("white"), midictrl(1, 1)
rslider pos(85, 0), size(80, 80) channel("res"), range(0, 1, 1), caption("Resonance"), colour("white"), midictrl(1, 2)
rslider pos(170, 0), size(80, 80) channel("disto"), range(0, 3, 2.5), caption("Dist."), colour("white"), midictrl(1, 3)
rslider pos(255, 0), size(80, 80) channel("envdep"), range(0, 1000, 500), caption("Env Depth"), colour("white"), midictrl(1, 4)
rslider pos(340, 0), size(80, 80) channel("decay"), range(0, 10, 1), caption("Decay"), colour("white"), midictrl(1, 5)
keyboard pos(0, 90), size(420, 50)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 --midi-key-cps=4 --midi-velocity-amp=5 -m0d
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

instr 1
kfco chnget "cf"		;filter cut-off
kres chnget "res" 		;filter resonance
kdist chnget "disto"		;filter distortion
kEnvMod chnget "envdep" 	;filter envelope depth
kdec chnget "decay"		;filter envelop decay

aenv madsr 0.2, .2, .6, .2 			;midi ADSR (output range 0-1)
avco vco2 p5, p4				;saw tooth waveform
kmod expsegr i(kEnvMod)+0.01, i(kdec), 0.1, i(kdec), 0.01	;exponential decay for filter cut-off
aflt lpf18 avco, kfco+kmod, kres, kdist 	;3-pole lowpass filter
outs aflt*aenv, aflt*aenv			
endin

</CsInstruments>
<CsScore>
f0 3600
</CsScore>
</CsoundSynthesizer>
