<Cabbage>
form caption("ADSR Envelopes") size(360, 425), colour("black")
image plant("ADSR1"), pos(20, 0), size(300, 150), colour("black"), outline("orange"), line(4) { 
rslider pos(20, 65), size(60, 60), channel("attack"), min(0), max(5), colour("orange")
rslider pos(85, 65), size(60, 60), channel("decay"), min(0), max(5), colour("orange")
rslider pos(150, 65), size(60, 60), channel("sustain"), min(0), max(1), colour("orange")
rslider pos(215, 65), size(60, 60), channel("release"), min(0), max(5), colour("orange")
image pos(20, 10), size(255, 51), file("adsr.png")
image pos(10, 10), size(30, 30), shape("ellipse"), colour("white")
label caption("1"), pos(12, 8), size(25, 32), colour("black")
label caption("Attack"), bounds(25, 125, 100, 16), colour("white")
label caption("Decay"), pos(90, 125), colour("white")
label caption("Sustain"), pos(152, 125), colour("white")
label caption("Decay"), pos(220, 125), colour("white")
}

image plant("ADSR2"), pos(20, 200), size(300, 150), colour("black"), outline("orange"), line(4) { 
rslider pos(20, 65), size(60, 60), channel("attack"), min(0), max(5), colour("orange")
rslider pos(85, 65), size(60, 60), channel("decay"), min(0), max(5), colour("orange")
rslider pos(150, 65), size(60, 60), channel("sustain"), min(0), max(1), colour("orange")
rslider pos(215, 65), size(60, 60), channel("release"), min(0), max(5), colour("orange")
image pos(20, 10), size(255, 51), file("adsr.png")
image pos(10, 10), size(30, 30), shape("ellipse"), colour("white")
label caption("2"), pos(12, 8), size(25, 32), colour("black")
label caption("Attack"), bounds(25, 125, 100, 16), colour("white")
label caption("Decay"), pos(90, 125), colour("white")
label caption("Sustain"), pos(152, 125), colour("white")
label caption("Decay"), pos(220, 125), colour("white")
}

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=null -M0 -b1024 --midi-key-cps=4 --midi-velocity-amp=5
;-+rtaudio=alsa -odac
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2

instr 1

endin



</CsInstruments>
<CsScore>
f1 0 1024 10 1
f0 3600
</CsScore>
</CsoundSynthesizer>
