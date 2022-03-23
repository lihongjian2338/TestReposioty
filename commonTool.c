namespace

{

	void Char2Hex(unsigned char ch, char* szHex)

	{

		static unsigned char saucHex[] = "0123456789ABCDEF";

		szHex[0] = saucHex[ch >> 4];

		szHex[1] = saucHex[ch & 0xF];

		szHex[2] = 0;

	}

	bool Hex2Char(char const* szHex, unsigned char& rch)

	{

		if (*szHex >= '0' && *szHex <= '9')

			rch = *szHex - '0';

		else if (*szHex >= 'A' && *szHex <= 'F')

			rch = *szHex - 55; //-'A' + 10

		else

			//Is not really a Hex string

			return false;

		szHex++;

		if (*szHex >= '0' && *szHex <= '9')

			(rch <<= 4) += *szHex - '0';

		else if (*szHex >= 'A' && *szHex <= 'F')

			(rch <<= 4) += *szHex - 55; //-'A' + 10;

		else

			//Is not really a Hex string

			return false;

		return true;

	}

	std::string Bin2Hex(const std::string& str, bool bUpper)

	{

		std::string strResult;

		for (size_t i = 0; i < str.length(); i++)

		{

			char szHex[3];

			Char2Hex(str[i], szHex);

			strResult += szHex;

		}

		if (!bUpper)

		{

			std::transform(strResult.begin(), strResult.end(), strResult.begin(), ::tolower);

		}

		return strResult;

	}



	std::string Hex2Bin(const std::string& hex)

	{

		std::string bin;

		std::string hex_upper = hex;

		std::transform(hex_upper.begin(), hex_upper.end(), hex_upper.begin(), ::toupper);

		if (hex_upper.length() % 2 != 0)

		{

			return bin;

		}

		bin.clear();

		for (size_t i = 0; i < hex_upper.length(); i += 2)

		{

			std::string strTmp = hex_upper.substr(i, 2);

			unsigned char ch;

			if (!Hex2Char(strTmp.c_str(), ch))

			{

				return bin;

			}

			bin += ch;

		}

		return bin;

	}



	std::string Encode(const unsigned char *pucData, int nLen)

	{

		const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		

		std::string strEncode;

		unsigned char Tmp[4] = { 0 };

		for (int i = 0; i < nLen / 3; i++)

		{

			Tmp[1] = *pucData++;

			Tmp[2] = *pucData++;

			Tmp[3] = *pucData++;

			strEncode += EncodeTable[Tmp[1] >> 2];

			strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];

			strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];

			strEncode += EncodeTable[Tmp[3] & 0x3F];

		}

		

		int Mod = nLen % 3;

		if (Mod == 1)

		{

			Tmp[1] = *pucData++;

			strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];

			strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];

			strEncode += "==";

		}

		else if (Mod == 2)

		{

			Tmp[1] = *pucData++;

			Tmp[2] = *pucData++;

			strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];

			strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];

			strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];

			strEncode += "=";

		}



		return strEncode;

	}



	std::string Base64Encode(const std::string &strData)

	{

		return Encode((const unsigned char *)strData.c_str(), strData.length());

	}



	INT32 ExtReadFile(const char* file_name, unsigned char* buf, unsigned int* buf_len)

	{

		INT32 nRet = 1;



		std::string strOutput("");



		std::fstream fs;

		fs.open(file_name, std::ios::in | std::ios::binary);

		if (fs.good())

		{

			std::ostringstream tmp;

			tmp << fs.rdbuf();

			strOutput = tmp.str();

			if (*buf_len >= strOutput.length())

			{

				memcpy(buf, strOutput.c_str(), strOutput.length());

				*buf_len = strOutput.length();

				nRet = 0;

			}

		}

		fs.close();



		return nRet;

	}



	INT32 ExtWriteFile(const char* file_name, const unsigned char* buf, unsigned int buf_len)

	{

		INT32 nRet = 1;

		std::string strInput((const char*)buf, buf_len);



		std::fstream fs;

		fs.open(file_name, std::ios::out | std::ios::binary);

		if (fs.good())

		{

			fs << strInput;

			nRet = 0;

		}

		fs.close();



		return nRet;

	}

}
