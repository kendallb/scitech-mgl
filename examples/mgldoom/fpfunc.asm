;****************************************************************************
;*
;* Language:    NASM
;* Environment: IBM PC
;*
;* Description: Module for fixed point arithmetic for MGLDoom
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

header  fpfunc                  ; Set up memory model

begcodeseg  fpfunc

cprocstart  FixedMul

        push    ebp
        mov     ebp,esp
        mov     eax,[ebp+8]
        imul    [DWORD ebp+12]
        shrd    eax,edx,16
        pop     ebp
        ret

cprocend

cprocstart  FixedDiv2

        push    ebp
        mov     ebp,esp
        mov     eax,[ebp+8]
        cdq
        shld    edx,eax,16
        sal     eax,16
        idiv    [DWORD ebp+12]
        pop     ebp
        ret

cprocend

endcodeseg  fpfunc

        END                     ; End of module

