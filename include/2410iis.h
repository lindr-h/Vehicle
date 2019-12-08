//====================================================================
// File Name : 2410iis.h
// Function  : S3C2410 
// Program   : Shin, On Pil (SOP)
// Date      : March 20, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (March 06, 2002) -> SOP
//====================================================================

#ifndef __2410IIS_H__
#define __2410IIS_H__

#ifdef __cplusplus
extern "C" {
#endif

//void PlayWav(U32 sample,S16 *lBuf,S16 *rBuf,U32 freq);
//void PlayWav(void);
void Init1341(char);
void Iis_Tx(void);
void Record_Iis(void);
void playwav(void);
//void Play1341(void);
//void Stop1341(void);
//void Iis_Tx(void);
//void Iis_Rx(void);

#ifdef __cplusplus
}
#endif

#endif    //__2410IIS_H__