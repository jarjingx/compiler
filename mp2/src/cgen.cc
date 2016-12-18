// 代码生成的主体代码部分

#define EXTERN
#include "cgen.h"
#include <string>
#include <sstream>


extern int cgen_debug;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.  Feel free to add your
// own definitions as you see fit.
//
//////////////////////////////////////////////////////////////////////
EXTERN Symbol 
	// required classes
	Object,
	IO,
	String,
	Int,
	Bool,
	Main,

	// class methods
	cool_abort,
	type_name,
	cool_copy,
	out_string,
	out_int,
	in_string,
	in_int,
	length,
	concat,
	substr,

	// class members
	val,

	// special symbols
	No_class,    // symbol that can't be the name of any user-defined class
	No_type,     // If e : No_type, then no code is generated for e.
	SELF_TYPE,   // Special code is generated for new SELF_TYPE.
	self,        // self generates code differently than other references

	// extras
	arg,
	arg2,
	prim_string,
	prim_int,
	prim_bool;


//********************************************************
//
// PREDEFINED FUNCTIONS:
//
// The following functions are already coded, you should
// not need to modify them, although you may if necessary.
//
//********************************************************

//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
	Object      = idtable.add_string("Object");
	IO          = idtable.add_string("IO");
	String      = idtable.add_string("String");
	Int         = idtable.add_string("Int");
	Bool        = idtable.add_string("Bool");
	Main        = idtable.add_string("Main");

	cool_abort  = idtable.add_string("abort");
	type_name   = idtable.add_string("type_name");
	cool_copy   = idtable.add_string("copy");
	out_string  = idtable.add_string("out_string");
	out_int     = idtable.add_string("out_int");
	in_string   = idtable.add_string("in_string");
	in_int      = idtable.add_string("in_int");
	length      = idtable.add_string("length");
	concat      = idtable.add_string("concat");
	substr      = idtable.add_string("substr");

	val         = idtable.add_string("val");

	No_class    = idtable.add_string("_no_class");
	No_type     = idtable.add_string("_no_type");
	SELF_TYPE   = idtable.add_string("SELF_TYPE");
	self        = idtable.add_string("self");

	arg         = idtable.add_string("arg");
	arg2        = idtable.add_string("arg2");
	prim_string = idtable.add_string("sbyte*");
	prim_int    = idtable.add_string("int");
	prim_bool   = idtable.add_string("bool");
}
//===============================================================================================================

// 该方法将被编译器的驱动程序 cgen-phase.cc 所调用
// 它将接收到的输出流和其名下所有的类传递给 CgenClassTable 的构造函数
// 构造函数完成所有的代码生成工作
void program_class::cgen(ostream &os) 
{
	initialize_constants();		// 执行上面的那个常数初始化的函数
	class_table = new CgenClassTable(classes,os);		// 将类和输出流传递给 CgenClassTable 的构造函数
}


// 为所有的字符串常量创建定义代码
void StrTable::code_string_table(ostream& s, CgenClassTable* ct)
{
	for (List<StringEntry> *l = tbl; l; l = l->tl()) {
		l->hd()->code_def(s, ct);
	}
}

// 为所有的整型常量创建定义代码
void IntTable::code_string_table(ostream& s, CgenClassTable* ct)
{
	for (List<IntEntry> *l = tbl; l; l = l->tl()) {
		l->hd()->code_def(s, ct);
	}
}

