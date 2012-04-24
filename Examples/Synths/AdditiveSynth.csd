<Cabbage>
form caption("Cabbage Simple Synth") size(580, 200), pluginID("add1")
groupbox text("Simple Additive Synth"), pos(0, 0), size(200, 100)
rslider bounds(5, 30, 60, 60), channel("rise"), size(300, 50), colour("white"), range(0, 1, 0.1), text("Rise")
rslider bounds(65, 30, 60, 60), channel("decay"), colour("white"), text("Decay"), range(0, 3, 1)
rslider bounds(125, 30, 60, 60), channel("deviation"), colour("white"), text("Dev.."), range(0, 3, 1) 
combobox bounds(0, 105, 200, 20), channel("table"), value(1), text("Wave Table 1", "Wave Table 2", "Wave Table 3", "Wave Table 4", "Wave Table 5", "Wave Table 6")
keyboard bounds(0, 130, 550, 50)
csoundoutput bounds(210, 00, 340, 145), text("Csound Output")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 --midi-key-cps=4 --midi-velocity-amp=5 
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2



instr 1
kdev chnget "deviation"
krise chnget "rise"
kdecay chnget "decay"
ktable chnget "table"
k1 expsegr 0.001, i(krise), p5/7, i(kdecay), 0.01
a1 oscilikt k1, p4, ktable
a2 oscilikt k1, p4+kdev, ktable
a3 oscilikt k1, p4+(kdev*2), ktable
a4 oscilikt k1, p4+(kdev*3), ktable
a5 oscilikt k1, p4-kdev, ktable
a6 oscilikt k1, p4-(kdev*2), ktable
a7 oscilikt k1, p4-(kdev*3), ktable
apan jspline 1, 1, 3
outs apan*(a1+a2+a3+a4+a5+a6+a7), (1-apan)*(a1+a2+a3+a4+a5+a6+a7)
endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1 [1/2] [1/3] [1/4] [1/5] [1/6] [1/7] [1/8] [1/9] [1/10] 
f2 0 4096 10 1 0 [1/3] 0 [1/5] 0 [1/7] 0 [1/9] 0 [1/11] 
f3 0 4096 10 1 [1/9] 0 [1/25] 0 [1/49] 0 [1/81] 0 [1/122]
f4 0 4096 10 1 0 0 0 .1 .1 .1 .1 .1 .1 
f5 0 4096 10 1 1 1 1 1 1 1 1 1
f6 0 4096 10 1 0 1 0 1 0 1 0 .1 
f0 300
</CsScore>
</CsoundSynthesizer>