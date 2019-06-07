#ifndef __DRAW_H__
#define __DRAW_H__



int OpenTextFile(char *FileName);
int SetTextDetail(char *pcHzkFile,char *pcFileFreeType, unsigned int FontSize);
int SelectAndInitDisplay(char *Name);
int ShowNextPage(void);
int GetDisplayResolution(int *pXres,int *pYres);

int ShowPrevPage(void);

#endif /* __DRAW_H__ */


