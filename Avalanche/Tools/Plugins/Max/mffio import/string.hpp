inline bool String::IsDigit() const
{
	for (int i = 0; i < this->size(); i++)
		if (isalpha((*this)[i]))
			return false;
	return true;
}


inline bool String::IsAlpha() const
{
	for (int i = 0; i < this->size(); i++)
		if (isdigit((*this)[i]))
			return false;
	return true;
}

inline bool String::IsSMPTE() const
{
	for (int i = 0; i < this->size(); i++) {
		if (isalpha((*this)[i])) 
			return false;
		if ((*this)[i] == ':')
			return true;
	}
	return false;
}

inline long String::AsInt() const
{
	if (!this->IsSMPTE()) {
		const char *s = this->c_str();
		return atol(s);
	}
	
	String temp = *this;
	StringArray tempsplit;
	temp.Split(':',tempsplit);
	int n0 = atol(tempsplit[0].c_str());
	int n1 = atol(tempsplit[1].c_str());
	return (n0*30)+n1;
}

inline double String::AsFloat() const
{
	const char *s = this->c_str();
	float value = atof(s);
	return value;
}

inline bool String::AsBool() const
{
	if (((*this) == "TRUE")||((*this) == "true"))
		return true;
	return false;
}

inline void String::Split(char c,StringArray &stringArray)
{
	String s;
	stringArray.clear();
	for (int i = 0; i < this->size(); i++) {
		if ((*this)[i] == c){
			stringArray.push_back(s);
			s.erase();
		}
		else
			s += (*this)[i];
	}
	if (s.size()>0)
		stringArray.push_back(s);
}


inline void String::ReplaceChar(char find,char replace)
{
	for (int i = 0; i < this->size(); i++) {
		if ((*this)[i] == find)
			(*this)[i] = replace;
	}
}


inline String String::GetFileNameWithoutExt()
{
	StringArray splitarray0,splitarray1;
	for (int i = 0; i < this->size(); i++) {
		if ((*this)[i] == '\\') {
			this->Split('\\',splitarray0);
			(splitarray0.back()).Split('.',splitarray1);
			return splitarray1.front();
		}
	}
	this->Split('/',splitarray0);
	(splitarray0.back()).Split('.',splitarray1);
	return splitarray1.front();
}

inline String String::GetFileNameWithExt()
{
	StringArray splitarray0;
	for (int i = 0; i < this->size(); i++) {
		if ((*this)[i] == '\\') {
			this->Split('\\',splitarray0);
			return splitarray0.back();
		}
	}
	this->Split('/',splitarray0);
	return splitarray0.back();
}

inline String String::GetPath()
{
	String path = "";
	StringArray splitarray0;
	for (int i = 0; i < this->size(); i++) {
		if ((*this)[i] == '\\') {
			this->Split('\\',splitarray0);
			for (int j = 0; j < splitarray0.size()-1; j++)
				path += splitarray0[j] + '\\';
			return path;
		}
	}
	this->Split('/',splitarray0);
	for (i = 0; i < splitarray0.size()-1; i++)
		path += splitarray0[i] + '/';
	return path;
}

inline bool String::Compare(String &substr)
{
	for (int i = 0; i < this->size(); i++)
	{
		if ((*this)[i] == substr[0])
		{
			for	(int j = 1; j < substr.size();j++)
			{
				if ((*this)[i+j] != substr[j])
					return false;
			}
			return true;
		}
	}
	return false;
}

inline int String::StripIntOnEnd(String &str)
{
	String integer;
	str = "";

	int len = this->length();
	int i;
	for (i = 0; i < len; i++)
		if (isalpha((*this)[len-i-1]))
			break;

	for (int j = 0; j < len-i;j++)
		str+=(*this)[j];

	for (j = len-i; j < len; j++)
		integer+=(*this)[j];

	return atoi(integer.c_str());
}

inline String StringArray::Combine(char c,int start,int stop)
{
	String combined = "";
	
	for (int i = start; i <= stop; i++)
	{
		combined += (*this)[i];
		if (i != stop)
			combined += c;
	}
	return combined;
}

inline void StringArray::Insert(String str,int idx)
{
	m_array.insert(m_array.begin() + idx,str);	
}