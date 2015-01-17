#include "NeatRam.h"

using namespace Neat;

Ram Ram::Shared;

DVar::DVar(void) :
	m_value(NS("")),
	m_getValueFunc(nullptr),
	m_setValueFunc(nullptr)
{
}


DVar::~DVar(void)
{
}

DVar::DVar(nstring value, nDvarGetValueFunc getFunc, nDvarSetValueFunc setFunc) :
	m_getValueFunc(getFunc),
	m_setValueFunc(setFunc)
{
	SetValue(value);
}

nstring DVar::GetValue(void)
{
	if (FUNCISNULL(m_getValueFunc))
		return m_value;

	return m_getValueFunc();
}

void DVar::SetValue(const nstring& value)
{
	if (FUNCISNULL(m_setValueFunc))
		m_value = value;
	else
		m_setValueFunc(value);
}

void DVar::AssignSetFunc(nDvarSetValueFunc setFunc)
{
	m_setValueFunc = setFunc;
}

void DVar::AssignGetFunc(nDvarGetValueFunc getFunc)
{
	m_getValueFunc = getFunc;
}
///////////////////////////////////////////////////////////////

Ram::Ram(void)
{
}

Ram::~Ram(void)
{
}

void Ram::Set(nstring key, nstring value)
{
	key = nToLower(nTrim(key));
	auto var = Get(key);
	if (var == nullptr)
	{
		//Does not contain such dvar. Create one.
		m_items[key] = DVar(value);
		return;
	}

	//Already contains the dvar. update value.
	var->SetValue(value);
}

void Ram::Set(nstring key, nDvarGetValueFunc getFunc, nDvarSetValueFunc setFunc)
{
	key = nToLower(nTrim(key));
	auto var = Get(key);
	if (var == nullptr)
	{
		//Does not contain such dvar. Create one.
		m_items[key] = DVar(NS(""), getFunc, setFunc);
		return;
	}

	//Already contains dvar. Update funcs.
	var->AssignGetFunc(getFunc);
	var->AssignSetFunc(setFunc);
}

bool Ram::Remove(nstring key)
{
	key = nToLower(nTrim(key));
	
	if (!Has(key))
		return false; //Key does not exist. Fail.

	m_items.erase(key);
	return true;
}

bool Ram::Has(nstring key) const
{
	key = nToLower(nTrim(key));
	return m_items.find(key) != m_items.end();
}

nstring Ram::GetValue(const nstring& ikey)
{
	auto key = nToLower(nTrim(ikey));
	auto var = Get(key);

	if (var == nullptr)
		return ikey; //return original query
		//return L"#ERROR"; //Does not contain key, return error string.

	return var->GetValue();
}

DVar* Ram::Get(nstring key)
{
	key = nToLower(nTrim(key));
	auto itr = m_items.find(key);

	if (itr == m_items.end())
		return nullptr;

	return &(itr->second);
}

std::vector<nstring> Ram::GetVars() const
{
	std::vector<nstring> result(m_items.size());
	for (auto itr = m_items.begin(); itr != m_items.end(); itr++)
		result.push_back(itr->first);
	return result;
}