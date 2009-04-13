
OBJ(Abstract)
OBJ(Real)
OBJ(String)
OBJ(Variable)
OBJ(List)
OBJ(Block)
OBJ(Map)


#ifndef BIN
# define BIN(ID, OP) OBJ(ID)
#endif

BIN(Assign, =)     
BIN(And, and)      
BIN(Or, or)        
BIN(Xor, xor)      
BIN(Eq, ==)        
BIN(Neq, !=)       
BIN(Le, <=)        
BIN(Ge, >=)        
BIN(Lt, <)         
BIN(Gt, >)         
BIN(Add, +)        
BIN(Sub, -)        
BIN(Mult, *)       
BIN(Div, /)        
BIN(Mod, %)

#undef BIN
#undef OBJ
