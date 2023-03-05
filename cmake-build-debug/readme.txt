Projekt realizowany w ramach przedmiotu "Podstawy programowanie", jest symulatorem gry w statki dla 2 graczy.

Dostępne komendy to:


BOARD_SIZE 21 10 - ustawnie wielkości planszy

NEXT_PLAYER A - ustawienie kolejności wykonywania ruchów

PRINT x - pokazuje stan gry widoczny dla danego gracza

REEF y x - stawia rafę, będąca przeszkodą dla statków

SET_FLEET P a1 a2 a3 a4 - ustawia wielkość floty gracza P (CARRIER ... DESTROYER)

[playerA] - zaczyna i kończy tury danego gracza

[playerB] - zaczyna i kończy tury danego gracza



PLACE_SHIP 16 0 N 0 DES - postawienie statku klasy [DES-destroyer] na polu 16,0, skierowanego na północ

MOVE i C F/R/L - przesuwa i-ty statek klasy C do przodu/w prawo/w lewo

SHOOT i C y x - strzał i-tego statku C klasy w pole y,x

SPY - umożliwia graczowi wgląd w objęte zasięgiem obszary



Przykładowy widok planszy wygląda następująco:

  00000000001
  01234567890
00
01
00 # @!++%
01
02   #%
03    @


Gdzie:

 % - silnik
 ! - armata
 @ - radar
 + - część statku

 # - rafa
 X - zniszczona część statku

 Statki maja wbudowane radary oraz armaty

 gdy zostaną zestrzelone, nie mogą funkcjonować

