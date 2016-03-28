; TRAPOUT2.ASM v2.0 by ARK (ark@lhq.com, root@ark.dyn.ml.org) 11-28-97
; Traps IN and OUT instructions in INT 10h and displays DX and AX/AL values.
;
; In the header "T DX/I AX/L", T is the Type of instruction (I=IN, O=OUT),
;  DX/I is the value of DX or the Immediate value if port<256, and AX/L
;  is the value of AX or AL depending on if an 8 or 16 bit value is listed.
;  AX/L is meaningless for IN's since it is the value if AX/L *before* the
;  call to IN.
;
; This is very useful to find information about how your video card works.
; I wrote this to get register dumps for my Trident TVGA9440AGi card so
;  that I could use it under Linux.
;
; NOTE: Pipe the output or you won't see anything!
;  (ex: TRAP-OUT 4F02 0101 > 640x480.256)
;
; New in v2.0:
;  * Traces into INT 10 calls that are called from inside INT 10!
;  * Allows AX and BX values to be specified!
;  * Command line accepts trailing spaces now.
;  x Code to trap INT's also! (T column='N', DX/I=INT ##, AX/L=AX value)
;    (Its commented out - but you can recompile with it if you want)
;
; How to assemble with Borland:
; tasm /ml /zd ncr.asm (case sensitive, line number debug info only)
; tlink /x /t ncr.obj (no map, make com file)
;

.model tiny           ; Tiny memory model, all segments point to the same 64K
.286                  ; This code will run on a 286...           actually, it 
.code                 ; Everything is in the code segment(cs)   will probably
.startup              ; Startup                               run on anything

jmp Start              ; Go to beginning of progam
realINT1 dd 52411A3Eh  ; Address of original INT 01h routine offset
realINT10 dd 3C1B214Bh ; Memory for [es:bx] of the real INT 10h
                       ; (defaults are '>-ARK!-<' just for looks in the .COM)

; strings
no_command_line db 'Use: TRAPOUT2 [AX] [BX]',13,10
db '     Traces all IN/OUT calls inside INT 10h',13,10,36
tracing db 'Tracing INT 10h with AX:',36
bx_msg db ' BX:',36
header db 13,10,'T DX/I AX/L',13,10,36

INT1 proc               ; Interrupt Service Routine for Single Step Debugging
        push ax                                              ; save registers
        push dx
        push es
        push di
        push bp        

        mov bp,sp                  ; set bp to the stack
        push word ptr cs:[bp+12]   ; put the real cs
        pop es                     ;  into es
        push word ptr cs:[bp+10]   ; put the real ip
        pop di                     ;  into di
        mov al,byte ptr es:[di]    ; set al to the next instruction that will
                                   ;  be executed after this INT 01 is done.

