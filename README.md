timetool
========

Timetool software transforms the time from Local Sidereal Time (LST), Local Time (LT) or Julian Dates (JD) to Universal Time (UT). 

For conversion: LST  -> UT see<br>
S. Aoki et al., The New Definition of Universal Time, Astron. Astrophys. 105, 359-361, (1982).

du = JulianDate(y, m, d, 0, 0, 0) - 2451545;<br>
T = du/36525;<br>
GMST1 = 24110.54841 + T*(8640184.812866 + 0.093104*T – 6.2e-6*T*T);<br>
LST = hours(hh, mm, ss); <br>
UT = LST - GMST1/3600.0 - Longitude/15; <br>
UT = UT/24; <br>
UT= 24*(UT - int(UT)); <br>

Site for checking: <br>
http://www.csgnetwork.com/siderealjuliantimecalc.html <br>

For MS Windows add<br>
CONFIG += console<br>
in .pro file.

N. Kirov, M. Tsvetkov, K. Tsvetkova, Technology for digitization of astronomical photographic plates, In: Proceedings of the
8th Annual International Conference on Computer Science and Education in Computer Science, 5–8 July 2012, Boston, USA, 109-114.<br>
(http://nikolay.kirov.be/zip/nkirov_boston_updated.pdf)

N. Kirov, M. Tsvetkov, K. Tsvetkova, WFPDB: Software for Time and Coordinates Conversions, In: Proceedings of the IX Bulgarian-Serbian Astronomical Conference: Astroinformatics, (IX BSACA)
Sofia, Bulgaria, July 2-4, 2014, Editors: M. K. Tsvetkov, M. S. Dimitrijevi?c, O. Kounchev, D. Jevremovi?c 
and K. Tsvetkova, Publ. Astron. Soc. ?Rudjer Bo?skovi?c? No 15, 2015, 43-48.<br>
 (http://nikolay.kirov.be/zip/06.pdf)


