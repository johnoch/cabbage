<Cabbage>
form caption("pvsfreeze") size(520, 140)

groupbox bounds(0,  0, 95, 110), text("Freeze Amps")
checkbox channel("freeza"), pos(10, 10), size(100,100), value(0)

groupbox bounds(100,  0, 95, 110), text("Freeze Freqs")
checkbox channel("freezf"), pos(110, 10), size(100,100), value(0)

groupbox bounds(200,  0, 95, 110), text("Freeze Both")
checkbox channel("both"), pos(210, 10), size(100,100), value(0)

rslider pos(300, 10), text("Mix"), channel("mix"), range(0, 1, 1)
rslider pos(400, 10), text("Level"), channel("lev"), range(0, 1, 0.5, 0.5)

</Cabbage>

<CsoundSynthesizer>

<CsOptions>
-d -n
</CsOptions>

<CsInstruments>

sr 		= 	44100
ksmps 		= 	32
nchnls 		= 	2
0dbfs		=	1	;MAXIMUM AMPLITUDE


opcode	pvsfreeze_module,a,akkkkiiii
	ain,kfreeza,kfreezf,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_freeze 	pvsfreeze 	f_anal, kfreeza, kfreezf			;FREEZE AMPLITUDES OR FREQUENCIES (OR NEITHER) ACCORDING TO THE SWITCHES gkfreeza AND gkfreezf
	aout		pvsynth f_freeze                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	kmix	chnget	"mix"
	kfreeza	chnget	"freeza"
	kfreezf	chnget	"freezf"
	kboth	chnget	"both"
	if kboth==1 then
	 kfreeza = 1
	 kfreezf = 1
	endif

	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	
	kmix		chnget	"mix"
	klev		chnget	"lev"
	aoutL		pvsfreeze_module	ainL,kfreeza,kfreezf,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvsfreeze_module	ainR,kfreeza,kfreezf,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>
<CsScore>
i 1 0 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>