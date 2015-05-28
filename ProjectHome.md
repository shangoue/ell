Ell is pure-header library to write EBNF grammars as C++ code. It eases the development of parsers or similar applications, while removing the need to write a lexer.

Ell is not a tool to generate parsers (like [ANTLR](http://www.antlr.org/)) : the grammar you write is directly embedded into your C++ code. The core library is light, headers-only and written using generation templates technique to achieve a fastest execution.

The service provided by Ell is very similar to what [Boost Spirit](http://spirit.sourceforge.net) provides, but with a simpler object model and without the need for Boost library. It also provides additional features, such as the writing of grammar with context-dependent skipper.

For more information, look at the [documentation](http://code.google.com/p/ell/wiki/ReferenceManualEn).

Example programs feature a tight and fast [XML parser](http://code.google.com/p/ell/source/browse/trunk/XmlParser/Include/ell/XmlParser.h).

ELL has been tested using GCC 4.x (linux) and visual studio 2010/2012 (windows 7).