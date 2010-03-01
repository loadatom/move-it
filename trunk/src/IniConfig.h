#ifndef ALIB_INI_CONFIG_H
#define ALIB_INI_CONFIG_H

#include <vector>
#include <map>

class wxString;
class wxMBConv;


namespace moveit {
	struct Section
	{
		std::map<wxString, wxString> keyValues;
	};

	class IniConfig
	{
	public:
		wxString m_iniFile;
		wxMBConv *m_pConv;
		std::map<wxString, Section> m_data;

		bool Init(wxString iniFile, wxMBConv& conv);
		bool Load();
		bool Save();
		bool GetVal(const wxString &section, const wxString &key, wxString* pVal);
		void GetVal(const wxString &section, const wxString &key, const wxString  &defaultVal, wxString* pVal);
		void SetVal(const wxString &section, const wxString &key, const wxString &val);
		void DelKey(const wxString &section, const wxString &key);
		void DelSection(const wxString &section);
	};
}
#endif	//ALIB_INI_CONFIG_H