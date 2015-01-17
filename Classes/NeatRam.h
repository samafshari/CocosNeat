#ifndef __RAM__H__
#define __RAM__H__

#include "NeatLib.h"
#define FUNCISNULL(func) ((func) == nullptr)

namespace Neat
{
	typedef std::function<void(const nstring&)> nDvarSetValueFunc;
	typedef std::function<nstring(void)> nDvarGetValueFunc;

	class DVar
	{
	private:
		nstring m_value;
		
		nDvarGetValueFunc m_getValueFunc;
		nDvarSetValueFunc m_setValueFunc;
	public:
		DVar(void);
		~DVar(void);
		DVar(nstring value, nDvarGetValueFunc getFunc = nullptr, nDvarSetValueFunc setFunc = nullptr);

		nstring GetValue(void);
		void SetValue(const nstring& value);

		void AssignSetFunc(nDvarSetValueFunc setFunc);
		void AssignGetFunc(nDvarGetValueFunc getFunc);
	};

	class Ram
	{
	private:
		std::map<nstring, DVar> m_items;
	public:
		Ram(void);
		~Ram(void);

		void Set(nstring key, nstring value);
		void Set(nstring key, nDvarGetValueFunc getFunc, nDvarSetValueFunc setFunc);
		bool Remove(nstring key);
		bool Has(nstring key) const;
		nstring GetValue(const nstring& key);
		DVar* Get(nstring key);
		std::vector<nstring> GetVars() const;

		static Ram Shared;
	};
}

#endif