#ifndef CODEINTERPRETER_H
#define CODEINTERPRETER_H

#include <stack>

#include "mathvm.h"
#include "interpreter_context.h"

namespace mathvm
{   
    class CodeInterpreter : Code
    {
    public:
        CodeInterpreter();
        CodeInterpreter(const CodeInterpreter& other);
        virtual ~CodeInterpreter();
        virtual Status* execute(vector<Var*>& vars) override;
    
    private:
        void executeInstruction(Instruction instruction);
        bool tryLoad(Instruction instruction);
        bool tryArithmetic(Instruction instruction);
        bool tryLogic(Instruction instruction);
        bool tryPrint(Instruction instruction);
        bool tryConvert(Instruction instruction);
        
        StackElement popStack();
    
    private:
        InterpreterContext* _context;
        std::stack<StackElement> _stack;
        
    };
}



#endif /* CODEINTERPRETER_H */

