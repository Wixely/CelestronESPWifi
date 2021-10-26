# CelestronESPWifi
ESP based wifi adapter for Celestron telescopes

The official celestron wifi module is €190.

CelestronESPWifi parts list:
  ESP Module: €1.25c
  Buck Converter: €0.75c
  Cable: €3.00
  Jellybeans: €2.00

Total = less than €10 anyway.

## Communications System ##

The scope uses a UART system over one wire. Multiple devices can communicate on this same bus by pulling RST low, then sending data. Source and Destination IDs are sent in the packets so devices know what data is for them.

## Notes and Features ##

1. Works with SkyPortal App seemingly perfectly.
2. Does not like other peripherals yet (keypad must be unplugged)
3. ESP firmware can be updated remotely

## Similar projects ##

This project would be much more difficult without the help of these posts:

https://sebastianfoerster86.wordpress.com/2020/08/10/celestron-teleskop-skyq-link-wi-fi-hack/

https://sites.google.com/site/wayneholder/nexstar-direct-telescope-mount-control

https://www.cloudynights.com/topic/652569-nexremote-cable-with-usb-ttl-serial-uart-converter-cable-with-ftdi-chip/page-2

Simply having this info meant that I could confirm my setup worked and my logic analyser was reading expected data.
