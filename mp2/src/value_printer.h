// 该头文件定义了打印一些 cool 语句的 LLVM 代码的 API

#ifndef __VALUE_PRINTER_H
#define __VALUE_PRINTER_H

#include "operand.h"
#include <assert.h>
#include <vector>

typedef string label;
/* Values acceptable by the icmp instruction */
typedef enum {EQ, NE, LT, LE, GT, GE} icmp_val;

class ValuePrinter {
	private:
		void bin_inst(ostream &o, string inst_name, operand op1, operand op2, operand result);
		operand bin_inst(string inst_name, operand op1, operand op2);
		void check_ostream(ostream& supplied) { assert (!stream || (stream == &supplied)); }	// 若当前流存在，但不等于给定的流，则出错
		void check_ostream() { assert(stream); }	// 若当前流不存在，则出错
		ostream* stream;	// stream 是指向某个输出流的指针
	public:
		// 构造函数
		ValuePrinter() : stream(NULL) {}		// 缺省的构造函数
		ValuePrinter(ostream& o) : stream(&o) {}	// 传进来一个流，取它的地址存入 stream

		// 全局常量初始化
		void init_constant(ostream &o, string name, const_value op);
		void init_constant(string name, const_value op);

		// 外部常量声明
		void init_ext_constant(ostream &o, string name, op_type type);
		void init_ext_constant(string name, op_type type);
		
		// 函数定义与声明（ 都分提供流与不提供流两种形式 ）
		void declare(ostream &o, op_type ret_type, string name, vector<op_type> args);	// 函数声明：参数为类型
		void declare(op_type ret_type, string name, vector<op_type> args);
		void define(ostream &o, op_type ret_type, string name, vector<operand> args);	// 函数定义：参数为操作数，即需要类型加变量
		void define(op_type ret_type, string name, vector<operand> args);
		void end_define(ostream &o) { check_ostream(o); o << "}\n\n"; }			// 函数结束定义
		void end_define() { *stream << "}\n\n"; }

		// 类型定义
		void type_define(ostream &o, string class_name, vector<op_type> attributes);
		void type_define(string class_name, vector<op_type> attributes);

		void type_alias_define(ostream &o, string alias_name, op_type type);
		void type_alias_define(string alias_name, op_type type);

		// 结构常量定义
		void init_struct_constant(ostream &o, operand constant,
			vector<op_type> field_types, vector<const_value> init_values);
		void init_struct_constant(operand constant,
			vector<op_type> field_types, vector<const_value> init_values);

		// 打印一个标号
		void begin_block(string label);

		// 二元运算操作
		void add(ostream &o, operand op1, operand op2, operand result);	
		void sub(ostream &o, operand op1, operand op2, operand result);	
		void mul(ostream &o, operand op1, operand op2, operand result);	
		void div(ostream &o, operand op1, operand op2, operand result);
		void xor_in(ostream &o, operand op1, operand op2, operand result);

		operand add(operand op1, operand op2);	
		operand sub(operand op1, operand op2);	
		operand mul(operand op1, operand op2);	
		operand div(operand op1, operand op2);
		operand xor_in(operand op1, operand op2);

		// 访存指令
		void malloc_mem(ostream &o, int size, operand result);
		void malloc_mem(ostream &o, operand size, operand result);
		void alloca_mem(ostream &o, op_type type, operand op2);
		void load(ostream &o, op_type type, operand op, operand op2);
		void store(ostream &o, operand op, operand op2);
		void getelementptr(ostream &o, op_type type, operand op1, operand op2, operand result);
		void getelementptr(ostream &o, op_type type, operand op1, operand op2, operand op3, operand result);
		void getelementptr(ostream &o, op_type type, operand op1, operand op2, operand op3, operand op4, operand result);
		void getelementptr(ostream &o, op_type type, operand op1, operand op2, operand op3, operand op4, operand op5, operand result);
		void getelementptr(ostream &o, op_type type, vector<operand> op, operand result);

		operand malloc_mem(int size);
		operand malloc_mem(operand size);
		operand alloca_mem(op_type type);
		operand load(op_type type, operand op);

		// 存储但不产生结果
		void store(operand op, operand op2);

		// getelementptr 需要一个参数作为其结果类型，因为这个类型很难计算出来
		operand getelementptr(op_type type, operand op1, operand op2, op_type result_type);
		operand getelementptr(op_type type, operand op1, operand op2, operand op3, op_type result_type);
		operand getelementptr(op_type type, operand op1, operand op2, operand op3, operand op4, op_type result_type);
		operand getelementptr(op_type type, operand op1, operand op2, operand op3, operand op4, operand op5, op_type result_type);
		operand getelementptr(op_type type, vector<operand> op, op_type result_type);

		// 终结指令
		void branch_cond(ostream &o, operand op, label label_true, label label_false);
		void branch_uncond(ostream &o, string label);
		void ret(ostream &o, operand op);
		void unreachable(ostream &o) { check_ostream(o); o << "\tunreachable\n"; }

		void branch_cond(operand op, label label_true, label label_false);
		void branch_uncond(string label);
		void ret(operand op);
		void unreachable() { unreachable(*stream); }

		// 其他操作	
		void select(ostream &o, operand op1, operand op2, operand op3, operand result);
		void icmp(ostream &o, icmp_val v, operand op1, operand op2, operand result);
		void call(ostream &o, vector<op_type> arg_types, string fn_name, bool is_global, vector<operand> args, operand result);
		void bitcast(ostream &o, operand op, op_type new_type, operand result);
		void ptrtoint(ostream &o, operand op, op_type new_type, operand result);

		operand select(operand op1, operand op2, operand op3);
		operand icmp(icmp_val v, operand op1, operand op2);
		operand call(vector<op_type> arg_types, op_type result_type, string fn_name, bool is_global, vector<operand> args);
		operand bitcast(operand op, op_type new_type);
		operand ptrtoint(operand op, op_type new_type);
};

#endif
