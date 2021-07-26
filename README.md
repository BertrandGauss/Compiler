# Compiler
一个基本的编译原理项目，包括：<br>
1.词法分析<br>
2.语法分析(生成了语法分析树）<br>
3.语义分析（包括了语法制导翻译以及中间代码生成生成了抽象语法树）<br>
4.目标代码生成（生成的MIPS指令，可以在MARS仿真程序上成功运行）<br>

一、词法分析<br>
输入源程序符号串，输出记号流<br>
词法编码表为：<br>
|词法单元|	词法编码|	词法单元|	词法编码|
|  ----  | ----  |  ----  | ----  |
|int|	1|	=|	20|
|if	|2	|==	|21|
|while|	3|	>|	22|
|get|	4|	<|	23|
|put|	5|	<>|	24|
|else|	6|	>=|	25|
|bool|	7|	<=|	26|
|true|	8|	&&|	27|
|false|9	|\|\||	28|
|,	|10|	!|	29|
|(	|11|	&|	30|
|)	|12|	\||	31|
|{	|13|	* |	32|
|}	|14|	/|	33|
|[	|15|	%|	34|
|]|	16|	id|	35|
|;|	17|	digits16|	36|
|+|	18|	digits8|	37|
|-|	19|	digits|	38|
|return	|39|		
<br>
二、语法分析<br>
文法：<br>
Program -> FunctionDefinition_or_DeclarationStatement Program<br>
Program -> AssignmentFunction Program<br>
Program -> whileLoop Program<br>
Program -> ConditionalStatement Program<br>
Program -> get ( id variableClosure ) ; Program<br>
Program -> put ( id variableClosure ) ; Program<br>
Program -> ε<br>
expression -> factors  term<br>
factors -> factor factorRecursion <br>
factorRecursion -> * factor  factorRecursion<br>
factorRecursion -> / factor  factorRecursion<br>
factorRecursion -> % factor  factorRecursion<br>
term -> + factors term<br>
term -> - factors term
term -> bitwiseOperators factors term<br>
term -> ε<br>
factorRecursion -> ε<br>
factor  -> ( expression )<br>
factor  -> variable<br>
factor  -> number<br>
variable -> id array<br>
array -> [ expression ]<br>
array -> ε<br>
variableClosure -> , variable variableClosure<br>
variableClosure -> ε<br>
FunctionDefinition_or_DeclarationStatement -> type id functionDefinition_or_Declaration<br>
functionDefinition_or_Declaration -> ( defineParameters ) { Program returnStatement } ;<br>
returnStatement -> return id ;<br>
functionDefinition_or_Declaration -> array variableClosure ;<br>
defineParameters -> type id defineParametersClosure <br>
defineParametersClosure -> , type id defineParametersClosure<br>
defineParametersClosure -> ε<br>
AssignmentFunction -> id Assignment_or_FunctionCall <br>
Assignment_or_FunctionCall  -> array = rightValue ; <br>
Assignment_or_FunctionCall  -> ( ParametersList ) ;<br>
rightValue -> expression  <br>
rightValue -> Boolean <br>
Boolean -> true<br>
Boolean -> false<br>
ParametersList -> Parameter parameterClosure<br>
parameterClosure -> , Parameter parameterClosure<br>
parameterClosure -> ε<br>
Parameter -> id<br>
Parameter -> number<br>
Parameter -> Boolean<br>
relation_or_logicalExpression -> ! rightValue<br>
relation_or_logicalExpression -> rightValue relation_or_logicalOperator M rightValue<br>
relation_or_logicalOperator -> logicalOperators<br>
relation_or_logicalOperator -> relationalOperators <br>
bitwiseOperators -> &<br>
logicalOperators -> &&<br>
bitwiseOperators -> |<br>
logicalOperators -> ||<br>
relationalOperators -> <<br>
relationalOperators -> ><br>
relationalOperators -> ==<br>
relationalOperators -> <><br>
relationalOperators -> >=<br>
relationalOperators -> <=<br>
whileLoop -> while ( M relation_or_logicalExpression ) { M Program } ;<br>
ConditionalStatement ->  if ( relation_or_logicalExpression ) { M Program } ; otherwiseStatement<br>
otherwiseStatement -> else { M Program } ;<br>
otherwiseStatement -> ε<br>
type -> int<br>
type -> bool<br>
number -> digits<br>
number -> digits8<br>
number -> digits16<br>
M -> ε<br>
三、语义分析<br>
设计翻译方案并生成了中间代码，中间代码为四元式形式的三地址码。<br>
四、目标代码生成<br>
将中间代码翻译成能在Mars仿真程序上正确运行的MIPS形式的汇编指令。

