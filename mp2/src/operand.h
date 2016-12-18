// 定义了一些类用于创建或处理操作数的类型

#ifndef __OPERAND_H
#define __OPERAND_H

#include "str_aux.h"
#include <assert.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <ostream>
#include <vector>

using std::string;	// 表明之后使用的这 三个变量 均处于 std 命名空间内
using std::ostream;
using std::vector;

// 所有会要使用到的类型（ 最后三个 MP2.2 才会用到 ）
typedef enum {EMPTY, VOID, INT1, INT1_PTR, INT1_PPTR, INT8, INT8_PTR, INT8_PPTR, 
	      INT32, INT32_PTR, INT32_PPTR, VAR_ARG, OBJ, OBJ_PTR, OBJ_PPTR} op_type_id;

// 基本操作数类型
class op_type {
	protected:
		op_type_id id;
		string name;
	public: 
		// 构造函数
		op_type() : id(EMPTY), name(""){}
		op_type(op_type_id i);
		op_type(string n) : id(OBJ), name("%" + n) {}	
		op_type(string n, int ptr_level);

		op_type_id get_id() { return id; }
		void set_id(op_type_id i) { id = i; }
		void set_type(op_type t) 
		  { id = t.get_id(); name = t.get_name(); }
		string get_name() { return name; }

		bool is_ptr() 
		  { return (id == INT1_PTR || id == INT8_PTR || 
		            id == INT32_PTR || id == OBJ_PTR); }
		op_type get_ptr_type();
		op_type get_deref_type();
		/* Is the type a double pointer? */
		bool is_pptr()
		  { return (id == INT1_PPTR || id == INT8_PPTR ||
		            id == INT32_PPTR || id == OBJ_PPTR); }
		bool is_int_object()
		  { return id == OBJ_PTR && name.compare("%Int*")==0; }
		bool is_bool_object()
		  { return id == OBJ_PTR && name.compare("%Bool*")==0; }
		bool is_string_object()
		  { return id == OBJ_PTR && name.compare("%String*")==0; }
		bool is_self_type()
		  { return id == OBJ && name.compare("%SELF_TYPE")==0; }
		bool is_same_with(op_type t)
		  { return name.compare(t.get_name())==0; }
};

// 数组类型
class op_arr_type : public op_type {
	private:
		int size;
	public:
		op_arr_type(op_type_id, int);
		int get_size() { return size; }
		op_type_id get_id() { return id; }
};

// 函数和函数指针类型
class op_func_type : public op_type {
	private:
    		op_type res;		// 返回值类型
		vector<op_type> args;	// 各参数类型
	public:
		op_func_type(op_type res_type, vector<op_type> arg_types);	// 构造函数原型
		bool is_ptr() { return false; }
		op_type get_ptr_type();
		op_type get_deref_type();
		bool is_pptr() { return false; }
};

class op_func_ptr_type : public op_type {
	private:
    op_type res;
		vector<op_type> args;
	public:
		op_func_ptr_type(op_type res_type, vector<op_type> arg_types);
		bool is_ptr() { return true; }
		op_type get_ptr_type();
		op_type get_deref_type();
		bool is_pptr() { return false; }
};

//===================================================================================

// 基本操作数
class operand {
	protected:
		op_type type;
		string name;
	public:
		operand() : type(EMPTY), name("") { }
		operand(const operand& other)
		  : type(other.type), name(other.name) {}
		operand(op_type t, string n) : type(t), name("%" + n) {}
		op_type get_type() { return type; }
		void set_type(op_type t) { type = t; }
		string get_typename() { return type.get_name(); }
		string get_name () { return name; }
		bool is_empty() { return type.get_id() == EMPTY; }
};

// 全局变量
class global_value : public operand {
	private:
		operand value;
	public:
		global_value(op_type t, string n, operand v) 
		  { type = t; name = "@" + n; value = v;}
		global_value(op_type t, string n) { type = t; name = "@" + n;}
		operand get_value() { return value; }
};

// 常量
class const_value : public operand {
	protected:
		string value;
		bool internal;
	public:
		const_value(op_type t, string val, bool intr)
		  : value(val), internal(intr) { type = t; name = value; }
		bool is_internal() { return internal; }
		string get_value() { return value; }
};


class casted_value : public const_value {
	protected:
		op_type precast_type;
	public:
		casted_value(op_type t, string val, op_type precast_t) :
		  const_value(t, "bitcast ("+precast_t.get_name()+" "+val+" to "+t.get_name()+")", true),
		  precast_type(precast_t) { }
		op_type get_precast_type() { return precast_type; }
		string get_precasttypename() { return precast_type.get_name(); }
};

// 整型常量
class int_value : public const_value {
	private:
		int i_value;
	public:
		int_value(int i): const_value(op_type(INT32), itos(i), true), i_value(i) {}
		int_value(int i, bool intr): const_value(op_type(INT32), itos(i), intr), i_value(i) {}
		int get_intvalue() { return i_value; }
};

// 布尔常量
class bool_value : public const_value {
	private:
		bool b_value;
	public:
		bool_value(bool b, bool intr) : const_value(op_type(INT1), "", intr), b_value(b)
		  { if (b) value = "true"; else value = "false"; name = value;}
		int get_boolvalue() { return b_value; }
};

class null_value : public const_value {
	public:
		null_value(op_type t) : const_value(t, "null", true) {}
};
#endif
