# Autoclicker
This emulate an usb mouse throught an attiny85
When pressing the button, you get 5 mode.
Pressing immediatly swhitch to the next mode. Pressing after 3 seconds go to
idle.
You can use a custom circuit or the usb-attiny provided by digispark

## 0 - Idle.
The mouse does nothing.
The led do a short blink evry 3s

## 1 - Round
Draw round on screen. This use a sin table for working faster
The led will blink at 0.5 Hz, with 50% of dutty.
This is useful for preventing computer sleeping

## 2 - Left click time to time
Provide a left click every 7s, without move.
The led will do a short blink, every 1s.
This may help if you want refresh a page or click next when installing a
software.

## 3 - Quick left click
Provide infinite and as quick as possible left click.
The led will blink quickly with (1s on, 0.2s off)
Use this mode if you cheat in video game, or if you have an application wich
show the same button as the same place.

## 4 - Draw a rose
Open paint on enjoy.
Led blink mediumly
I didn't found any seriuos use case of this feature
