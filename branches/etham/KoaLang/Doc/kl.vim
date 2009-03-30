" Vim syntax file
" Language:	KoaLang
" Maintainer:	
" Last Change:	2007 Feb 13

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	klStatement	break return
syn keyword	klConditional	if else switch
syn keyword     klLogical       not and or xor
syn keyword	klRepeat	while for do in until

syn match       klOperator      display "[-:+.%#<>,\\\*/=]"

syn match	klSpecial       display "{\|}"

syn region      klString        start="\"" end="\""

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match	klNumbers	display transparent "\<\d\|\.\d" contains=klNumber,klFloat,klOctalError,klOctal
" Same, but without octal error (for comments)
syn match	klNumbersCom	display contained transparent "\<\d\|\.\d" contains=klNumber,klFloat,klOctal
syn match	klNumber		display contained "\d\+\(u\=l\{0,2}\|ll\=u\)\>"
"hex number
syn match	klNumber		display contained "0x\x\+\(u\=l\{0,2}\|ll\=u\)\>"
" Flag the first zero of an octal number as something special
syn match	klOctal		display contained "0\o\+\(u\=l\{0,2}\|ll\=u\)\>" contains=klOctalZero
syn match	klOctalZero	display contained "\<0"
syn match	klFloat		display contained "\d\+f"
"floating point number, with dot, optional exponent
syn match	klFloat		display contained "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="
"floating point number, starting with a dot, optional exponent
syn match	klFloat		display contained "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match	klFloat		display contained "\d\+e[-+]\=\d\+[fl]\=\>"

" flag an octal number with wrong digits
syn match	klOctalError	display contained "0\o*[89]\d*"
syn case match

syn region klComment  start="/;" end=";/" extend
syn region klCommentL start=";" end="$" keepend

hi def link klOperator		Operator
hi def link klCommentL		Comment
hi def link klConditional	Conditional
hi def link klRepeat		Repeat
hi def link klStatement		Statement
hi def link klNumber		Number
hi def link klOctal		Number
hi def link klOctalZero		Error
hi def link klFloat		Float
hi def link klString		String
hi def link klComment		Comment
hi def link klSpecial		SpecialChar

let b:current_syntax = "kl"
