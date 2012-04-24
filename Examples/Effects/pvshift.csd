<Cabbage>
form caption("pvshift Frequency Shifter") size(560, 180)
groupbox bounds(10,  10, 220, 140), text("Shift"), plant("Shift"){
rslider pos( 10, 20), text("Course"), channel("shiftC"), range(-5000, 5000, 0)
rslider pos(110, 20), text("Fine"), channel("shiftF"), range(-10, 10, 0)
}
groupbox bounds(230,  10, 310, 140){
rslider pos(230, 30), text("Feedback"), channel("FB"), range(0, 1, 0)
rslider pos(330, 30), text("Mix"), channel("mix"), range(0, 1, 0.5)
rslider pos(430, 30), text("Level"), channel("lev"), range(0, 1, 0.5, 0.5)
}

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


opcode	pvshift_module,a,akkkkiiii
	ain,kshift,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype	xin
	f_FB		pvsinit iFFTsize,ioverlap,iwinsize,iwintype, 0			;INITIALISE FEEDBACK FSIG
	f_anal  	pvsanal	ain, iFFTsize, ioverlap, iwinsize, iwintype		;ANALYSE AUDIO INPUT SIGNAL AND OUTPUT AN FSIG
	f_mix		pvsmix	f_anal, f_FB						;MIX AUDIO INPUT WITH FEEDBACK SIGNAL
	f_shift		pvshift f_mix, kshift, 20					;SHIFT FREQUENCIES
	f_FB		pvsgain f_shift, kfeedback 					;CREATE FEEDBACK F-SIGNAL FOR NEXT PASS
	aout		pvsynth f_shift                      				;RESYNTHESIZE THE f-SIGNAL AS AN AUDIO SIGNAL
	amix		ntrpol		ain, aout, kmix					;CREATE DRY/WET MIX
			xout		amix*klev	
endop

instr	1
	ainL,ainR	ins
	kmix	chnget	"mix"
	kFB	chnget	"FB"

	iFFTsize	=	1024
	ioverlap	=	256
	iwinsize	=	1024
	iwintype	=	1
	
	kfeedback	chnget	"FB"
	kshiftC		chnget	"shiftC"
	kshiftF		chnget	"shiftF"
	kporttime	linseg	0,0.001,0.02
	kshift		portk	kshiftC+kshiftF,kporttime
	kmix		chnget	"mix"
	klev		chnget	"lev"
	aoutL		pvshift_module	ainL,kshift,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
	aoutR		pvshift_module	ainR,kshift,kfeedback,kmix,klev,iFFTsize,ioverlap,iwinsize,iwintype
			outs	aoutR,aoutR
endin

</CsInstruments>

<CsScore>
i 1 0 [60*60*24*7]
</CsScore>

</CsoundSynthesizer>