//
// Sets up declarations for extra functions needed for code generation
// You should not need to modify this code for MP2.1
//
void CgenClassTable::setup_external_functions()		// 该函数的实现可以作为代码生成的参照！！！   cankao
{
	ValuePrinter vp;
	// setup function: external int strcmp(sbyte*, sbyte*)
	op_type i32_type(INT32), i8ptr_type(INT8_PTR), vararg_type(VAR_ARG);

	vector<op_type> strcmp_args;
	strcmp_args.push_back(i8ptr_type);
	strcmp_args.push_back(i8ptr_type);	
	vp.declare(*ct_stream, i32_type, "strcmp", strcmp_args); 

	// setup function: external int printf(sbyte*, ...)
	vector<op_type> printf_args;
	printf_args.push_back(i8ptr_type);
	printf_args.push_back(vararg_type);
	vp.declare(*ct_stream, i32_type, "printf", printf_args);

	// setup function: external void abort(void)
	op_type void_type(VOID);
	vector<op_type> abort_args;
	vp.declare(*ct_stream, void_type, "abort", abort_args);

	// setup function: external i8* malloc(i32)
	vector<op_type> malloc_args;
	malloc_args.push_back(i32_type);
	vp.declare(*ct_stream, i8ptr_type, "malloc", malloc_args);
}

// Creates AST nodes for the basic classes and installs them in the class list
void CgenClassTable::install_basic_classes()
{
	// The tree package uses these globals to annotate the classes built below.
	curr_lineno = 0;
	Symbol filename = stringtable.add_string("<basic class>");

	//
	// A few special class names are installed in the lookup table but not
	// the class list. Thus, these classes exist, but are not part of the
	// inheritance hierarchy.
	 
	// No_class serves as the parent of Object and the other special classes.
	Class_ noclasscls = class_(No_class,No_class,nil_Features(),filename);
	install_special_class(new CgenNode(noclasscls, CgenNode::Basic, this));
	delete noclasscls;

	Class_ primintcls = class_(prim_int,No_class,nil_Features(),filename);
	install_special_class(new CgenNode(primintcls, CgenNode::Basic, this));
	delete primintcls;
	Class_ primboolcls = class_(prim_bool,No_class,nil_Features(),filename);
	install_special_class(new CgenNode(primboolcls, CgenNode::Basic, this));
	delete primboolcls;
	// 
	// The Object class has no parent class. Its methods are
	//        cool_abort() : Object   aborts the program
	//        type_name() : Str       returns a string representation of class name
	//        copy() : SELF_TYPE      returns a copy of the object
	//
	// There is no need for method bodies in the basic classes---these
	// are already built in to the runtime system.
	//
	Class_ objcls =
		class_(Object, 
		       No_class,
		       append_Features(
		       append_Features(
		       single_Features(method(cool_abort, nil_Formals(), 
		                              Object, no_expr())),
		       single_Features(method(type_name, nil_Formals(),
		                              String, no_expr()))),
		       single_Features(method(cool_copy, nil_Formals(), 
		                              SELF_TYPE, no_expr()))),
		       filename);
	install_class(new CgenNode(objcls, CgenNode::Basic, this));
	delete objcls;

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
	Class_ intcls=
		class_(Int, 
		       Object,
		       single_Features(attr(val, prim_int, no_expr())),
		       filename);
	install_class(new CgenNode(intcls, CgenNode::Basic, this));
	delete intcls;

//
// Bool also has only the "val" slot.
//
	Class_ boolcls=
		class_(Bool,  
		       Object, 
		       single_Features(attr(val, prim_bool, no_expr())),
		       filename);
	install_class(new CgenNode(boolcls, CgenNode::Basic, this));
	delete boolcls;
}

//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_classes(Classes cs)
{
	for (int i = cs->first(); cs->more(i); i = cs->next(i)) {
		install_class(new CgenNode(cs->nth(i),CgenNode::NotBasic,this));
	}
}

// 
// Add this CgenNode to the class list and the lookup table
// 
void CgenClassTable::install_class(CgenNode *nd)
{
	Symbol name = nd->get_name();

	if (probe(name))
		return;

	// The class name is legal, so add it to the list of classes
	// and the symbol table.
	nds = new List<CgenNode>(nd,nds);
	addid(name,nd);
}

