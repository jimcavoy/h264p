#include "MDPMParser.h"

#include <iterator>
#include <memory.h>
#include <string>


#ifdef _WIN32
#define sprintf sprintf_s
#endif


using namespace std;
using namespace ThetaStream;

namespace
{

std::string month(char m)
{
	switch(m)
	{
	case 0x01: return string("Jan");
	case 0x02: return string("Feb");
	case 0x03: return string("Mar");
	case 0x04: return string("Apr");
	case 0x05: return string("May");
	case 0x06: return string("Jun");
	case 0x07: return string("Jul");
	case 0x08: return string("Aug");
	case 0x09: return string("Sep");
	case 0x10: return string("Oct");
	case 0x11: return string("Nov");
	case 0x12: return string("Dec");
	}

	return string("bad value");
}

template< class Iterator>
bool isMDPM(Iterator first, Iterator last)
{
	std::vector<char> UUID(16);
	char temp[16] = { (char)0x17, (char)0xee, (char)0x8c, (char)0x60,
		(char)0xf8, (char)0x4d, (char)0x11, (char)0xd9,
		(char)0x8c, (char)0xd6, (char)0x08, (char)0x00,
		(char)0x20, (char)0x0c, (char)0x9a, (char)0x66};
	UUID.assign(&temp[0], &temp[0]+16);

	return std::equal(first,last,UUID.begin());
}

void printout(json::Array& elmts, unsigned char tag, const char* name, const char* value)
{
	char t[16];
	sprintf(t,"%#02x", tag);
	json::Object exif;

	exif["tag"] = json::String(t);
	exif["name"] = json::String(name);
	exif["value"] = json::String(value);

	elmts.Insert(exif);
}

float rational2float(const char ur[4])
{
	float ret = 0.0;

	char cNum[2]{};
	char cDom[2]{};
	cNum[1] = ur[0];cNum[0] = ur[1];
	cDom[1] = ur[2];cDom[0] = ur[3];
	int num = 0;
	int dom = 0;

	memcpy(&num,(void*)cNum,2);
	memcpy(&dom,(void*)cDom,2);

	ret = (float)num/(float)dom;
	return ret;
}

template<class Iterator >
std::string createUUID(Iterator first, Iterator last)
{
	std::string ret;
	char c[BUFSIZ];
	Iterator it = first;

	for(size_t i = 0; it != last; ++it, ++i)
	{
		sprintf(c,"%02x", (unsigned char)*it);
		ret += c;
		if(i == 3 || i == 5 || i == 7 || i == 9)
			ret += "-";
	}

	return ret;
}


}// unnamed namespace


/////////////////////////////////////////////////////////////////////////////
// MDPMParser
MDPMParser::MDPMParser(json::Object& nalu)
	:nalu_(nalu)
{
}


MDPMParser::~MDPMParser(void)
{
}

