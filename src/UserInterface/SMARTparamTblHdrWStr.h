/**SMARTparamTblHdrWStr.h: File name comes from:
 * param=PARAMeter
 * Tbl=TaBLe:
 * Hdr=HeaDeR
 * WStr=Wide STRing
 *
 * Created 25.09.2022 by Stefan Gebauer*/

#ifndef USERINTERFACE_SMARTPARAMTBLHDRWSTR_H_
#define USERINTERFACE_SMARTPARAMTBLHDRWSTR_H_

#ifdef __cplusplus
/**Must declare as extern "C" if compiling as C++, else Microsoft Visual Studio
 * gives linker errors.*/
extern "C"{
#endif

extern wchar_t * enColHdrWstrs[];
extern unsigned maxColValStrWdthInPx[];
extern wchar_t * widestColValStr[];

#ifdef __cplusplus
}///extern "C"
#endif

#endif /* USERINTERFACE_SMARTPARAMTBLHDRWSTR_H_ */