// 
// Add this CgenNode to the special class list and the lookup table
// 
void CgenClassTable::install_special_class(CgenNode *nd)
{
	Symbol name = nd->get_name();

	if (probe(name))
		return;

	// The class name is legal, so add it to the list of special classes
	// and the symbol table.
	special_nds = new List<CgenNode>(nd, special_nds);
	addid(name,nd);
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
	for(List<CgenNode> *l = nds; l; l = l->tl())
		set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNode *nd)
{
	CgenNode *parent_node = probe(nd->get_parent());
	nd->set_parentnd(parent_node);
	parent_node->add_child(nd);
}

// Get the root of the class tree.
CgenNode *CgenClassTable::root()
{
	return probe(Object);
}

//////////////////////////////////////////////////////////////////////
//
// Special-case functions used for the method Int Main::main() for
// MP2-1 only.
//
//////////////////////////////////////////////////////////////////////

CgenNode* CgenClassTable::getMainmain(CgenNode* c)
{
	if (c && ! c->basic())
		return c;                   // Found it!

	List<CgenNode> *children = c->get_children();
	for (List<CgenNode> *child = children; child; child = child->tl()) {
		if (CgenNode* foundMain = this->getMainmain(child->hd()))
			return foundMain;   // Propagate it up the recursive calls
	}

	return 0;                           // Make the recursion continue
}

//-------------------------------------------------------------------
//
// END OF PREDEFINED FUNCTIONS
//
//-------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
//
// coding string, int, and boolean constants
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type stringEntry.  stringEntry methods are defined both for string
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Create global definitions for constant Cool objects
//
void CgenClassTable::code_constants()
{
}

// generate code to define a global string constant
void StringEntry::code_def(ostream& s, CgenClassTable* ct)
{
}

