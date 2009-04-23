" Vim syntax file
" Language:     kl

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" A bunch of useful kl keywords
syn keyword klKeyword    if else while do for in break print return or xor and not

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match       klNumbers       display transparent "\<\d\|\.\d" contains=klNumber,klFloat,klOctalError,klOctal
" Same, but without octal error (for comments)
syn match       klNumbersCom    display contained transparent "\<\d\|\.\d" contains=klNumber,klFloat,klOctal
syn match       klNumber        display contained "\d\+\(u\=l\{0,2}\|ll\=u\)\>"
"hex number
syn match       klNumber        display contained "0x\x\+\(u\=l\{0,2}\|ll\=u\)\>"
" Flag the first zero of an octal number as something special
syn match       klOctal         display contained "0\o\+\(u\=l\{0,2}\|ll\=u\)\>" contains=klOctalZero
syn match       klOctalZero     display contained "\<0"
syn match       klFloat         display contained "\d\+f"
"floating point number, with dot, optional exponent
syn match       klFloat         display contained "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="
"floating point number, starting with a dot, optional exponent
syn match       klFloat         display contained "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match       klFloat         display contained "\d\+e[-+]\=\d\+[fl]\=\>"
"hexadecimal floating point number, optional leading digits, with dot, with exponent
syn match       klFloat         display contained "0x\x*\.\x\+p[-+]\=\d\+[fl]\=\>"
"hexadecimal floating point number, with leading digits, optional dot, with exponent
syn match       klFloat         display contained "0x\x\+\.\=p[-+]\=\d\+[fl]\=\>"

" flag an octal number with wrong digits
syn match       klOctalError    display contained "0\o*[89]\d*"
syn case match

syn region      klString        start=+L\="+ skip=+\\\\\|\\"\|\\$+ excludenl end=+"+ end='$'


syn region      klComment       start="'" skip="\\$" end="$" keepend

syn match       klDefine        display "\%(<\%(\w\|\s\)*>\)\=\s*\w\+\s*\%(<\%(\w\|\s\)*>\)\=:"me=e-1


hi def link klKeyword Keyword
hi def link klNumbers Number
hi def link klString String
hi def link klComment Comment
hi def link klDefine Label

let b:current_syntax = "koalang"
