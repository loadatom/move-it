#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include "IniConfig.h"

namespace {
	bool SplitKeyValue(const wxString& line, wxString* pKey, wxString* pVal)
	{
		int pos = line.Find(wxT('='));
		wxString key, val;

		if (pos == wxNOT_FOUND)
			return false;
		
		key = line.Mid(0, pos);
		val = line.Mid(pos + 1);
		key.Trim();
		val.Trim(false);
		*pKey = key;
		*pVal = val;
		return true;
	}

	bool IsSection(const wxString& line)
	{
		wxString ln = line.Strip();
		if (ln.StartsWith(wxT("[")) && ln.EndsWith(wxT("]")))
			return true;
		return false;
	}

	wxString GetSectionName(const wxString& line)
	{
		return line.Mid(1, line.Len() - 2);
	}
}

namespace moveit {

	bool IniConfig::Init(wxString iniFile, wxMBConv& conv)
	{
		m_iniFile = iniFile;
		m_pConv = conv.Clone();
		return true;
	}

	bool IniConfig::Load()
	{
		bool bResult = false;
		bool bRetCode = false;
		wxFile iniFile;
		
		bRetCode = iniFile.Open(m_iniFile, wxFile::read);
		if (!bRetCode)
		{
			goto Exit0;
		}
		else
		{
			wxFileInputStream fileStream(iniFile);
			wxTextInputStream textStream(fileStream, wxT("\t"), *m_pConv);
			wxString line = textStream.ReadLine();
			wxString curSection;
			while (!line.IsEmpty())
			{
				Section* pSectionData = NULL;
				wxString key, val;

				if (IsSection(line))
				{
					curSection = GetSectionName(line);
					pSectionData = &m_data[curSection];
				}
				else if(SplitKeyValue(line, &key, &val))
				{
					if (curSection.IsEmpty())
						goto Exit0;
					SetVal(curSection, key, val);
				}
				line = textStream.ReadLine();
			}
		}
		bResult = true;
Exit0:
		iniFile.Close();
		if (!bResult)
		{
			m_data.clear();
		}
		return bResult;
	}

	bool IniConfig::Save()
	{
		bool bResult = false;
		bool bRetCode = false;
		wxFile iniFile;
		
		bRetCode = iniFile.Open(m_iniFile, wxFile::write);
		if (!bRetCode)
			goto Exit0;
		else
		{
			wxFileOutputStream outStream(iniFile);
			wxTextOutputStream txtStream(outStream, wxEOL_NATIVE, *m_pConv);
			for (std::map<wxString, Section>::iterator sectionIter = m_data.begin();
				sectionIter != m_data.end();
				++sectionIter)
			{
				wxString sectionLine = wxString::Format(wxT("[%s]\n"), sectionIter->first.c_str());;
				txtStream.WriteString(sectionLine);

				for (std::map<wxString, wxString>::iterator kvIter = sectionIter->second.keyValues.begin();
					kvIter != sectionIter->second.keyValues.end();
					++kvIter)
				{
					wxString kvLine = wxString::Format(wxT("%s = %s\n"), kvIter->first.c_str(), kvIter->second.c_str());
					txtStream.WriteString(kvLine);
				}
				txtStream.WriteString(wxT("\n"));
			}
		}
		bResult = true;
Exit0:
		iniFile.Close();
		return bResult;
	}

	bool IniConfig::GetVal(const wxString &section, const wxString &key, wxString* pVal)
	{
		bool bResult = false;
		std::map<wxString, Section>::iterator sectionIter = m_data.find(section);
		std::map<wxString, wxString>::iterator kvIter;
		Section* pSection = NULL;

		if (sectionIter == m_data.end())
			goto Exit0;
		pSection = &sectionIter->second;
		kvIter = pSection->keyValues.find(key);
		if (kvIter == pSection->keyValues.end())
			goto Exit0;
		*pVal = kvIter->second;

		bResult = true;
Exit0:
		return bResult;
	}

	void IniConfig::GetVal(const wxString &section, const wxString &key, const wxString &defaultVal, wxString *pVal)
	{
		if (GetVal(section, key, pVal))
			return;
		SetVal(section, key, defaultVal);
		GetVal(section, key, pVal);
	}

	void IniConfig::SetVal(const wxString &section, const wxString &key, const wxString &val)
	{
		Section* pSection = NULL;

		pSection = &m_data[section];
		pSection->keyValues[key] = val;	
	}

	void IniConfig::DelKey(const wxString &section, const wxString &key)
	{
		std::map<wxString, Section>::iterator sectionIter = m_data.find(section);
		
		if (sectionIter != m_data.end())
		{
			Section* pSection = &sectionIter->second;
			pSection->keyValues.erase(key);
		}
	}

	void IniConfig::DelSection(const wxString &section)
	{
		m_data.erase(section);
	}
}