; This code will trap INT's also...
;        cmp al,0CDh                ; If al is not CD (INT) keep going
;        jne not_int                ; If it is, display some stuff...
;; This will skip doing the INT's...
;;        add word ptr cs:[bp+10],2  ; Add 2 to the real ip, to skip the INT
;        mov dl,4Eh                 ; Display an N
;        mov ah,02h                 ;  The immediate value/DX is the INT ##
;        int 21h                    ;  that is called. AX is the value before
;        mov dl,20h                 ; Display a space
;        mov ah,02h                 ; 
;        int 21h                    ; Display the immediate value which is
;        jmp is_imm                 ;  reallly the interrupt number called.

    not_int:
        and al,0F4h                ; If al is E4-E7 or EC-EF (all IN/OUT's)
        cmp al,0E4h                ; Then we display our stuff
        jne not_io                 ; Otherwise, do nothing
                                   ; note: 1 more byte of code after this
                                   ; jmp will make it out of range...

        mov al,byte ptr es:[di]         ; Set al to next instruction
        test al,02h                     ; If bit 1 is set then we have an OUT
        jz is_in                        ; If bit 1 is 0, we have an IN

        mov dl,4Fh                                             ; Display an O
        mov ah,02h
        int 21h
        jmp dx_or_imd

    is_in:                                                     ; Display an I
        mov dl,49h
        mov ah,02h
        int 21h

    dx_or_imd:                                              ; Display a space
        mov dl,20h
        mov ah,02h
        int 21h

        mov al,byte ptr es:[di]    ; Set al to next instruction
        test al,08h                ; If bit 3 is set then we are using DX
        jz is_imm                  ; If bit 3 is 0, we are using an immediate

        mov ax,[bp+6]                                      ; restore dx to ax
        call ShowHex                                       ; Display dx
        call ShowHex
        call ShowHex
        call ShowHex
        jmp ax_or_al

    is_imm:
        mov dl,20h                                         ; Display 2 spaces
        mov ah,02h
        int 21h
        mov dl,20h
        mov ah,02h
        int 21h

        mov ah,byte ptr es:[di+1] ; Set ah to byte after the next instruction
        call ShowHex              ; Display the immediate value
        call ShowHex

    ax_or_al:
        mov dl,2Ch                                          ; Display a comma
        mov ah,02h
        int 21h

        mov al,byte ptr es:[di]        ; Set al to next instruction
        test al,01h                    ; If bit 0 is set then we are using AX
        jz is_al                       ; If bit 0 is 0, we are using AL

        mov ax,[bp+8]                                            ; Restore ax
        call ShowHex                                             ; Display ax
        call ShowHex
        call ShowHex
        call ShowHex
        jmp print_next_line

    is_al:
        mov ah,[bp+8]                                      ; Restore al to ah
        call ShowHex                                       ; Display al
        call ShowHex

    print_next_line:
        mov dl,0Dh                                          ; print a newline
        mov ah,02h
        int 21h
        mov dl,0Ah
        mov ah,02h
        int 21h
    
    not_io:
        pop bp                                            ; restore registers
        pop di
        pop es
        pop dx
        pop ax
        iret                                                  ; end interrupt
INT1 endp

; INT 10h that fakes the real INT 10 and sets the trap flag.
INT10 proc                    ; Interrupt Service Routine for Tracing INT 10h
        push ax           ; Save AX
        pushf             ; Put flags on the stack
        pop ax            ; Then into AX
        or ax,0100h       ; Set the trap flag
        push ax           ; Trap Flag calls INT 01h between every instruction
        popf              ; Stuff new flags back into the flags register
        pop ax            ; Restore AX

        cli              ; Fake INT call: clear interrupt flag, skip clearing
        pushf            ;  trap flag, push flags, call to location.
        call cs:[realINT10]      ; This call to INT 10h is be trapped for
                                 ; IN/OUT/INT Normal INT calls would clear
                                 ; the trap flag and then INT 01h would never
                                 ; be called.
        iret                                                  ; end interrupt
INT10 endp

; function that prints the highest 4 bits of ax as text {0-9,A-F} to stdout
; ax will be shifted left 4 bits on return.
ShowHex proc
        push ax                                              ; save registers
        push dx

        shr ax,0Ch                  ; move the highest 4 bits to the lowest 4
        and al,0Fh                  ; limit to lowest 4 bits
        or  al,30h                  ; change range to 30h-3Fh {0-9:;<=>?}
        cmp al,39h                  ; if it is 30h-39h
        jbe is_0_thru_9             ; then its already set
        add al,07h                  ; otherwise change :;<=>? to A-F
    is_0_thru_9:
        mov dl,al
        mov ah,02h
        int 21h

        pop dx                                      ; restore dx
        pop ax                                      ; restore ax
        shl ax,4                                    ; set up ax for next call
        ret                                         ; return
ShowHex endp

Start:                               ; Program begins here
        mov si,0080h                 ; CS:0080h is the command line 
        cmp byte ptr [si],10         ; I want it to be at least 10 bytes long
        jae  process_command_line    ; if not, abort

        mov dx,offset no_command_line        ; ds is preset
        mov ah,09h                           ; Dos function 09h
        int 21h                              ; Display no command line string
        ret                                  ; Exit program

    process_command_line:
        inc si                  ; move si to start of actual string
        mov ax,[si+1]           ; copy first 2 chrs to ax, skipping the space
        mov bx,[si+3]           ; copy 2nd two characters to bx
        sub al,30h              ; subtract 30h so chrs 0-9 have value 0-9
        cmp al,09h              ; if its 0-9, its ok.
        jbe al_is_ok            ; if its not, its probably A-F or a-f
        sub al,07h              ; so subtract 7 more
        and al,0Fh              ; and limit to 0-F
    al_is_ok:
        sub ah,30h              ; do the same to ah
        cmp ah,09h
        jbe ah_is_ok
        sub ah,07h
        and ah,0Fh
    ah_is_ok:
        sub bl,30h              ; do the same to bl
        cmp bl,09h
        jbe bl_is_ok
        sub bl,07h
        and bl,0Fh
    bl_is_ok:
        sub bh,30h              ; do the same to bh
        cmp bh,09h
        jbe bh_is_ok
        sub bh,07h
        and bh,0Fh
    bh_is_ok:
        shl al,04h              ; Combine the values so that AL-AH-BL-BH
        or  ah,al               ; Goes into                  --AH- --AL-
        mov al,bl               ;                            <----AX--->
        shl al,04h
        or  al,bh
        mov word ptr [si],ax    ; store the value over the string
        
        mov ax,[si+6]           ; copy 3rd 2 chrs to ax, skip the 2nd space
        mov bx,[si+8]           ; copy 4th two characters to bx
        sub al,30h              ; subtract 30h so chrs 0-9 have value 0-9
        cmp al,09h              ; if its 0-9, its ok.
        jbe al_is_ok2           ; if its not, its probably A-F or a-f
        sub al,07h              ; so subtract 7 more
        and al,0Fh              ; and limit to 0-F
    al_is_ok2:
        sub ah,30h              ; do the same to ah
        cmp ah,09h
        jbe ah_is_ok2
        sub ah,07h
        and ah,0Fh
    ah_is_ok2:
        sub bl,30h              ; do the same to bl
        cmp bl,09h
        jbe bl_is_ok2
        sub bl,07h
        and bl,0Fh
    bl_is_ok2:
        sub bh,30h              ; do the same to bh
        cmp bh,09h
        jbe bh_is_ok2
        sub bh,07h
        and bh,0Fh
    bh_is_ok2:
        shl al,04h              ; Combine the values so that AL-AH-BL-BH
        or  ah,al               ; Goes into                  --AH- --AL-
        mov al,bl               ;                            <----AX--->
        shl al,04h
        or  al,bh
        mov word ptr [si+2],ax  ; store the value over the string
                             ; Now [si] contains the real values of AX and BX

        mov dx,offset tracing                      ; ds is preset
        mov ah,09h                                 ; Dos function 09h
        int 21h                                    ; Display tracing string
        mov ax,word ptr [si]                       ; Restore ax
        call ShowHex                               ; Display command line
        call ShowHex                               ;  ax value back to user
        call ShowHex                               ;  by placing it in ax
        call ShowHex                               ;  and calling ShowHex
        mov dx,offset bx_msg                       ; ds is preset
        mov ah,09h                                 ; Dos function 09h
        int 21h                                    ; Display bx message
        mov ax,word ptr [si+2]                     ; Restore bx into ax
        call ShowHex                               ; Display command line
        call ShowHex                               ;  bx value back to user
        call ShowHex                               ;  by placing it in ax
        call ShowHex                               ;  and calling ShowHex
        mov dx,offset header                       ; ds is preset
        mov ah,09h                                 ; Dos function 09h
        int 21h                                    ; Display header to output

        mov ax,3501h                ; Dos function 35h, Get vector of INT 01h
        int 21h                     ; Store it in es:bx
        mov word ptr [realINT1],bx   ; Store address of original INT 01h
        mov word ptr [realINT1+2],es ; into realINT1
        
        mov ax,3510h                ; Dos function 35h, Get vector of INT 10h
        int 21h                     ; Store it in es:bx
        mov word ptr [realINT10],bx  ; Store address of original INT 10h
        mov word ptr [realINT10+2],es ; into realINT10 so we can fake an INT

        mov ax,2501h               ; Dos function 25h, Store DS:DX to INT 01h
        mov dx,offset INT1         ; ds is preset, dx is the handler's offset
        int 21h                    ; Set new Single Step handler
        
        mov ax,2510h               ; Dos function 25h, Store DS:DX to INT 10h
        mov dx,offset INT10        ; ds is preset, dx is the handler's offset
        int 21h                    ; Set new Video Interrupt

        mov ax,word ptr [si]             ; We will use the command line ax/bx
        mov bx,word ptr [si+2]           ; values for the fake int call
        int 10h                          ; Call my int 10 which fakes the
                                         ;  real int 10 and traps it.

        mov ax,2501h               ; Dos function 25h, Store DS:DX to INT 01h
        mov dx,word ptr [realINT1] ; ds/dx are in realINT1
        push ds                    ; Save old ds
        push word ptr [realINT1+2] ; Put segment on stack
        pop ds                     ; Set ds to the segment
        int 21h                    ; Reset old Single Step handler
        pop ds                     ; Restore old ds

        mov ax,2510h               ; Dos function 25h, Store DS:DX to INT 10h
        mov dx,word ptr [realINT10] ; ds/dx are in realINT10
        push ds                     ; Save old ds
        push word ptr [realINT10+2] ; Put segment on stack
        pop ds                      ; Set ds to the segment
        int 21h                     ; Reset old Video Interrupt
        pop ds                      ; Restore old ds

        mov ax,0003h               ; Set ax to 3
        int 10h                    ; Set 80x25 Text mode
        ret                        ; End of program
end                                                             ; End of file
