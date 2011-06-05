<Cabbage>
form caption("Simple Sine"), size(450, 80)
checkbox pos(1, 1), size(100, 60), caption("tyest"), channel("sine"), items("one", "two")
</Cabbage>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr     = 44100
kr     = 4410
ksmps  = 10
nchnls = 2

instr 1 ;Simple sine at 440Hz
k1 chnget "sine"
a1	oscili 10000,k1*1000,1
	outs a1, a1
	endin
</CsInstruments>

<CsScore>
f1 0 4096 10 1
i1 0 300
</CsScore>
</CsoundSynthesizer>
