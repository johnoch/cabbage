<Cabbage>
form size(300, 450), caption("SyncGrain")
hslider pos(10, 0), channel("amplitude"), caption("Amplitude"), colour("lightgrey"), range(0.0, 1, .7)
xypad bounds(10, 60, 250, 250), channel("time_ratio", "pitch_ratio"), text("Time/Pitch Ratios"), rangex(-2, 2, -1), rangey(-2, 1, 0)
hslider pos(10, 320), channel("grain_size"), caption("Grain Size"), colour("lightgrey"), range(0.01, .5, .19)
hslider pos(10, 380), channel("grain_overlap"), caption("Grain Overlap"), colour("lightgrey"), range(1, 50, 10)
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2

/*************************************/
/* TPScaler instrument by            */
/* Victor Lazzarini                  */
/* Cabbage mod by Rory Walsh, 2012   */
/*************************************/

instr 1
k1 chnget "time_ratio"
k2 chnget "pitch_ratio"
k3 chnget "grain_size"
k4 chnget "grain_overlap"
k5 chnget "amplitude"

iomax = 100         
kol = int(k4)      
kgr = k3           
kfr = kol/kgr     
kps = 1/kol      

ain    inch  1
awp  phasor sr/ftlen(1)
awin tablei awp, 2, 1
     tablew ain*awin, awp, 1, 1
asig syncgrain  k5, kfr, k2, kgr, kps*k1, 1, 3, iomax

   outs asig, asig

endin

</CsInstruments>
<CsScore>
f2 0 131072 7 0 36 1 131000 1 36 0
f3 0  16384 9 0.5 1 0
f1 0 131072 7 0 131072 0
i1 0 3600
</CsScore>
</CsoundSynthesizer>