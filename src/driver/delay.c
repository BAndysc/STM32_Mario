#include <delay.h>
#include <stm32.h>

/* Ta implementacja nie działa, bo kompilator,
optymalizując kod, usuwa pętlę. */
void DelayBad1(unsigned count) {
  while (count--);
}

/* Ta implementacja działa, ale nie jest dobra, bo kod wynikowy
niepotrzebnie odwołuje się do pamięci danych, co może np. zmniejszać
efektywność transmisji DMA wykonywanej w tle. */
void DelayBad2(volatile unsigned count) {
  while (count--);
}

/* Optymalizację można zblokować, umieszczając wstawkę asemblerową.
Dodatkowo zmieniając liczbę NOP-ów, można dopasować czas opóźnienia:
tu jedna iteracja pętli wykonuje się 4 takty zegara.
Ta imlementacja używa tylko rejestrów. */
void Delay(unsigned count) {
  while (count--) {
    __NOP();
    __NOP();
  }
}
