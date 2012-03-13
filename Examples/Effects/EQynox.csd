<Cabbage>
form size(410, 360), caption("EQynox")
groupbox bounds(0, 0, 390, 335), text("Bandpass Filters"), plant("filters"){
vslider bounds(15, 15, 60, 200), caption("Freq1"), channel("rand_filter_1"), range(0, 3, .24)
vslider bounds(75, 15, 60, 200), caption("Freq2"), channel("rand_filter_2"), range(0, 3, .24)
vslider bounds(135, 15, 60, 200), caption("Freq3"), channel("rand_filter_3"), range(0, 3, .24)
vslider bounds(195, 15, 60, 200), caption("Freq4"), channel("rand_filter_4"), range(0, 3, .24)
vslider bounds(255, 15, 60, 200), caption("Loop"), channel("loop_time"), range(0, 1, .24)
vslider bounds(315, 15, 60, 200), caption("Offset"), channel("offset"), range(0, 1, 0)
image bounds(15, 220, 360, 100), colour("black"), shape("rounded"), outline("white"), line(4)
rslider bounds(25, 230, 80, 80), caption("Reverb"), channel("rev"), range(0, 1, .2)
rslider bounds(110, 230, 80, 80), caption("Q"), channel("qfactor"), range(1, 1000, 1)
rslider bounds(195, 230, 80, 80), caption("Main In"), channel("main_in"), range(0, 1, .5)
rslider bounds(280, 230, 80, 80), caption("Main Out"), channel("main_out"), range(0, 1, .5)
}
</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 32
nchnls = 2

instr BandpassFilters
;random bandpass filtering with vcomb to pan
ain inch 1
kin chnget "main_in"
ain = ain*kin

kfreq1 chnget "rand_filter_1"
kfreq2 chnget "rand_filter_2"
kfreq3 chnget "rand_filter_3"
kfreq4 chnget "rand_filter_4"

krand1 randh 1000, kfreq1, 2
krand2 randh 1000, kfreq2, 2
krand3 randh 1000, kfreq3, 2
krand4 randh 1000, kfreq4, 2


kq chnget "qfactor"

aflt1 butterbp ain, abs(krand1)+100, abs(krand1)/(kq+5)  
aflt2 butterbp ain, abs(krand2)+300, abs(krand2)/(kq+5)
aflt3 butterbp ain, abs(krand3)+400, abs(krand3)/(kq+5)
aflt4 butterbp ain, abs(krand4)+500, abs(krand4)/(kq+5)

abal balance (aflt1+aflt2+aflt3+aflt4)/4, ain

kloop chnget "loop_time"
koff chnget "offset"
krev chnget "reverb"

aL vcomb abal, 2, kloop, 2
aR vcomb abal, 2, kloop+koff, 2

kout chnget "main_out"

aL reverb aL, krev
aR reverb aR, krev
outs aL*kout, aR*kout

endin

</CsInstruments>

<CsScore>
i1 0 1000
</CsScore>

</CsoundSynthesizer>