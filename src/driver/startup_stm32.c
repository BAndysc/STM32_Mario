/** Uniwersalna procedura startowa dla STM32 **/

/* Następujące zmienne są zdefiniowane w skrypcie konsolidatora. */
extern unsigned long _sidata;   /* pierwsze 4 bajty danych
                                   inicjowanych w FLASH */
extern unsigned long _sdata;    /* pierwsze 4 bajty danych
                                   inicjowanych w RAM */
extern unsigned long _edata;    /* 4 bajty tuż za danymi
                                   inicjowanymi w RAM */
extern unsigned long _sbss;     /* pierwsze 4 bajty danych
                                   nieinicjowanych w RAM */
extern unsigned long _ebss;     /* 4 bajty tuż za danymi
                                   nieinicjowanymi w RAM */
extern unsigned long _estack;   /* początkowy wierzchołek stosu */

/* Deklaracja głównej funkcji programu */
int main(void);

/* Domyślna procedura obsługi przerwania - nieoczekiwane
   przerwanie zawiesza mikrokontroler. */
static void Default_Handler(void) {
  for (;;);
}

/* Procedura wołana po wyzerowaniu mikrokontrolera */
static void Reset_Handler(void) {
  unsigned long *src, *dst;
  /* Kopiuj dane inicjowane z FLASH do RAM. */
  for (dst = &_sdata, src = &_sidata; dst < &_edata; ++dst, ++src)
    *dst = *src;
  /* Zeruj dane nieinicjowane. */
  for (dst = &_sbss; dst < &_ebss; ++dst)
    *dst = 0;
  /* Wołaj główną funkcję programu - zwykle nigdy nie wraca. */
  main();
  /* Gdyby jednak nastąpił powrót, kręć się w kółko. */
  for (;;);
}

/* Włączany plik jest generowany automatycznie. */
#include <interrupt_vector_stm32.c>
