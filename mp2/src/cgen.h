#include "cool-tree.h"
#include "symtab.h"
#include "value_printer.h"

// 一共包含三个类：CgenClassTable、CgenNode、CgenEnvironment


//==========================================================================================

// CgenClassTable： 【 类的代码生成表 】
// 代表 cool 程序的顶层，是一个 classes 形成的 list
// 用来通过名字查找类，查找 CgenNodes
// 完成全局的代码生成
// 通过其构造函数，能够找到为整个 cool 程序作代码生成的入口点

class CgenClassTable : public cool::SymbolTable<Symbol,CgenNode> 
{
	private:
	// Class list
	List<CgenNode> *nds;
	List<CgenNode> *special_nds;
	int current_tag;
    
	CgenNode* getMainmain(CgenNode* c);

	public:
	// The ostream where we are emitting code
	ostream *ct_stream;
	// CgenClassTable constructor begins and ends the code generation process
	CgenClassTable(Classes, ostream& str);		// 构造函数: 以下各种定义在 private 里的函数，都是在构造函数里被调用的
	~CgenClassTable();				// 析构函数 

	// Get the root of the class Tree, i.e. Object
	CgenNode *root();
	int get_num_classes() const	{ return current_tag; }

	private:
	// COMPLETE FUNCTIONS
    
	// Create declarations for C runtime functions we need to generate code
	void setup_external_functions();
	void setup_classes(CgenNode *c, int depth);

	// The following creates an inheritance graph from a list of classes.  
	// The graph is implemented as a tree of `CgenNode', and class names 
	// are placed in the base class symbol table.
	void install_basic_classes();
	void install_class(CgenNode *nd);
	void install_special_class(CgenNode *nd);
	void install_classes(Classes cs);
	void build_inheritance_tree();
	void set_relations(CgenNode *nd);
    
	// INCOMPLETE FUNCTIONS
    
	// Setup each class in the table and prepare for code generation phase
	void setup();
    
	// Code generation functions. You need to write these functions.
	void code_module();
	void code_constants();
	void code_main();

	// ADD CODE HERE

};

//==========================================================================================

// CgenNode： 【 代码生成结点 】
// 每个结点对应一个 cool 中的类，以至于可以按照 class 为单位产生代码
// 1. 完成类的属性的布局
// 2. 创建必要的类型
// 3. 为每个方法产生代码

class CgenNode : public class__class 
{
	public:
	enum Basicness { Basic, NotBasic };

	void codeGenMainmain(std::ostream &o);

	private: 
	CgenNode *parentnd;                        // Parent of class
	List<CgenNode> *children;                  // Children of class
	Basicness basic_status;                    // `Basic' or 'NotBasic'
	CgenClassTable *class_table;
    
	// Class tag.  Should be unique for each class in the tree
	int tag;
	int max_child;


	// ADD CODE HERE


	public:
	// COMPLETE FUNCTIONS

	// Relationships with other nodes in the tree
 	CgenNode *get_parentnd() { return parentnd; }
	void add_child(CgenNode *child);
	void set_parentnd(CgenNode *p);
	int basic() { return (basic_status == Basic); }
	List<CgenNode> *get_children() { return children; }
    
	// Accessors for other provided fields
	int get_tag() const 	{ return tag; }
	CgenClassTable *get_classtable() { return class_table; }

	void set_max_child(int mc) 	{ max_child = mc; }
	int get_max_child() const 	{ return max_child; }

	// INCOMPLETE FUNCTIONS
    
	// Constructs a CgenNode from a Class
	CgenNode(Class_ c, Basicness bstatus, CgenClassTable *class_table);
	virtual ~CgenNode() { }

	// Class setup. You need to write the body of this function.
	void setup(int tag, int depth);

	// Class codegen. You need to write the body of this function.
	void code_class();

	// ADD CODE HERE
	string get_type_name() { return string(name->get_string()); }


	private:
	// Layout the methods and attributes for code generation
	// You need to write the body of this function.
	void layout_features();

	// ADD CODE HERE

};

//==========================================================================================

// CgenEnvironment：【 代码生成的环境 】
// 提供生成“方法”的代码的环境
// 提供 Cool 程序中的名字到 LLVM 代码中值的映射关系
// 需要随着一个域的进入与退出、新变量的声明等而不断维护
// 可以用来记录一些代码生成过程中需要使用到的非局部信息

class CgenEnvironment
{
	private:

	// ADD CODE HERE
	CgenNode *cur_class;		// cur_class 指向当前类结点

	public:
	// mapping from variable names to memory locations
	cool::SymbolTable<Symbol,operand> var_table;
    op_type recent_type;
	// Keep counters for unique name generation in the current method
	int block_count;
	int tmp_count;
	int ok_count;

	std::ostream *cur_stream;	// cur_stream 指向当前流

	// fresh name generation functions
	string new_name();
	string new_ok_label();
	const string new_label(const std::string& prefix, bool increment);

	// Used in provided code for the (case..of) construct
	string next_label;
	operand branch_operand;    
	void add_local(Symbol name, operand &vb);
	void kill_local();
	// end of helpers for provided code

	CgenEnvironment(ostream &strea, CgenNode *cur_class);		// 构造函数： 传入ostream 类型变量的引用 strea ？？？ 什么地方用上了 ？？？	


	operand *lookup(Symbol name)	{ return var_table.lookup(name); }
    
	CgenNode *get_class() { return cur_class; }
	void set_class(CgenNode *c) { cur_class = c; }
    
	// INCOMPLETE FUNCTIONS

	// Must return the CgenNode for a class given the symbol of its name
	CgenNode *type_to_class(Symbol t);
	// ADD CODE HERE
	
};

// Utitlity function
// Generate any code necessary to convert from given operand to
// dest_type, assuing it has already been checked to be compatible
operand conform(operand src, op_type dest_type, CgenEnvironment *env);
// Retrieve the class tag from operand src. Argument is the cgen node for
// the static class of src.
operand get_class_tag(operand src, CgenNode *src_cls, CgenEnvironment *env);
