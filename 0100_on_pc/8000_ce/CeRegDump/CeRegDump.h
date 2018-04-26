#ifndef CE_REG_DUMP_H
#define CE_REG_DUMP_H

#include <Windows.h>
#include <crtdefs.h>
#include <yvals.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>

#define LOG_INFO	L"INFO"
#define LOG_ERROR	L"ERROR"
#define CRD_LOG_PRINT(lev, format, ...) ::wprintf( L"[CRD][%08X][%s], " L#format L"\n", ::GetTickCount(), lev, __VA_ARGS__ );


class CeRegDump {

public:

	CeRegDump() 
		:	m_hPredefinedKey()
			, m_predefinedKeyName()
			// , m_hRootKey()
			, m_subKeyName()
			, m_rootNode(0) 
			, m_init(false)
			, m_dwNodeCnt(0)
			, m_dwValueCnt(0)
			, m_dwQIKErrorCnt(0)
			, m_dwEVErrorCnt(0)
			, m_dwEKErrorCnt(0)
			{ CRD_LOG_PRINT(LOG_ERROR, "You shall not call the default constructor" ); };
			
	CeRegDump(HKEY hPredefinedKey) 
		:	m_hPredefinedKey(hPredefinedKey)
			, m_predefinedKeyName()
			// , m_hRootKey()
			, m_subKeyName()
			, m_rootNode(0)			
			, m_init(false)
			, m_dwNodeCnt(0)
			, m_dwValueCnt(0)
			, m_dwQIKErrorCnt(0)
			, m_dwEVErrorCnt(0)
			, m_dwEKErrorCnt(0)			
			{ 
				if ( mapToPredefinedName(m_hPredefinedKey) ) { 
					
					CRD_LOG_PRINT(LOG_INFO, "extracted predefined key name: [%s]", m_predefinedKeyName.c_str() );
					CRD_LOG_PRINT(LOG_INFO, "extracted sub key name: [%s]", m_subKeyName.c_str() );	
					m_init = true; 
				}else {
					CRD_LOG_PRINT(LOG_ERROR, "failed to map to predefined key name" );
				}
			};
			
	// all registry are in format of WCHAR
	CeRegDump(HKEY hPredefinedKey, const std::wstring& subKeyName) 
		:	m_hPredefinedKey(hPredefinedKey)
			, m_predefinedKeyName()
			//, m_hRootKey()
			, m_subKeyName(subKeyName)
			, m_rootNode(0)	
			, m_init(true)	
			, m_dwNodeCnt(0)
			, m_dwValueCnt(0)
			, m_dwQIKErrorCnt(0)
			, m_dwEVErrorCnt(0)
			, m_dwEKErrorCnt(0)			
			{};
			
	CeRegDump(const std::wstring& keyFullName) 
		:	m_hPredefinedKey()
			, m_predefinedKeyName()
			//, m_hRootKey()
			, m_subKeyName()
			, m_rootNode(0)	
			, m_init(false)		
			, m_dwNodeCnt(0)
			, m_dwValueCnt(0)
			, m_dwQIKErrorCnt(0)
			, m_dwEVErrorCnt(0)
			, m_dwEKErrorCnt(0)			
			{ 
				if ( extractFromFullName(keyFullName, m_predefinedKeyName, m_subKeyName) ) {
				
					if ( mapToPredefinedHKEY(m_predefinedKeyName, m_hPredefinedKey) ) {
					
						CRD_LOG_PRINT(LOG_INFO, "extracted predefined key name: [%s]", m_predefinedKeyName.c_str() );
						CRD_LOG_PRINT(LOG_INFO, "extracted sub key name: [%s]", m_subKeyName.c_str() );
						m_init = true;
					}
					else {
						CRD_LOG_PRINT(LOG_ERROR, "failed to map the predefined key" );
					}
				}
				else {
					CRD_LOG_PRINT(LOG_ERROR, "failed to extract predefined key name and/or sub key name" );
				}
			};		
			
	~CeRegDump() { releaseTree(m_rootNode); };
	
	bool traverse();
	bool dump(const std::wstring& tar);
	
private:

	const static std::wstring		WSTR_HKEY_CLASSES_ROOT;
	const static std::wstring		WSTR_HKEY_CURRENT_USER;
	const static std::wstring		WSTR_HKEY_LOCAL_MACHINE;
	const static std::wstring		WSTR_HKEY_USERS;

	enum { SPARED_TAIL_SIZE = 256 };
	
	struct KeyInfo {
	
		std::wstring		m_class;
		DWORD				m_subKeyNum;
		DWORD				m_maxSubKeyLen;
		DWORD				m_maxClassLen;
		DWORD				m_valueNum;
		DWORD				m_maxValueNameLen;
		DWORD				m_maxValueDataLen;
	};
	
