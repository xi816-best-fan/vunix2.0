xui:
  ld r1, [5]               ; 0 0 1   r0+r0+n
  ld r2, [r3]              ; 0 1 0   r1+r0+0
  ld r3, [r3 + 5]          ; 0 1 1   r1+r0+n
  ld r4, [r3 + r4]         ; 1 1 0   r1+r2+0
  ld r5, [r3 + r4 + 5]     ; 1 1 1   r1+r2+n

