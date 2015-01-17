#include "NeatLib.h"
#include "NeatOperator.h"

namespace Neat
{
	nstring nEvaluateAdd(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) + nToFloat(rvalue));
	}

	nstring nEvaluateSub(const nstring& lvalue, const nstring& rvalue)
	{
		if (lvalue.size() == 0 || lvalue == NS(""))
			return nToString(-nToFloat(rvalue));

		if (lvalue == NS("-")) //in case of --1, or (-(-1)) :-)
			return rvalue;

		return nToString(nToFloat(lvalue) - nToFloat(rvalue));
	}

	nstring nEvaluateMul(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) * nToFloat(rvalue));
	}

	nstring nEvaluateDiv(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) / nToFloat(rvalue));
	}

	nstring nEvaluatePow(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(pow(nToFloat(lvalue), nToFloat(rvalue)));
	}

	nstring nEvaluateMod(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(fmod(nToFloat(lvalue), nToFloat(rvalue)));
	}

	nstring nEvaluateNot(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(!nToInt(lvalue));
	}

	nstring nEvaluateAnd(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToInt(lvalue) & nToInt(rvalue));
	}

	nstring nEvaluateOr(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToInt(lvalue) | nToInt(rvalue));
	}

	nstring nEvaluateXor(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToInt(lvalue) ^ nToInt(rvalue));
	}

	nstring nEvaluateCmpEqual(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) == nToFloat(rvalue));
	}

	nstring nEvaluateCmpNotEqual(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) != nToFloat(rvalue));
	}

	nstring nEvaluateCmpGreaterThan(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) > nToFloat(rvalue));
	}

	nstring nEvaluateCmpGreaterEqualThan(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) >= nToFloat(rvalue));
	}

	nstring nEvaluateCmpLessThan(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) < nToFloat(rvalue));
	}

	nstring nEvaluateCmpLessEqualThan(const nstring& lvalue, const nstring& rvalue)
	{
		return nToString(nToFloat(lvalue) <= nToFloat(rvalue));
	}

	Operator::Operator(void)
	{
	}


	Operator::~Operator(void)
	{
	}

	Operator::Operator(nstring sign, nOperatorFunction func, int ord, bool unary) :
		m_sign(sign),
		m_func(func),
		m_order(ord),
		m_unary(unary)
	{
	}

	nstring Operator::Evaluate(const nstring& lvalue, const nstring& rvalue) const
	{
		return m_func(lvalue, rvalue);
	}
}