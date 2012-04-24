;The following instrument is only useful as a plugin as it retrieves data from the plugin host. 
;Cabbage can find out the host BPM, the current times in seconds since the start
;of play, and whether the host is recording or playing back.
<Cabbage>
form caption("Host Info") size(460, 320)
csoundoutput bounds(10, 5, 420, 260), text("Csound Output")
checkbox text("Show host time - if host is not playing this will not display"), bounds(15, 270, 420, 20), channel("show_time")

</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
; Initialize the global variables.
sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

instr 1

endin

</CsInstruments>
<CsScore>
f1 0 4096 10 1
i1 0 1000
</CsScore>
</CsoundSynthesizer>