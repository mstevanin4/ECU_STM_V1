MS41 Quickflash - Siemens MS41 Flasher and Logger
https://www.bimmertuningtools.com 

===========================

Getting Started Tuning Your M52/S52 With RomRaider-
https://sites.google.com/site/openms41/romraider/how-to-get-started-tuning-your-m52-s52-with-romraider

Enable Write Mode on MS41
https://sites.google.com/site/openms41/read-write-ms41/enable-write

Romraider MS41 Tuning discussion-
http://www.romraider.com/forum/viewforum.php?f=58

===========================

Requirements:
.NET Framework 4.6 or later
FTDI Driver - https://ftdichip.com/drivers/d2xx-drivers/
FT232RL Cable

Windows 7 users must have the below updates installed: 
https://catalog.update.microsoft.com/search.aspx?q=kb4474419 
https://catalog.update.microsoft.com/search.aspx?q=4490628

===========================

Using MS41 Quickflash:

Functions will remain disabled until your cable is properly detected and installed. If Quickflash reamins disabled after connecting your cable, there is a problem with detecting the cable, wrong cable, or cable drivers installed improperly. 

Reading ECU-
Use the read tab. You can read the partial (calibration/tune) or the full ecu. There's a fast read option that is fairly stable that can cut the read time down significantly. 

Flashing ECU- 
Use the Flash tab. You can flash tune/partial/calibration section or full(calibration and program) section. It will also patch between MS41.0/ms41.1/ms41.2. 
Check the ews delete checkbox before selecting your file to have EWS delete automatically patched when flashing. You can EWS delete with both full or partial.

Clear Tab allows you to clear diagnostic trouble codes(DTCs) and adaptations. It also allows you to read DTCs. 

Adaptations tab allows you to read learned knock adaptations as well as learned lambda adaptations. 

Live tab allows you to view and optionally log live engine parameters. Both telegram and traditional logging are supported. The log file will save to the directory MS41 Quickflash is located. 