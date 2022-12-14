; /***********************************************************************/
; /* Copyright (c) 2022 Clay Kress                                       */
; /*                                                                     */
; /* This file is part of AzulHW.                                        */
; /* AzulHW is free software: you can redistribute it and/or modify it   */
; /* under the terms of the GNU General Public License as published by   */
; /* the Free Software Foundation, either version 3 of the License, or   */
; /* (at your option) any later version.                                 */
; /*                                                                     */
; /* AzulHW is distributed in the hope that it will be useful,           */
; /* but WITHOUT ANY WARRANTY; without even the implied warranty         */
; /* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             */
; /*                                                                     */
; /* See the GNU General Public License for more details.                */
; /*                                                                     */
; /* You should have received a copy of the GNU General Public License   */
; /* along with AzulHW. If not, see <https://www.gnu.org/licenses/>.     */
; /*                                                                     */
; /***********************************************************************/

portB= $6000
portA= $6001
ddrB= $6002
ddrA= $6003

E = %10000000
RW= %01000000
RS= %00100000

  .org $8000

reset:
  lda #$ff
  sta ddrB

  lda #$ff
  sta ddrA

; Set 8-bit mode, 2-line display, 5x8 font
  lda #%00111000  ; Command
  sta portB
  lda #$00  ; Clear E/RW/RS bits
  sta portA
  lda #E    ; Send Toggle
  sta portA
  lda #$00  ; Clear E/RW/RS bits
  sta portA

; Set Display on, Cursor on, Blink off
  lda #%00001110  ; Command
  sta portB
  lda #$00  ; Clear E/RW/RS bits
  sta portA
  lda #E    ; Send Toggle
  sta portA
  lda #$00  ; Clear E/RW/RS bits
  sta portA

; Set Increment cursor, Don't shift display
  lda #%00000110  ; Command
  sta portB
  lda #$00  ; Clear E/RW/RS bits
  sta portA
  lda #E    ; Send Toggle
  sta portA
  lda #$00  ; Clear E/RW/RS bits
  sta portA

; Clear display
  lda #%00000001  ; Command
  sta portB
  lda #$00  ; Clear E/RW/RS bits
  sta portA
  lda #E    ; Send Toggle
  sta portA
  lda #$00  ; Clear E/RW/RS bits
  sta portA

; Write the first letter
  lda #"H"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"e"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"l"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"l"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"o"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #","
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #" "
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"W"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"o"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"r"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"l"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"d"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

; Write the first letter
  lda #"!"
  sta portB
  lda #RS
  sta portA
  lda #(RS | E)
  sta portA
  lda #RS
  sta portA

end:
  jmp end

;  .org $fffc
;  .word reset
;  .word $0000