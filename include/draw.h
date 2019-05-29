#ifndef __DRAW_H__
#define __DRAW_H__



int OpenTextFile(char *FileName);
int SetTextDetail(char *pcHzkFile,char *pcFileFreeType, unsigned int FontSize);
int SelectAndInitDisplay(char *Name);
int ShowNextPage(void);

int ShowPrevPage(void);

#endif /* __DRAW_H__ */


