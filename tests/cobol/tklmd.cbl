       identification division.
       program-id. tklmd.
      *
      * compute the SHA-256 hash of nothing
      *
      * To compile (31-bit):
      *
      * $ cob2 -otklmd tklmd.cbl
      *
      * To run (31-bit):
      *
      * $ STEPLIB=MACHO.PDSE ./tklmd
      *
      * To compile (64-bit):
      *
      * $ cob2 -q64 -qdll -otklmd tklmd.cbl
      *
      * To run (64-bit):
      *
      * $ STEPLIB=MACHO.PDSE64 ./tklmd
      *
       environment division.
       data division.
       working-storage section.
      *
      * initial hash values for SHA-256
      *
       01  initial-hash.
           05  h0    pic x(4)   value X'6A09E667'.
           05  h1    pic x(4)   value X'BB67AE85'.
           05  h2    pic x(4)   value X'3C6EF372'.
           05  h3    pic x(4)   value X'A54FF53A'.
           05  h4    pic x(4)   value X'510E527F'.
           05  h5    pic x(4)   value X'9B05688C'.
           05  h6    pic x(4)   value X'1F83D9AB'.
           05  h7    pic x(4)   value X'5BE0CD19'.
       01  klmd-fn   pic x(8)   value "KLMD".
       01  klmd3-fn  pic x(8)   value "KLMD3".
       01  gr1.
           05  hash  pic x(32).
           05  mbl   pic 9(18)  comp-5.
       01  r1        pic x(1).
       01  pr1       pointer.
       01  r2        pic x(1).
       01  pr2       pointer.
       01  ticks     pic 9(18)  comp-5.
      *
      * register variables dependent on addressing mode
      *
       >>IF IGY-LP = 64
       01  gr0       pic 9(18)  comp-5.
       01  r1plus1   pic 9(18)  comp-5.
       01  r2plus1   pic 9(18)  comp-5.
       01  cc        pic 9(18)  comp-5.
       01  invokes   pic 9(18)  comp-5.
       >>ELSE
       01  gr0       pic 9(9)   comp-5.
       01  r1plus1   pic 9(9)   comp-5.
       01  r2plus1   pic 9(9)   comp-5.
       01  cc        pic 9(9)   comp-5.
       01  invokes   pic 9(9)   comp-5.
       >>END-IF
       procedure division.
      *
      * SHA-256 of nothing with CC loop exposed
      *
           set pr1 to address of r1
           set pr2 to address of r2
           move 2 to gr0
           move initial-hash to hash
           move 0 to mbl
           move 0 to r2plus1
           call klmd-fn using gr0 gr1 pr1 r1plus1 pr2 r2plus1
                              cc ticks
           if cc = 0 then
               display "SHA-256(''): " function hex-of(hash)
                  "  in " ticks " ticks"
           else
               display "KLMD failed. CC=" cc
           end-if
      *
      * SHA-256 of nothing with CC loop hidden
      *
           move 2 to gr0
           move initial-hash to hash
           move 0 to mbl
           move 0 to r2plus1
           move 3 to cc
           perform until cc not equal 3
               call klmd3-fn using gr0 gr1 r1 r1plus1 r2 r2plus1
                                   cc invokes ticks
           end-perform
           if cc = 0 then
               display "SHA-256(''): " function hex-of(hash)
                  "  in " ticks " ticks"
           else
               display "KLMD3 failed. CC=" cc
           end-if

           stop run.