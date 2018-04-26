#include "TK_Tools.h"

namespace TK_Tools {

class RegKey
{
public:
	enum ErrCode {
		EC_OK,
		EC_FAILED,
		EC_NULL_PTR,
		EC_INVALID_HANDLE,
		EC_NO_ROOT_KEY,
		EC_FAILED_TO_ALLOC_MEM,
		EC_FAILED_TO_OPEN_KEY,
		EC_FAILED_TO_CREATE_KEY,
		EC_FAILED_TO_DELETE_KEY,
		EC_FAILED_TO_DELETE_VALUE,
		EC_FAILED_TO_SET_VALUE,
		EC_FAILED_TO_GET_VALUE,
		EC_FAILED_TO_GET_KEY_INFO,
		EC_FAILED_TO_GET_VALUE_INFO,
		EC_FAILED_TO_FLUSH_KEY,
		EC_NO_SUB_KEY,
		EC_UNKNOWN_VALUE_TYPE,
	};
	
	struct KeyValue {
		KeyValue();
		KeyValue(const KeyValue &other);
		KeyValue(const std::vector<BYTE> &vBinary);
		KeyValue(const BYTE *pData, unsigned int nDataSize);
		KeyValue(DWORD vDword);
		KeyValue(const std::wstring &vSz, bool bExpend = false);
		KeyValue(const std::vector<std::wstring> &vMultiSz);
		KeyValue(const wchar_t *ppMultiSz[], unsigned int nCount);

		KeyValue &operator =(const KeyValue &other);

		std::wstring ToStr();
		static bool FromStr(const std::wstring &sValueStr, KeyValue &rRetKeyValue);
		
		enum Type {
			KVT_UNKNOWN,
			KVT_BINARY,
			KVT_DWORD,
			KVT_SZ,
			KVT_EXPAND_SZ,
			KVT_MULTI_SZ,
		} type;
		
		struct Value {
			Value() : vBinary(), vDword(0), vSz(), vExpendSz(), vMultiSz() {}
			Value(const Value &other) : vBinary(other.vBinary), vDword(other.vDword), vSz(other.vSz), vExpendSz(other.vExpendSz), vMultiSz(other.vMultiSz) {}
			Value(const std::vector<BYTE> &a_vBinary) : vBinary(a_vBinary), vDword(0), vSz(), vExpendSz(), vMultiSz() {}
			Value(DWORD a_vDword) : vBinary(), vDword(a_vDword), vSz(), vExpendSz(), vMultiSz() {}
			Value(const std::wstring &a_vSz) : vBinary(), vDword(0), vSz(a_vSz), vExpendSz(), vMultiSz() {}
			Value(const std::wstring &a_vExpendSz, int nPlaceHolder) : vBinary(), vDword(0), vSz(), vExpendSz(a_vExpendSz), vMultiSz() {}
			Value(const std::vector<std::wstring> &a_vMultiSz) : vBinary(), vDword(0), vSz(), vExpendSz(), vMultiSz(a_vMultiSz) {}

			Value &operator =(const Value &other) { vBinary = other.vBinary; vDword = other.vDword; vSz = other.vSz; vExpendSz = other.vExpendSz; vMultiSz = other.vMultiSz; return *this; }
		
			std::vector<BYTE> vBinary;
			DWORD vDword;
			std::wstring vSz;
			std::wstring vExpendSz;
			std::vector<std::wstring> vMultiSz;
		} value;
	};

	struct KeyNameValuePair {
		std::wstring sKeyName;
		KeyValue oKeyValue;
	};

private:
	RegKey();

public:
	RegKey(HKEY hKey);
	virtual ~RegKey();

public:
	static ErrCode CreateKey(RegKey **ppRetKey, const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);
	static ErrCode OpenKey(RegKey **ppRetKey, const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);
	static ErrCode DeleteKey(const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);
	static bool KeyExists(const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);
	static ErrCode GetSubKeyNames(std::vector<std::wstring> &arrSubKeyNames, const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);
	static ErrCode FlushKey(const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);
	static ErrCode FlushAllKeys();

	static ErrCode SetValue(const std::wstring &sKeyPath, const std::wstring &sValueName, const KeyValue &rValue, bool bCreatePathIfNotExists = false, const RegKey *pParentKey = NULL);
	static ErrCode GetValue(const std::wstring &sKeyPath, const std::wstring &sValueName, KeyValue &rValue, const RegKey *pParentKey = NULL);
	static ErrCode DeleteValue(const std::wstring &sKeyPath, const std::wstring &sValueName, const RegKey *pParentKey = NULL);
	static bool ValueExists(const std::wstring &sKeyPath, const std::wstring &sValueName, const RegKey *pParentKey = NULL);
	static ErrCode GetValues(std::vector<KeyNameValuePair> &arrKeyValues, const std::wstring &sKeyPath, const RegKey *pParentKey = NULL);

public:
	ErrCode CreateKey(RegKey **ppRetKey, const std::wstring &sKeyName);
	ErrCode CreateKey(const std::wstring &sKeyName);
	ErrCode OpenKey(RegKey **ppRetKey, const std::wstring &sKeyName);
	ErrCode DeleteKey(const std::wstring &sKeyName);
	bool KeyExists(const std::wstring &sKeyName);
	ErrCode GetSubKeyNames(std::vector<std::wstring> &arrKeyNames);
	ErrCode FlushKey();

	ErrCode SetValue(const std::wstring &sValueName, const KeyValue &rValue);
	ErrCode GetValue(const std::wstring &sValueName, KeyValue &rValue);
	ErrCode DeleteValue(const std::wstring &sValueName);
	bool ValueExists(const std::wstring &sValueName);
	ErrCode GetValues(std::vector<KeyNameValuePair> &arrKeyValues);

public:
	HKEY GetHandle() const { return m_hKey; }
    static DWORD GetWin32LastError() { return m_nWin32LastError; }

private:
	static ErrCode ParseKeyPath(const std::wstring &sKeyPath, const RegKey *pParentKey, std::vector<std::wstring> &sPathNodes, HKEY &hParentKey);
	static HKEY GetRootHKeyForStr(const std::wstring &sPathNode);

private:
	RegKey(RegKey &);
	RegKey &operator =(RegKey &);

private:
	HKEY m_hKey;
    static DWORD m_nWin32LastError;
};

} // namespace TK_Tools {