// generate code to define a global int constant
void IntEntry::code_def(ostream& s, CgenClassTable* ct)
{
	// Leave this method blank, since we are not going to use global
	// declarations for int constants.
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//
// CgenClassTable constructor orchestrates all code generation
//
CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(0)
{
	if (cgen_debug) std::cerr << "Building CgenClassTable" << endl;		// 在 debug 模式下输出运行信息
	ct_stream = &s;		// 取输出流的地址
	// Make sure we have a scope, both for classes and for constants
	enterscope();

	// 创建继承树，每个 CgenNode 代表一个 class
	install_basic_classes();	// 加载基类
	install_classes(classes);	// 加载所有类
	build_inheritance_tree();	// 创建继承树

	// First pass	还没看懂究竟做了些啥
	setup();

	// Second pass
	code_module();
	// Done with code generation: exit scopes
	exitscope();

}

CgenClassTable::~CgenClassTable()
{
}

// The code generation first pass.  Define these two functions to traverse
// the tree and setup each CgenNode
void CgenClassTable::setup()
{
	setup_external_functions();	// 生成 strcmp、printf、abort、malloc 四个函数的声明代码
	setup_classes(root(), 0);
}

void CgenClassTable::setup_classes(CgenNode *c, int depth)
{
	// MAY ADD CODE HERE
	// if you want to give classes more setup information

	c->setup(current_tag++, depth);
	List<CgenNode> *children = c->get_children();
	for (List<CgenNode> *child = children; child; child = child->tl())
		setup_classes(child->hd(), depth + 1);		// 递归处理
	
	c->set_max_child(current_tag-1);

	
	//if (cgen_debug)
	//	std::cerr << "Class " << c->get_name() << " assigned tag "
	//		<< c->get_tag() << ", max child " << c->get_max_child()
	//		<< ", depth " << c->get_depth() << endl;
	
}


// The code generation second pass. Add code here to traverse the tree and		
// emit code for each CgenNode
void CgenClassTable::code_module()
{
	code_constants();

	// This must be after code_module() since that emits constants
	// needed by the code() method for expressions
	CgenNode* mainNode = getMainmain(root());
	mainNode->codeGenMainmain( *ct_stream );
	code_main();
}

//
// Create LLVM entry point. This function will initiate our Cool program 
// by generating the code to execute (new Main).main()
//
void CgenClassTable::code_main()
{
	ValuePrinter vp( *ct_stream );

    // Declare a string constant
    string str("Main.main() returned %d\n");
    op_arr_type op_type_array(INT8, str.length()+1 );
    op_arr_type i8_arr_type( INT8, 25 );
    const_value strConst( op_type_array, str, false );
    vp.init_constant("main.printout.str", strConst);

    // Define a function main that has no parameters and returns an i32
	vp.define( INT32, "main", vector<operand>() );
	
	// Define an entry basic block
	vp.begin_block( "entry" );

	// Call Main_main(). This returns int* for phase 1, Object for phase 2
	operand res1( vp.call( vector<op_type>(), INT32, "Main_main", 1, vector<operand>() ) );

	// Get the address of the string "Main_main() returned %d\n" using
	// getelementptr
    op_arr_type i8_ptr_arr_type( INT8_PTR, 25 );
	global_value op1( i8_ptr_arr_type, "main.printout.str" );
	operand res2( vp.getelementptr( i8_arr_type, op1, int_value(0), int_value(0), INT8_PTR ) );

	// Call printf with the string address of "Main_main() returned %d\n"
	// and the return value of Main_main() as its arguments
	op_type i8ptr_type( INT8_PTR ), vararg_type( VAR_ARG );
	vector<op_type> printf_args1;
	printf_args1.push_back( INT8_PTR );
	printf_args1.push_back( VAR_ARG );
	vector<operand> printf_args2;
    printf_args2.push_back( res2 );
	printf_args2.push_back( res1 );
	vp.call( printf_args1, INT32, "printf", 1, printf_args2 );

	// Insert return 0
	vp.ret( int_value( 0 ) );

	vp.end_define( );
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTable *ct)
: class__class((const class__class &) *nd), 
  parentnd(0), children(0), basic_status(bstatus), class_table(ct), tag(-1)
{ 
	// ADD CODE HERE
}

void CgenNode::add_child(CgenNode *n)
{
	children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNode *p)
{
	assert(parentnd == NULL);
	assert(p != NULL);
	parentnd = p;
}

//
// Class setup.  You may need to add parameters to this function so that
// the classtable can provide setup information (such as the class tag
// that should be used by this class).  
// 
// Things that setup should do:
//  - layout the features of the class
//  - create the types for the class and its vtable
//  - create global definitions used by the class such as the class vtable
//
void CgenNode::setup(int tag, int depth)
{
	this->tag = tag;        // diff1  layout_features();
}

// 
// code-gen function main() in class Main
//
void CgenNode::codeGenMainmain( std::ostream &o )
{
	ValuePrinter vp( o );
	// In Phase 1, this can only be class Main. Get method_class for main().
	assert(std::string(this->name->get_string()) == std::string("Main"));
	method_class* mainMethod = (method_class*) features->nth(features->first());

	// ADD CODE HERE TO GENERATE THE FUNCTION int Mainmain().
	// Generally what you need to do are:
	// -- setup or create the environment, env, for translating this method
	// -- invoke mainMethod->code(env) to translate the method

    CgenEnvironment *newEnv = new CgenEnvironment( o, this );
    vector<operand> main_args;
    op_type i32_type( INT32 );
    vp.define( i32_type, "Main_main", main_args );
    vp.begin_block( "entry" );
    mainMethod->code( newEnv );
    vp.end_define( );
}

//
// CgenEnvironment functions
//

//
// Class CgenEnvironment should be constructed by a class prior to code
// generation for each method.  You may need to add parameters to this
// constructor.
//
CgenEnvironment::CgenEnvironment(std::ostream &o, CgenNode *c)
{
	cur_class = c;
	cur_stream = &o;
	var_table.enterscope();
    tmp_count = 0;
    block_count = ok_count = -1;
	// ADD CODE HERE
}

// Look up a CgenNode given a symbol
CgenNode *CgenEnvironment::type_to_class(Symbol t) {
	return t == SELF_TYPE ? get_class() 
		: get_class()->get_classtable()->lookup(t);
}

// Provided CgenEnvironment methods
// Generate unique string names
std::string CgenEnvironment::new_name() {
	std::stringstream s;
	s << tmp_count++;
	return "tmp." + s.str();
}

std::string CgenEnvironment::new_ok_label() {
	std::stringstream s;
	s << ok_count++;
	return "ok." + s.str();
}
const std::string CgenEnvironment::new_label(const std::string& prefix,
		bool increment) {
	std::string suffix = itos(block_count);
	block_count += increment;
	return prefix + suffix;
}

void CgenEnvironment::add_local(Symbol name, operand &vb) {
	var_table.enterscope();
	var_table.addid(name, &vb);
}

void CgenEnvironment::kill_local() {
	var_table.exitscope();
}


////////////////////////////////////////////////////////////////////////////
//
// APS class methods
//
////////////////////////////////////////////////////////////////////////////

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.handcode.h'.
//
//*****************************************************************

//
// Create a method body
// 
void method_class::code(CgenEnvironment *env)
{
	if (cgen_debug) std::cerr << "method" << endl;
    ValuePrinter vp( *( env -> cur_stream ) );
    vp.ret( expr->code( env ) );
    vp.begin_block("abort");
    vector<op_type> abort_args_types;
    vector<operand> abort_args;
    operand ab_call = vp.call( abort_args_types, VOID, "abort", true, abort_args );
    vp.unreachable();
}

//
// Codegen for expressions.  Note that each expression has a value.
//

operand assign_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "assign" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand assign_operand = *(env->var_table.lookup(name));
    operand expr_operand = expr->code(env);
    vp.store(*(env->cur_stream), expr_operand, assign_operand);
    return expr_operand;
}

