//
// ScriptOpcode.h
//

// stack
SCRIPT_OPCODE( OPCODE_PUSH_VAR           , 2 ) // subcode:var_index	
SCRIPT_OPCODE( OPCODE_PUSH_VAR_INDEXED   , 2 ) // subcode:var_index		stack is offset from var
SCRIPT_OPCODE( OPCODE_PUSH_INT           , 3 ) // subcode:none			[ int ]
SCRIPT_OPCODE( OPCODE_PUSH_FLOAT         , 3 ) // subcode:none			[ float ]
SCRIPT_OPCODE( OPCODE_PUSH_STRING        , 2 ) // subcode:none
SCRIPT_OPCODE( OPCODE_POP                , 1 ) // subcode:none
SCRIPT_OPCODE( OPCODE_POP_AND_STORE      , 2 ) // subcode:var index
SCRIPT_OPCODE( OPCODE_STORE              , 2 ) // subcode:var_index		store stack top in variable, no pop.
SCRIPT_OPCODE( OPCODE_STORE_INDEXED      , 2 ) // subcode:var_index		stack[-2]=var, stack[-1]=index, index popped off
SCRIPT_OPCODE( OPCODE_DUP                , 1 ) // subcode:none         :: stack = stack[-1]
SCRIPT_OPCODE( OPCODE_SWAP               , 2 ) // subcode:none         :: swaps two specified stack elements ( 0=top, 1=first from top, ... )
SCRIPT_OPCODE( OPCODE_PUSH_OWNER         , 1 ) // subcode:none       :: push the vm->Owner onto the stack

// compare
SCRIPT_OPCODE( OPCODE_LESS_THAN          , 1 ) // subcode:none			:: stack = ( stack[-2] <  stack[-1] )
SCRIPT_OPCODE( OPCODE_LESS_THAN_EQ       , 1 ) // subcode:none			:: stack = ( stack[-2] <= stack[-1] )
SCRIPT_OPCODE( OPCODE_GTR_THAN           , 1 ) // subcode:none			:: stack = ( stack[-2] >  stack[-1] )
SCRIPT_OPCODE( OPCODE_GTR_THAN_EQ        , 1 ) // subcode:none			:: stack = ( stack[-2] >= stack[-1] )
SCRIPT_OPCODE( OPCODE_EQUAL              , 1 ) // subcode:none			:: stack = ( stack[-2] == stack[-1] )
SCRIPT_OPCODE( OPCODE_NOT_EQUAL          , 1 ) // subcode:none			:: stack = ( stack[-2] != stack[-1] )

// arithmetic
SCRIPT_OPCODE( OPCODE_ADD                , 1 ) // subcode:none			:: stack = ( stack[-2] + stack[-1] )
SCRIPT_OPCODE( OPCODE_SUB                , 1 ) // subcode:none			:: stack = ( stack[-2] - stack[-1] )
SCRIPT_OPCODE( OPCODE_MULT               , 1 ) // subcode:none			:: stack = ( stack[-2] * stack[-1] )
SCRIPT_OPCODE( OPCODE_DIV                , 1 ) // subcode:none			:: stack = ( stack[-2] / stack[-1] )
SCRIPT_OPCODE( OPCODE_MOD                , 1 ) // subcode:none			:: stack = ( stack[-2] % stack[-1] )
SCRIPT_OPCODE( OPCODE_NEGATE             , 1 ) // subcode:none			:: stack = ( - stack[-1] )
SCRIPT_OPCODE( OPCODE_ABS                , 1 ) // subcode:none			:: stack = ( abs( stack[-1] ) )
SCRIPT_OPCODE( OPCODE_SHIFT_L            , 1 ) // subcode:none			:: stack = ( stack[-2] << stack[-1] )
SCRIPT_OPCODE( OPCODE_SHIFT_R            , 1 ) // subcode:none			:: stack = ( stack[-2] >> stack[-1] )
SCRIPT_OPCODE( OPCODE_LOGICAL_AND        , 1 ) // subcode:none			:: stack = ( stack[-2] && stack[-1] )
SCRIPT_OPCODE( OPCODE_LOGICAL_OR         , 1 ) // subcode:none			:: stack = ( stack[-2] || stack[-1] )
SCRIPT_OPCODE( OPCODE_LOGICAL_NOT        , 1 ) // subcode:none			:: stack = ( ! stack[-1] )
SCRIPT_OPCODE( OPCODE_BITWISE_AND        , 1 ) // subcode:none			:: stack = ( stack[-2] & stack[-1] )
SCRIPT_OPCODE( OPCODE_BITWISE_OR         , 1 ) // subcode:none			:: stack = ( stack[-2] | stack[-1] )
SCRIPT_OPCODE( OPCODE_BITWISE_XOR        , 1 ) // subcode:none			:: stack = ( stack[-2] ^ stack[-1] )
SCRIPT_OPCODE( OPCODE_BITWISE_NOT        , 1 ) // subcode:none			:: stack = ( ~ stack[-1] )

// jumps
SCRIPT_OPCODE( OPCODE_BRANCH_TRUE        , 2 ) // subcode:offset			:: jump taken if stack[-1] true
SCRIPT_OPCODE( OPCODE_BRANCH_FALSE       , 2 ) // subcode:offset			:: jump taken if stack[-1] false
SCRIPT_OPCODE( OPCODE_BRANCH_TRUE_NO_POP , 2 ) // subcode:offset			:: jump taken if stack[-1] true, leaves stack[-1] in place
SCRIPT_OPCODE( OPCODE_BRANCH_FALSE_NO_POP, 2 ) // subcode:offset			:: jump taken if stack[-1] false, leaves stack[-1] in place
SCRIPT_OPCODE( OPCODE_JUMP               , 2 ) // subcode:offset			:: jump taken.
SCRIPT_OPCODE( OPCODE_CALL               , 2 ) // subcode:offset			:: PCstack[-1] = PC, jump to offset
SCRIPT_OPCODE( OPCODE_CALL_C             , 2 ) // subcode:c func #		:: C func consumes parm stack, returns parms
SCRIPT_OPCODE( OPCODE_RETURN             , 1 ) // subcode:none			   :: PC = PCstack[-1]
SCRIPT_OPCODE( OPCODE_SWITCH             , 2 ) // subcode:switch table # :: consumes parm stack for switched label

// flow control
SCRIPT_OPCODE( OPCODE_END                , 1 ) // subcode:none
SCRIPT_OPCODE( OPCODE_SLEEP              , 1 ) // subcode:none         :: sleep number of seconds pushed on stack 

// debug
SCRIPT_OPCODE( OPCODE_PRINT              , 1 ) // subcode:none			:: uses last stack, prints value

// do nothing
SCRIPT_OPCODE( OPCODE_TARGET             , 0 ) // dummy opcode, target of branch
SCRIPT_OPCODE( OPCODE_NOP                , 0 ) // dummy
