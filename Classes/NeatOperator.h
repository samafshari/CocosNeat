#ifndef __OPERATOR_H__
#define __OPERATOR_H__

namespace Neat
{
	typedef nstring (*nOperatorFunction)(const nstring&, const nstring&);

	nstring nEvaluateAdd(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateSub(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateMul(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateDiv(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluatePow(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateMod(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateNot(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateAnd(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateOr(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateXor(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateCmpEqual(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateCmpNotEqual(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateCmpGreaterThan(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateCmpGreaterEqualThan(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateCmpLessThan(const nstring& lvalue, const nstring& rvalue = NS(""));
	nstring nEvaluateCmpLessEqualThan(const nstring& lvalue, const nstring& rvalue = NS(""));

	class Operator
	{
	public:
		nstring m_sign;
		nOperatorFunction m_func;
		bool m_unary;
		int m_order;
		nchar m_tag;

		Operator(void);
		Operator(nstring sign, nOperatorFunction func, int ord, bool unary = false);
		~Operator(void);

		nstring Evaluate(const nstring& lvalue, const nstring& rvalue = NS("")) const;
	};
}

#endif