operand cond_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "cond" << endl;
	ValuePrinter vp(*(env->cur_stream));
    operand second_reg;
    string result1, result2, result3;
    operand get_type_op = then_exp->code(env);
    second_reg = vp.alloca_mem(get_type_op.get_type());
    env->block_count++;
    result1 = env->new_label("end.", false);
    result2 = env->new_label("true.", false);
    result3 = env->new_label("false.", false);
    vp.branch_cond(*(env->cur_stream), pred->code(env), result2/*true*/, result3/*false*/);
    //true
    vp.begin_block(result2);
    operand then_op = then_exp->code(env);
    vp.store(*(env->cur_stream), then_op, second_reg);
    vp.branch_uncond(*(env->cur_stream), result1);
    //false
    vp.begin_block(result3);
    operand else_op = else_exp->code(env);
    vp.store(*(env->cur_stream), else_op, second_reg);
    vp.branch_uncond(*(env->cur_stream), result1);
    //end
    vp.begin_block(result1);
    operand final_ret_op = vp.load(second_reg.get_type().get_deref_type(), second_reg);
    return final_ret_op;
}

operand loop_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "loop" << endl;
	ValuePrinter vp(*(env->cur_stream));
    string result1, result2, result3;
    env->block_count++;
    result1 = env->new_label("loop.", false);
    result2 = env->new_label("true.", false);
    result3 = env->new_label("false.", false);
    vp.branch_uncond(*(env->cur_stream), result1);
    vp.begin_block(result1);
    operand pred_op = pred->code(env);
    vp.branch_cond(*(env->cur_stream), pred_op, result2, result3);
    vp.begin_block(result2);
    operand ret_op = body->code(env);
    vp.branch_uncond(*(env->cur_stream), result1);
    vp.begin_block(result3);
    return ret_op;
} 

operand block_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "block" << endl;
	int i = body->first();
    operand complete_block;
    while(body->more(i)){
        complete_block = body->nth(i)->code(env);
        i = body->next(i);
    }
    return complete_block;
}

