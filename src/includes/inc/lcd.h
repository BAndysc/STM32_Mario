#ifndef _LCD_H
#define _LCD_H 1

void LCDconfigure(void);
void LCDclear(void);
void LCDgoto(int textLine, int charPos);
void LCDputchar(char c);
void LCDputcharWrap(char c);

#endif