	struct KeyValue {
	
		std::wstring				m_valueName;
		DWORD						m_valueType;
		std::vector<BYTE>			m_valueData;
		
		void format(std::wstring& output) {

			switch (m_valueType) {
				
				case REG_BINARY: { 
					output.append(L"Type: REG_BINARY\n\t" ); 
					break; 
				}
				case REG_DWORD: { 
					output.append(L"Type: REG_DWORD\n\t" ); 
					
					break; 
				}
				/*
				case REG_DWORD_LITTLE_ENDIAN: { 
					output.append(L"Type: REG_DWORD_LITTLE_ENDIAN\n\t" ); 
					break; 
				}
				*/
				case REG_DWORD_BIG_ENDIAN: { 
					output.append(L"Type: REG_DWORD_BIG_ENDIAN\n\t" ); 
					break; 
				}
				case REG_EXPAND_SZ: { 
					output.append(L"Type: REG_EXPAND_SZ\n\t" ); 
					break; 
				}
				case REG_LINK: { 
					output.append(L"Type: REG_LINK\n\t" ); 
					break; 
				}
				case REG_MULTI_SZ: { 
					output.append(L"Type: REG_MULTI_SZ\n\t" ); 
					break; 
				}
				case REG_NONE: { 
					output.append(L"Type: REG_NONE\n\t" ); 
					break; 
				}
				case REG_RESOURCE_LIST: { 
					output.append(L"Type: REG_RESOURCE_LIST\n\t" ); 
					break; 
				}
				case REG_SZ: { 
					output.append(L"Type: REG_SZ\n\t" ); 
					break; 
				}
				default: { 
					output.append(L"Type: REG_NONE\n\t" ); 
					break;
				}
			}
			
			output.append(L"Data: \" ");
			std::vector<BYTE>::iterator itr;
			for ( itr = m_valueData.begin(); itr != m_valueData.end(); itr++ ) {
			
				enum { BUFF_SIZE = 10 };
				WCHAR buff[BUFF_SIZE + 1];
				::memset(buff, 0, sizeof(buff));
				::wsprintf(buff, L"%02X ", *itr);
				output.append(buff);
			}
			output.append(L"\"\n\n");
		};
	};
	
	struct KeyNode {
	
		std::wstring				m_keyName;
		KeyInfo						m_info;
		std::list<KeyValue*>		m_values;
		KeyNode*					m_ancester;
		std::list<KeyNode*>			m_childList;
		
		static KeyValue* createKeyValueOnHeap() { return new KeyValue; };
		static void destoryKeyValueOnHeap(KeyValue* p) { if (p) { delete p; } };
		
		~KeyNode() {		
			std::list<KeyValue*>::iterator itr;
			for ( itr = m_values.begin(); itr != m_values.end(); itr++ ) {
				destoryKeyValueOnHeap(*itr);
			}
			m_values.clear();
			m_childList.clear();
		};
	};

	// call ::RegQueryInfoKey()
	bool extractKeyInfo(HKEY hKey, KeyInfo& info);
	// call ::RegEnumValue()
	bool extractKeyValue(HKEY hKey, KeyInfo& info, std::list<KeyValue*>& list);
	// call ::RegEnumKeyEx()
	bool extractSubKeysName(HKEY hKey, KeyInfo& info, std::list<std::wstring>& children);
	// call three functions above
	bool extractInfoToNode(HKEY hKey, KeyNode* tar, std::list<std::wstring>& children);
	
	KeyNode* createNode() { m_dwNodeCnt++; return new KeyNode; };
	void destoryNode(KeyNode* node) { if (node) { delete node; } };
	
	void releaseTree(KeyNode* start);
	
	bool extractFromFullName(const std::wstring& fullName, std::wstring& predefinedName, std::wstring& subKeyName);
	bool mapToPredefinedHKEY(const std::wstring& name, HKEY& key);
	bool mapToPredefinedName(HKEY key);
	
	void handleNode(HKEY hSubKey, KeyNode* elem, std::list< KeyNode* >& stack);
	
	HKEY						m_hPredefinedKey;
	std::wstring				m_predefinedKeyName;
	// HKEY						m_hRootKey;
	std::wstring				m_subKeyName;
	KeyNode*					m_rootNode;
	bool 						m_init;
	DWORD						m_dwNodeCnt;
	DWORD						m_dwValueCnt;
	DWORD						m_dwQIKErrorCnt;
	DWORD						m_dwEVErrorCnt;
	DWORD						m_dwEKErrorCnt;
};


#endif	/* CE_REG_DUMP_H */