void MDPMParser::Visit(SEIUserDataUnreg& sei)
{
	std::string strUUID;
	json::Object seiObj;
	seiObj["type"] = json::String("User Data (unregistered)");

	SEIimpl::RawByteStreamPayload uuid;
	sei.uuid(std::back_inserter(uuid));
	SEIimpl::RawByteStreamPayload ud;
	sei.payload(std::back_inserter(ud));

	char name[5]{};

	if(ud.size() > 21)
	{
		size_t payloadSz = sei.payloadSize();
		seiObj["payloadSize"] = json::Number((unsigned) payloadSz);
		strUUID = createUUID(uuid.begin(),uuid.end());
		seiObj["uuid_iso_iec_11578"] = json::String(strUUID);

		if(isMDPM(uuid.begin(),uuid.end()))
		{
			json::Object mdpmObj;
			json::Array exifElmts;

			name[0] = ud[16]; name[1] = ud[17]; name[2] = ud[18]; name[3] = ud[19];
			mdpmObj["Name"] = json::String(name);

			int n = 0;
			char t = 0x00;
			char v[4]; memset(v,0,4);
			size_t i = 21;
			while(i < payloadSz)
			{
				int mod = n%5;

				switch(mod)
				{
				case 0: t = ud[i++]; break;
				case 1: v[mod-1] = ud[i++]; break;
				case 2: v[mod-1] = ud[i++]; break;
				case 3: v[mod-1] = ud[i++]; break;
				case 4: 
					{
					v[mod-1] = ud[i++];
					MDPMElement e(t,v);
					elements_.push_back(e);
					}break;
				}
				n++;
			}

			char value[BUFSIZ];
			MDPMParser::iterator it;
			for(it = elements_.begin(); it != elements_.end(); ++it)
			{
				int t=0;
				memset(&t,it->tag(),1);
				const char* v = it->value();
				switch(t)
				{
				case 0x18: {
					string mon = month(v[3]);
					sprintf(value,"%x%x %s", v[1],v[2],mon.c_str());
					printout(exifElmts,t,"DateTimeOriginal",value);
					}break;
				case 0x19: {
					sprintf(value,"%x day, %x:%x:%x",v[0],v[1],v[2],v[3]);
					printout(exifElmts,t,"DateTimeOriginal",value);
					}break;
				case 0x70: {
					sprintf(value,"%02x %02x %02x %02x",(unsigned char)v[0],(unsigned char)v[1],(unsigned char)v[2],(unsigned char)v[3]);
					printout(exifElmts,t,"Camera1",value);
					}break;
				case 0x71: {
					sprintf(value,"%02x %02x %02x %02x",(unsigned char)v[0],(unsigned char)v[1],(unsigned char)v[2],(unsigned char)v[3]);
					printout(exifElmts,t,"Camera2",value);
					}break;
				case 0x7f: {
					sprintf(value,"%02x %02x %02x %02x",(unsigned char)v[0],(unsigned char)v[1],(unsigned char)v[2],(unsigned char)v[3]);
					printout(exifElmts,t,"Shutter",value);
					}break;
				case 0xa1: {
					const char* v = it->value();
					float deg = rational2float(v);
					sprintf(value,"%.2f",deg);
					printout(exifElmts,t,"FNumber",value);
					}break;
				case 0xb0: {
					sprintf(value,"%x.%x.%x.%x",v[0],v[1],v[2],v[3]);
					printout(exifElmts,t,"GPSVersionID",value);
					}break;
				case 0xb1: {
					sprintf(value,"%c",v[0]);
					printout(exifElmts,t,"GPSLatitudeRef",value);
					}break;
				case 0xb2: {
					const char* v = it->value();
					float deg = rational2float(v);
					sprintf(value,"%.0f degrees",deg);
					printout(exifElmts,t,"GPSLatitude",value);
					}break;
				case 0xb3: {
					float deg = rational2float(v);
					sprintf(value,"%.0f minutes",deg);
					printout(exifElmts,t,"GPSLatitude",value);
					}break;
				case 0xb4: {
					const char* v = it->value();
					float deg = rational2float(v);
					sprintf(value,"%.3f seconds",deg);
					printout(exifElmts,t,"GPSLatitude",value);
					}break;
				case 0xb5: {
					sprintf(value,"%c",v[0]);
					printout(exifElmts,t,"GPSLongitudeRef",value);
					}break;
				case 0xb6: {
					float deg = rational2float(v);
					sprintf(value,"%.0f degrees",deg);
					printout(exifElmts,t,"GPSLongitude",value);
					}break;
				case 0xb7: {
					float deg = rational2float(v);
					sprintf(value,"%.0f minutes",deg);
					printout(exifElmts,t,"GPSLongitude",value);
					}break;
				case 0xb8: {
					float deg = rational2float(v);
					sprintf(value,"%.3f seconds",deg);
					printout(exifElmts,t,"GPSLongitude",value);
					}break;
				case 0xb9: {
					int flag;
					memcpy(&flag,(void*)v,4);
					string strVal = flag ? "Below Sea Level" : "Above Sea Level";
					sprintf(value,"%s",strVal.c_str());
					printout(exifElmts,t,"GPSAltitudeRef",value);
					}break;
				case 0xba: {
					float alt = rational2float(v);
					sprintf(value,"%.3f meters",alt);
					printout(exifElmts,t,"GPSAltitude",value);
					}break;
				case 0xbb: {
					float h = rational2float(v);
					sprintf(value,"%.0f hours",h);
					printout(exifElmts,t,"GPSTimeStamp",value);
					}break;
				case 0xbc: {
					float m = rational2float(v);
					sprintf(value,"%.0f minutes",m);
					printout(exifElmts,t,"GPSTimeStamp",value);
					}break;
				case 0xbd: {
					float s = rational2float(v);
					sprintf(value,"%.0f seconds",s);
					printout(exifElmts,t,"GPSTimeStamp",value);
					}break;
				case 0xbe: {
					string s;
					switch(v[0])
					{
					case 'A': s = "Measurement Active"; break;
					case 'V': s = "Measurement Void"; break;
					}
					sprintf(value,"%s", s.c_str());
					printout(exifElmts,t,"GPSStatus",value);
					}break;
				case 0xbf: {
					string s;
					switch(v[0])
					{
					case 3: s = "3-Dimensional Measurement"; break;
					case 2: s = "2-Dimensional Measurement"; break;
					default: s = "Unknown";
					}
					sprintf(value,"%s", s.c_str());
					printout(exifElmts,t,"GPSMeasureMode",value);
					}break;
				case 0xc0: {
					float s = rational2float(v);
					sprintf(value,"%.4f",s);
					printout(exifElmts,t,"GPSDOP",value);
					}break;
				case 0xc1: {
					string ref;
					switch(v[0])
					{
					case 'K': ref = "km/h"; break;
					case 'M': ref = "mph"; break;
					case 'N': ref = "knots"; break;
					}

					sprintf(value,"%s",ref.c_str());
					printout(exifElmts,t,"GPSSpeedRef",value);
					}break;
				case 0xc2: {
					float s = rational2float(v);
					sprintf(value,"%.3f",s);
					printout(exifElmts,t,"GPSSpeed",value);
					}break;
				case 0xc3: {
					string ref;
					switch(v[0])
					{
					case 'M': ref = "Magnetic North"; break;
					case 'T': ref = "True North"; break;
					}
					sprintf(value,"%s",ref.c_str());
					printout(exifElmts,t,"GPSTrackRef",value);
					}break;
				case 0xc4: {
					float s = rational2float(v);
					sprintf(value,"%.3f degrees",s);
					printout(exifElmts,t,"GPSTrack",value);
					}break;
				case 0xc5: {
					string ref;
					switch(v[0])
					{
					case 'M': ref = "Magnetic North"; break;
					case 'T': ref = "True North"; break;
					}
					sprintf(value,"%s",ref.c_str());
					printout(exifElmts,t,"GPSImgDirectionRef",value);
					}break;
				case 0xc6: {
					float s = rational2float(v);
					sprintf(value,"%.3f degrees",s);
					printout(exifElmts,t,"GPSImgDirection",value);
					}break;
				case 0xc7: 
				case 0xc8: {
					sprintf(value,"%c%c%c%c",v[0],v[1],v[2],v[3]);
					printout(exifElmts,t,"GPSMapDatum",value);
					}break;
				case 0xe0: {
					string make;
					short m;
					char temp[2]{};
					temp[1] = v[0]; //big endian to little endian
					temp[0] = v[1];
					memcpy(&m,temp,2);
					switch(m)
					{
					case 0x0103: make="Panasonic"; break;
					case 0x0108: make="Sony"; break;
					case 0x1011: make="Canon"; break;
					}
					sprintf(value,"%s",make.c_str());
					printout(exifElmts,t,"MakeModel",value);
					}break;
				case 0xe4:
				case 0xe5:
				case 0xe6: {
					sprintf(value,"%c%c%c%c",v[0],v[1],v[2],v[3]);
					printout(exifElmts,t,"Model",value);
					}break;
				default: {
					memset(value,0,BUFSIZ);
					sprintf(value,"%02x %02x %02x %02x",(unsigned char)v[0],(unsigned char)v[1],(unsigned char)v[2],(unsigned char)v[3]);
					printout(exifElmts,t,"Unknown",value);
					}
				} //switch
			} // for
			mdpmObj["EXIFElements"] = exifElmts;
			seiObj["user_data_payload_byte"] = mdpmObj;
		} // if
		else
		{
			
			std::string val;
			// start at index 16 to skip over the UUID
			for(size_t n = 16; n < payloadSz; ++n)
			{
				val += ud[n];
			}
			seiObj["user_data_payload_byte"] = json::String(val);
		}
	}

	nalu_["SEI"] = seiObj;
}

	