<Cabbage>
form caption("Phaser") size(460, 120), pluginID("esgs")
rslider bounds(0, 0, 80, 80), caption("Rate"), channel("rate"), range(0, 1, 0.2)
rslider bounds(90, 0, 80, 80), caption("Depth"), channel("depth"), range(0, 1, 0.5)
rslider bounds(180, 0, 80, 80), caption("Freq."), channel("freq"), range(0, 1, 0.4)
rslider bounds(270, 0, 80, 80), caption("Feedback"), channel("fback"), range(0, 1, 0.4)
rslider bounds(360, 0, 80, 80), caption("Level"), channel("level"), range(0, 1, 1)
launch bounds(0, 100, 100, 60), text("View Help"), file("help.html")
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1



opcode	Phaser,a,akkkk
	ain,krate,kdepth,kfreq,kfback	xin					;READ IN INPUT ARGUMENTS
	krate		expcurve	krate,10					;CREATE AN EXPONENTIAL REMAPPING OF krate
	krate		scale	krate,14,0.01					;RESCALE 0 - 1 VALUE TO 0.01 - 14	
	klfo	lfo	kdepth*0.5, krate, 1					;LFO FOR THE PHASER (TRIANGULAR SHAPE)
	aout	phaser1	ain, cpsoct((klfo+(kdepth*0.5)+kfreq)), 8, kfback	;PHASER1 IS APPLIED TO THE INPUT AUDIO
		xout	aout							;SEND AUDIO BACK TO CALLER INSTRUMENT
endop

instr 1
krate chnget "rate"
kdepth chnget "depth"
kfreq chnget "freq"
kfback chnget "fback"
klevel chnget "level"
printk2 klevel
printk2 krate
a1,a2	ins
a1	Phaser	a1,krate,kdepth*8,(kfreq*5)+6,kfback*0.9	
a2	Phaser	a2,krate,kdepth*8,(kfreq*5)+6,kfback*0.9	
a1	=	a1 * klevel
a2	=	a2 * klevel
	outs	a1,a2
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>