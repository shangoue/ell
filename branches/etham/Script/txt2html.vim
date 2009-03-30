" TXT2HTML, Samuel Hangouët
" updated 26 nov 2007
"
" espaces, first destroy <160> chars
%s/ / /ge
set expandtab
retab
%s/\s\+$//ge
w
" protection des sauts de lignes
"%s/\([^\\]\)\\\n/\1/ge
%s/^\\\n//ge
" commentaires
%s/\n\n\/\*\n\([^(].*\n\+\)*\*\//\r\r/ge
" ancres
%s/@@ *\(\S\+\) *|\(.*\)@@/<a name="\1">\2<\/a>/ge
%s/@@\(.\+\)@@/<a name="\1">\1<\/a>/ge
" sauts de lignes
%s/\\\\\n*/ <br \/>/ge
" gras
%s/\%( \|^\)\*\*\(\([^*]\+\n\)*[^*]\+\)\*\*\%( \|$\)/ <strong>\1<\/strong> /ge
" souligné
%s/\%( \|^\)__\(\%(\%(_\=[^_]\)\+\n\)*\%(_\=[^_]\)\+\)__\%( \|$\)/ <span style="text-decoration: underline">\1<\/span> /ge
" italique
%s/\%( \|^\)\/\/\(\%(\%(\/\=[^/]\)\+\n\)*\%(\/\=[^/]\)\+\)\/\/\%( \|$\)/ <span style="font-style: italic">\1<\/span> /ge
" images
%s/{{ *\([^|]*\)\(\.png\) *| *\(\%([^}]\|}[^}]\)*[^} ]\)\s*}}/<div style="text-align:center"><br \/><a name="\1" \/><br \/><img style="max-width:90%" src="\1\2" alt="\3" \/><br \>\3<br \><\/div>/ge
" liens
%s/{{ *\(\%(http\|ftp\)s\=:\/\/[^|]*[^ |]\) *| *\(\%([^}]\|}[^}]\)*[^} ]\)\s*}}/<a href="\1">\2<\/a>/ge
%s/{{ *\([^|]*[^ |]\) *| *\(\%([^}]\|}[^}]\)*[^} ]\)\s*}}/<a href="#\1">\2<\/a>/ge
%s/{{ *\(\%([^}]\|}[^}]\)\+\)\s*}}/<a href="#\1">\1<\/a>/ge
%s/\[\[\(\%([^\]]\|][^\]]\)\+\)]]/[<a href="#\1">\1<\/a>]/ge
%s/\([^"]\)\(http\|ftp\)\(s\=:\/\/[^<()\\| \t,;:]\+\)/\1<a href="\2\3">\2\3<\/a>/ge
" espaces insecables
%s/ :/\&nbsp;:/ge
" tableaux
%s/^\n|/\r<table>\r|/e
%s/|\n\n/|\r<\/table>\r\r/ge
%s/^\(|.*\)$/<tr>\r\1\r<\/tr>/e
%s/^|\(.*\)|/<td>\1<\/td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
%s/^\(<td>[^|]*\)|/\1<\/td><td>/e
" titres barres
%s/^====== *\(.*\S\) *======/<a name="\1" \/><h6>\1<\/h6>/e
%s/^===== *\(.*\S\) *=====/<a name="\1" \/><h5>\1<\/h5>/e
%s/^==== *\(.*\S\) *====/<a name="\1" \/><h4>\1<\/h4>/e
%s/^=== *\(.*\S\) *===/<a name="\1" \/><h3>\1<\/h3>/e
%s/^== *\(.*\S\) *==/<a name="\1" \/><h2>\1<\/h2>/e
%s/^= *\(.*\S\) *=/<a name="\1" \/><h1>\1<\/h1>/e
" titres num
%s/^\(\d\+\.\d\+\.\d\+\.\d\+\.\d\+\.\d\+\.\=\) \(.*\)$/<h6><a name="\1">\2<\/a><\/h6>/ge
%s/^\(\d\+\.\d\+\.\d\+\.\d\+\.\d\+\.\=\) \(.*\)$/<h5><a name="\1">\2<\/a><\/h5>/ge
%s/^\(\d\+\.\d\+\.\d\+\.\d\+\.\=\) \(.*\)$/<h4><a name="\1">\2<\/a><\/h4>/ge
%s/^\(\d\+\.\d\+\.\d\+\.\=\) \(.*\)$/<h3><a name="\1">\2<\/a><\/h3>/ge
%s/^\(\d\+\.\d\+\.\=\) \(.*\)$/<h2><a name="\1">\2<\/a><\/h2>/ge
%s/^\(\d\+\.\=\) \(.*\)$/<h1><a name="\1">\2<\/a><\/h1>/ge
" sous-titres
%s/^\* \+\(.*\S\)\s*:/<br \/><strong><a name="\1">\1<\/a><\/strong>\&nbsp;:/ge
" listes à puces
" <li> niveau 2
%s/^    [-*] \+\(.*\)\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\%(\n    \+\([^ *-].*\)\)\=\n\+/ <li>\1 \2 \3 \4 \5 \6 \7 \8 \9<\/li> \r/e
" <li> niveau 1
%s/^  [-*] \+\(.*\)\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\%(\n  \+\([^ *-].*\)\)\=\n\+/<li>\1 \2 \3 \4 \5 \6 \7 \8 \9<\/li>\r/e
"   <ul> niveau 1
%s/^\(\%([^ <]\| \=<[^l]\).*\)\=\n<li>/\1\r<ul>\r<li>/e
"   </ul> niveau 1
%s/\(<\/li> \=\)\n\([^ <]\| \=<[^l]\|$\)/\1\r<\/ul>\r\2/ge
"   <ul> niveau 2
%s/^\(\%(\S\| [^<]\| <[^l]\).*\)\=\n <li>/\1\r<ul>\r <li>/e
"   </ul> niveau 2
%s/\(<\/li> \)\n\(\S\| [^<]\| <[^l]\)/\1\r<\/ul>\r\2/ge
" barres
%s/^----\+$/<hr \/>/ge
" chapeaux
%s/^((\n\(\%(\%([^)]\|)[^)]\)\+\n\)*\)))$/<p class="chap">\1<\/p>/ge
" paragraphes preformates
%s/^\n\(  .*\%(\n\+  .*\)*\)/\r<pre class="bordure">\1<\/pre>/ge
" paragraphes
%s/\n\n\(..*\(\n..*\)*\)/\r<p>\r\1\r<\/p>/ge
" TODO
%s/TODO/<span style="color:red;font-weight:bold">TODO<\/span>/ge
" en-tete et pied de page
norm 1GO<?xml version="1.0" encoding="UTF-8" ?>
norm o<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
norm o<html xmlns="http://www.w3.org/1999/xhtml" lang="fr" xml:lang="fr">
norm o<head><meta http-equiv="content-type" content="text/html; charset=UTF-8" /><title>
norm "%pbhxdwA</title><style>
norm o img {border: 0; margin: 1pt}
norm o table {border-collapse: collapse}
norm o .bordure, td, th {border: gray 1px solid; padding: 5pt}
norm o hr {clear: both}
norm o body {text-align: justify; font-family: Verdana, sans-serif; font-size: 10pt; margin: 12pt;}
norm o .chap {margin-left: 24pt; font-style: italic}
norm o .ret {text-indent: 24pt}
norm o p {margin-bottom: 12pt} pre p {margin: 0}
norm o</style></head><body>
norm Go</body></html>
sav! %:r.html
b#