operand let_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "let" << endl;
    ValuePrinter vp(*(env->cur_stream));
    op_type type = INT32;
    string tempString(type_decl->get_string());
    if(tempString.compare("Bool") == 0)
        type = INT1;
    else if(tempString.compare("Int") == 0)
        type = INT32;
    operand new_op = init->code(env);
    string iden_temp(identifier->get_string());
    operand identifier_op(type, iden_temp);
    operand new_var = vp.alloca_mem(type);
    env->add_local(identifier, new_var);
    if(!(new_op.get_type().get_id() == EMPTY))
        vp.store(*(env->cur_stream), new_op, new_var);
    else
    {
        string val;
        if(type.get_id() == INT1)
            val = "false";
        else if(type.get_id() == INT32)
            val = "0";
        vp.store(*(env->cur_stream), const_value(type, val, true), new_var);
    }
    return body->code(env);
}

operand plus_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "plus" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand plus_op = vp.add(e1_code, e2_code);
    return plus_op;
}

operand sub_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "sub" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand sub_op = vp.sub(e1_code, e2_code);
    return sub_op;
}

operand mul_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "mul" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand mul_op = vp.mul(e1_code, e2_code);
    return mul_op;
}

operand divide_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "div" << endl;
    ValuePrinter vp(*(env->cur_stream));
    env->ok_count++;
    string ok_label_new = env->new_ok_label();
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand eq_op = vp.icmp(EQ, e2_code, int_value(0));
    vp.branch_cond(eq_op, "abort", ok_label_new);
    vp.begin_block(ok_label_new);
    operand div_op = vp.div(e1_code, e2_code);
    return div_op;
}

operand neg_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "neg" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand negated = vp.sub(int_value(0), e1_code);
    return negated;
}

operand lt_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "lt" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand lt_op = vp.icmp(LT, e1_code, e2_code);
    return lt_op;
}

operand eq_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "eq" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand eq_op = vp.icmp(EQ, e1_code, e2_code);
    return eq_op;
}

operand leq_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "leq" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand e2_code = e2->code(env);
    operand leq_op = vp.icmp(LE, e1_code, e2_code);
    return leq_op;
}

operand comp_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "complement" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand e1_code = e1->code(env);
    operand comp_op = vp.xor_in(e1_code, bool_value(true, true));
    return comp_op;
}

operand int_const_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "Integer Constant" << endl;
    int_value int_const(atoi(token->get_string()));
    return int_const;
}

operand bool_const_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "Boolean Constant" << endl;
    bool_value bool_const(val, false);
    return bool_const;
}

operand object_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "Object" << endl;
    ValuePrinter vp(*(env->cur_stream));
    operand *ret_op;
    ret_op = env->lookup(name);
    return vp.load( ret_op->get_type().get_deref_type(),*ret_op );
}

operand no_expr_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "No_expr" << endl;
    operand nothing;
    return nothing;
}

//*****************************************************************
// The next few functions are for node types not supported in Phase 1
// but these functions must be defined because they are declared as
// methods via the Expression_SHARED_EXTRAS hack.
//*****************************************************************

operand static_dispatch_class::code(CgenEnvironment *env) 
{ 
	if (cgen_debug) std::cerr << "static dispatch" << endl;
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

operand string_const_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "string_const" << endl;
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

operand dispatch_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "dispatch" << endl;
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

operand typcase_class::code(CgenEnvironment *env)
{
	if (cgen_debug) 
		std::cerr << "typecase::code()" << endl;
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

operand new__class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "newClass" << endl;
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

operand isvoid_class::code(CgenEnvironment *env) 
{
	if (cgen_debug) std::cerr << "isvoid" << endl;
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

// Create the LLVM Function corresponding to this method.
void method_class::layout_feature(CgenNode *cls) 
{
	assert(0 && "Unsupported case for phase 1");
}

// If the source tag is >= the branch tag and <= (max child of the branch class) tag,
// then the branch is a superclass of the source
operand branch_class::code(operand expr_val, operand tag, op_type join_type, CgenEnvironment *env) {
	assert(0 && "Unsupported case for phase 1");
	return operand();
}

// Assign this attribute a slot in the class structure
void attr_class::layout_feature(CgenNode *cls)
{
	assert(0 && "Unsupported case for phase 1");
}

void attr_class::code(CgenEnvironment *env)
{
	assert(0 && "Unsupported case for phase 1");
}

