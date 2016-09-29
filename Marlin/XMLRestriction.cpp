/////////////////////////////////////////////////////////////////////////////////
//
// SourceFile: XMLRestriction.cpp
//
// Marlin Server: Internet server/client
// 
// Copyright (c) 2016 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "stdafx.h"
#include "XMLRestriction.h"
#include "CrackURL.h"
#include <stdint.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Do not warn about formatting CStrings
#pragma warning(disable:6284)

XMLRestriction::XMLRestriction(CString p_name)
               :m_name(p_name)
{
}

void
XMLRestriction::AddEnumeration(CString p_enum,CString p_displayValue /*=""*/)
{
  if(!HasEnumeration(p_enum))
  {
    m_enums.insert(std::make_pair(p_enum,p_displayValue));
  }
}

bool 
XMLRestriction::HasEnumeration(CString p_enum)
{
  return m_enums.find(p_enum) != m_enums.end();
}

CString 
XMLRestriction::GiveDisplayValue(CString p_enum)
{
  XmlEnums::iterator it = m_enums.find(p_enum);
  if(it != m_enums.end())
  {
    return it->second;
  }
  return "";
}

// Print restriction as XML comment node
// <!--datatype has values: M (='Male customer') / F (='Female customer') / U (='Unknown')-->
// <!--datatype has values: M / F / U-->
CString
XMLRestriction::PrintEnumRestriction(CString p_name)
{
  bool print = false;
  CString restriction;
  restriction.Format("<!--%s has values: ",p_name);

  for(auto& enumvalue : m_enums)
  {
    if(print) restriction += " / ";
    restriction += enumvalue.first;
    if(!enumvalue.second.IsEmpty())
    {
      restriction += " (='";
      restriction += enumvalue.second;
      restriction += "')";
    }
    print = true;
  }

  restriction += "-->";
  return restriction;
}

// <!--maxLength: 20-->
CString
XMLRestriction::PrintIntegerRestriction(CString p_name,int p_value)
{
  CString restriction;
  restriction.Format("<!--%s: %d-->",p_name,p_value);
  return restriction;
}

// <!--pattern: ab*-->
CString
XMLRestriction::PrintStringRestriction(CString p_name,CString p_value)
{
  CString restriction;
  restriction.Format("<!--%s: %s-->",p_name,p_value);
  return restriction;
}

// <!--whitespace: collapse-->
CString
XMLRestriction::PrintSpaceRestriction()
{
  CString value;

  switch(m_whiteSpace)
  {
    case 1: value = "preserve"; break;
    case 2: value = "replace";  break;
    case 3: value = "collapse"; break;
  }
  return PrintStringRestriction("whitespace",value);
}

CString
XMLRestriction::PrintRestriction(CString p_name)
{
  CString restriction;

  // Print base type
  if(!m_baseType.IsEmpty()) restriction += PrintStringRestriction("base",m_baseType);
  // All integer restrictions
  if(m_length)         restriction += PrintIntegerRestriction("length (EXACT)",m_length);
  if(m_maxLength)      restriction += PrintIntegerRestriction("maxLength",     m_maxLength);
  if(m_minLength)      restriction += PrintIntegerRestriction("minLength",     m_minLength);
  if(m_totalDigits)    restriction += PrintIntegerRestriction("totalDigits",   m_totalDigits);
  if(m_fractionDigits) restriction += PrintIntegerRestriction("fractionDigits",m_fractionDigits);
  // All string restrictions
  if(!m_maxExclusive.IsEmpty()) restriction += PrintStringRestriction("maxExclusive",m_maxExclusive);
  if(!m_maxInclusive.IsEmpty()) restriction += PrintStringRestriction("maxInclusive",m_maxInclusive);
  if(!m_minExclusive.IsEmpty()) restriction += PrintStringRestriction("minExclusive",m_minExclusive);
  if(!m_minInclusive.IsEmpty()) restriction += PrintStringRestriction("minInclusive",m_minInclusive);
  if(!m_pattern     .IsEmpty()) restriction += PrintStringRestriction("pattern",     m_pattern);
  // Whitespace handling
  if(m_whiteSpace) 
  {
    restriction += PrintSpaceRestriction();
  }
  // Print enumerations
  if(!m_enums.empty())
  {
    restriction += PrintEnumRestriction(p_name);
  }
  return restriction;
}

//////////////////////////////////////////////////////////////////////////
//
//  RESTRICTIONS FOR A CLASS OF MESSAGES. I.E. a WSDL registration file
//
//////////////////////////////////////////////////////////////////////////

XMLRestriction* 
XMLRestrictions::FindRestriction(CString p_name)
{
  AllRestrictions::iterator it = m_restrictions.find(p_name);
  if(it != m_restrictions.end())
  {
    return &(it->second);
  }
  return nullptr;
}

XMLRestriction* 
XMLRestrictions::AddRestriction(CString p_name)
{
  XMLRestriction* res = FindRestriction(p_name);
  if(res == nullptr)
  {
    // Insert
    XMLRestriction rest(p_name);
    m_restrictions.insert(std::make_pair(p_name,rest));
    // Re-Find
    res = FindRestriction(p_name);
  }
  return res;
}

void
XMLRestrictions::AddEnumeration(CString p_name,CString p_enum,CString p_displayValue /*=""*/)
{
  XMLRestriction* res = FindRestriction(p_name);
  if(res)
  {
    res->AddEnumeration(p_enum,p_displayValue);
  }
}

bool            
XMLRestrictions::HasEnumeration(CString p_name,CString p_enum)
{
  XMLRestriction* res = FindRestriction(p_name);
  if(res)
  {
    return res->HasEnumeration(p_enum);
  }
  return false;
}

CString
XMLRestrictions::GiveDisplayValue(CString p_name,CString p_enum)
{
  XMLRestriction* res = FindRestriction(p_name);
  if(res)
  {
    return res->GiveDisplayValue(p_name);
  }
  return "";
}

//////////////////////////////////////////////////////////////////////////
//
// CHECKING A RESTRICTION
//
//////////////////////////////////////////////////////////////////////////

CString
XMLRestriction::CheckAnyURI(CString p_value)
{
  CString result;
  CrackedURL url(p_value);
  if(!url.Valid())
  {
    result = "Not a valid URI: " + p_value;
  }
  return result;
}

// Integer of an arbitrary length
CString 
XMLRestriction::CheckInteger(CString p_value)
{
  CString value(p_value);
  value.Trim();
  int ch = value.GetAt(0);
  if(ch == '+' || ch == '-')
  {
    value = value.Mid(1);
  }

  for(int ind = 0;ind < value.GetLength(); ++ind)
  {
    if(!isdigit(value.GetAt(ind)))
    {
      return "Not an integer, but: " + p_value;
    }
  }
  return "";
}

CString
XMLRestriction::CheckBoolean(CString p_value)
{
  p_value.Trim();
  if(!p_value.IsEmpty())
  {
    if(p_value.CompareNoCase("true") &&
       p_value.CompareNoCase("false") &&
       p_value.CompareNoCase("1") &&
       p_value.CompareNoCase("0"))
    {
      CString details("Not a boolean, but: ");
      details += p_value;
      return details;
    }
  }
  return "";
}

CString
XMLRestriction::CheckBase64(CString p_value)
{
  for(int ind = 0; ind < p_value.GetLength(); ++ind)
  {
    int ch = p_value.GetAt(ind);
    if(!isspace(ch) && !isalnum(ch))
    {
      if((ind >= p_value.GetLength() - 2) && ch == '=')
      {
        continue;
      }
      return "Not a base64Binary field";
    }
  }
  return "";
}

CString
XMLRestriction::CheckDouble(CString p_value,bool p_specials)
{
  CString result;
  p_value.TrimLeft('-');
  p_value.TrimLeft('+');

  if(p_specials)
  {
    if(p_value == "INF" || p_value == "NaN")
    {
      return result;
    }
  }
  for(int ind = 0; ind < p_value.GetLength(); ++ind)
  {
    int ch = p_value.GetAt(ind);
    if(!isspace(ch) && !isdigit(ch) && ch != '+' && ch != '-' && toupper(ch) != 'E')
    {
      result  = "Not a number: ";
      result += p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckDatePart(CString p_value)
{
  CString result;
  int dateYear,dateMonth,dateDay;

  p_value.Trim();
  int num = sscanf_s(p_value,"%d-%d-%d",&dateYear,&dateMonth,&dateDay);
  if(num != 3)
  {
    result = "Not a date: ";
    result += p_value;
  }
  else
  {
    if(dateYear  < 0 || dateYear  > 9999 ||
       dateMonth < 1 || dateMonth > 12   ||
       dateDay   < 1 || dateDay   > 31    )
    {
      result = "Date out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckTimeZone(CString p_value)
{
  CString result;
  int pos = p_value.Find(':');
  int hours = 0;
  int minutes = 0;

  if(pos > 0)
  {
    int num = sscanf_s(p_value,"%d:%d",&hours,&minutes);
    if(num != 2)
    {
      result = "Not a timezone hour:min but: " + p_value;
    }
    else
    {
      if(hours > 14 || minutes >= 60)
      {
        result = "Timezone out of range: " + p_value;
      }
    }
  }
  else
  {
    int num = sscanf_s(p_value,"%d",&hours);
    if(num != 1)
    {
      result = "Not a timezone hour but: " + p_value;
    }
    else
    {
      if(hours > 14)
      {
        result = "Timezone out of range: " + p_value;
      }
    }
  }
  return result;
}

// YYYY-MM-DD[[+/-]nn[:nn]]
// YYYY-MM-DD[Znn[:nn]]
CString
XMLRestriction::CheckDate(CString p_value)
{
  CString result;
  int pos1 = p_value.Find('Z');
  int pos2 = p_value.Find('+');
  int pos3 = p_value.Find('-');

  if(pos1 > 0) 
  {
    result += CheckDatePart(p_value.Left(pos1));
    result += CheckTimeZone(p_value.Mid(pos1 + 1));
  }
  else if(pos2 > 0 || pos3 > 0)
  {
    pos1 = max(pos2,pos3);
    result += CheckDatePart(p_value.Left(pos1));
    result += CheckTimeZone(p_value.Mid(pos1 + 1));
  }
  else
  {
    result = CheckDatePart(p_value);
  }
  return result;
}

// YYYY-MM-DDThh:mm:ss
CString
XMLRestriction::CheckStampPart(CString p_value)
{
  CString result;
  int dateYear,dateMonth,dateDay;
  int timeHour,timeMin,timeSec;

  p_value.Trim();
  int num = sscanf_s(p_value,"%d-%d-%dT%d:%d:%d"
                    ,&dateYear,&dateMonth,&dateDay
                    ,&timeHour,&timeMin,  &timeSec);
  if(num != 6)
  {
    result = "Not a dateTime: " + p_value;
  }
  else if(dateYear  < 0 || dateYear  > 9999 ||
          dateMonth < 1 || dateMonth >   12 ||
          dateDay   < 1 || dateDay   >   31 ||
          timeHour  < 0 || timeHour  >   23 ||
          timeMin   < 0 || timeMin   >   59 ||
          timeSec   < 0 || timeSec   >   60  )
  {
    result = "DateTime out of range: " + p_value;
  }
  return result;
}

// YYYY-MM-DDThh:mm:ss[Znn[:nn]]
// YYYY-MM-DDThh:mm:ss[[+/-]nn[:nn]]
CString
XMLRestriction::CheckDateTime(CString p_value,bool p_explicit)
{
  CString result;
  int pos1 = p_value.Find('Z');
  int pos2 = p_value.Find('+');
  int pos3 = p_value.Find('-');

  if(pos1 > 0)
  {
    result += CheckStampPart(p_value.Left(pos1));
    result += CheckTimeZone (p_value.Mid(pos1 + 1));
  }
  else if(pos2 > 0 || pos3 > 0)
  {
    pos1 = max(pos2,pos3);
    result += CheckStampPart(p_value.Left(pos1));
    result += CheckTimeZone (p_value.Mid(pos1 + 1));
  }
  else
  {
    if(p_explicit)
    {
      result = "dayTimeStamp missing an explicit timezone: " + p_value;
    }
    else
    {
      result = CheckStampPart(p_value);
    }
  }
  return result;
}

CString
XMLRestriction::CheckTimePart(CString p_value)
{
  CString result;
  int hour = 0;
  int min  = 0;
  int sec  = 0;

  int num = sscanf_s(p_value,"%d:%d:%d",&hour,&min,&sec);
  if(num != 3)
  {
    result = "Not a time: " + p_value;
  }
  else if(hour < 0 || hour > 23 ||
          min  < 0 || min  > 59 ||
          sec  < 0 || sec  > 60)
  {
    result = "time out of range: " + p_value;
  }
  return result;
}

// HH:MM::SS[Znn[:nn]]
// HH:MM::SS[[+/-]nn[:nn]]
CString
XMLRestriction::CheckTime(CString p_value)
{
  CString result;
  int pos1 = p_value.Find('Z');
  int pos2 = p_value.Find('+');
  int pos3 = p_value.Find('-');

  if(pos1 > 0)
  {
    result += CheckTimePart(p_value.Left(pos1));
    result += CheckTimeZone(p_value.Mid(pos1 + 1));
  }
  else if(pos2 > 0 || pos3 > 0)
  {
    pos1 = max(pos2,pos3);
    result += CheckTimePart(p_value.Left(pos1));
    result += CheckTimeZone(p_value.Mid(pos1 + 1));
  }
  else
  {
    result = CheckTimePart(p_value);
  }
  return result;
}

CString
XMLRestriction::CheckGregDay(CString p_value)
{
  p_value.Trim();
  p_value.Trim('-'); // Up to 2 chars may appear
  int num = atoi(p_value);
  CString result;

  if(num < 1 || num > 31)
  {
    result = "Not a Gregorian day in month: " + p_value;
  }
  return result;
}

CString
XMLRestriction::CheckGregMonth(CString p_value)
{
  p_value.Trim();
  p_value.Trim('-'); // Up to 2 chars may appear
  int num = atoi(p_value);
  CString result;

  if(num < 1 || num > 12)
  {
    result = "Not a Gregorian month in year: " + p_value;
  }
  return result;
}

CString
XMLRestriction::CheckGregYear(CString p_value)
{
  p_value.Trim();
  int num = atoi(p_value);
  CString result;

  if(num < 01 || num > 99)
  {
    result = "Not a Gregorian XML year: " + p_value;
  }
  return result;
}

CString
XMLRestriction::CheckGregMD(CString p_value)
{
  p_value.Trim();
  p_value.TrimLeft('-'); // up to 2 chars may appear
  CString result;
  int month = 0;
  int day   = 0;

  int num = sscanf_s(p_value,"%d-%d",&month,&day);
  if(num != 2)
  {
    result = "Not a Gregorian month-day value: " + p_value;
  }
  else if(month < 1 || month > 12 ||
          day   < 1 || day   > 31 )
  {
    result = "Gregorian month-day overflow: " + p_value;
  }
  return result;
}

CString
XMLRestriction::CheckGregYM(CString p_value)
{
  p_value.Trim();
  CString result;
  int year  = 0;
  int month = 0;

  int num = sscanf_s(p_value,"%d-%d",&year,&month);
  if(num != 2)
  {
    result = "Not a Gregorian year-month value: " + p_value;
  }
  else if(year  < 0 || year  > 9999 ||
          month < 1 || month >   12  )
  {
    result = "Gregorian year-month overflow: " + p_value;
  }
  return result;
}

CString
XMLRestriction::CheckHexBin(CString p_value)
{
  for(int ind = 0; ind < p_value.GetLength(); ++ind)
  {
    int ch = p_value.GetAt(ind);
    if(!isspace(ch) && !isxdigit(ch))
    {
      return "Not a base64Binary field";
    }
  }
  return "";
}

CString
XMLRestriction::CheckLong(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < INT32_MIN || INT32_MAX < val)
    {
      result = "Long int out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckShort(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < INT16_MIN || INT16_MAX < val)
    {
      result = "Short out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckByte(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < INT8_MIN || INT8_MAX < val)
    {
      result = "Byte out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckNNegInt(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < 0 || INT32_MAX < val)
    {
      result = "nonNegativeInteger out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckPosInt(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val <= 0 || INT32_MAX < val)
    {
      result = "positiveInteger out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckUnsLong(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < 0 || UINT32_MAX < val)
    {
      result = "unsignedLong / unsignedInt out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckUnsShort(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < 0 || UINT16_MAX < val)
    {
      result = "unsignedShort out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckUnsByte(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < 0 || UINT8_MAX < val)
    {
      result = "unsignedByte out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckNonPosInt(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < INT32_MIN || 0 < val)
    {
      result = "nonPositiveInteger out of range: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckNegInt(CString p_value)
{
  CString result = CheckInteger(p_value);
  if(result.IsEmpty())
  {
    INT64 val = _atoi64(p_value);
    if(val < INT32_MIN || 0 <= val)
    {
      result = "negativeInteger out of range: " + p_value;
    }
  }
  return result;
}

CString   
XMLRestriction::CheckNormal(CString p_value)
{
  CString result;

  for(int ind = 0;ind < p_value.GetLength(); ++ind)
  {
    int ch = p_value.GetAt(ind);
    if(ch == '\r' || ch == '\n' || ch == '\t')
    {
      result = "normalizedString contains red space: " + p_value;
    }
  }
  return result;
}

CString
XMLRestriction::CheckToken(CString p_value)
{
  CString result = CheckNormal(p_value);

  if(result.IsEmpty())
  {
    if(p_value.Left(1) == " " || p_value.Right(1) == " ")
    {
      result = "token cannot start or end with a space: " + p_value;
    }
    else if(p_value.Find("  ") >= 0)
    {
      result = "token cannot contain separators larger than a space: " + p_value;
    }
  }
  return result;
}

CString   
XMLRestriction::CheckNMTOKEN(CString p_value)
{
  CString result = CheckToken(p_value);

  if(result.IsEmpty())
  {
    for(int ind = 0;ind < p_value.GetLength(); ++ind)
    {
      int ch = p_value.GetAt(ind);
      if(!isalnum(ch) && ch != ':' && ch != '-' && ch != '.' && ch != '_' && ch < 128)
      {
        result = "NMTOKEN with illegal characters: " + p_value;
      }
    }
  }
  return result;
}

CString   
XMLRestriction::CheckName(CString p_value)
{
  CString result = CheckNMTOKEN(p_value);
  if(result.IsEmpty())
  {
    int ch = p_value.GetAt(0);
    if(ch != ':' && !isalpha(ch) && ch != '_' && ch < 128)
    {
      result = "Name should begin with a name-start-character: " + p_value;
    }
  }
  return result;
}

CString   
XMLRestriction::CheckNCName(CString p_value)
{
  CString result = CheckName(p_value);
  if(result.IsEmpty())
  {
    if(p_value.Find(':') >= 0)
    {
      result = "NCName cannot contain a colon: " + p_value;
    }
  }
  return result;
}

// Check "name:name" -> Qualified name
CString   
XMLRestriction::CheckQName(CString p_value)
{
  CString result;
  int pos = p_value.Find(':');

  if(pos < 0)
  {
    return CheckNCName(p_value);
  }
  int pos2 = p_value.Find(':',pos + 1);
  if(pos2 > pos)
  {
    result = "QName cannot have more than one colon: " + p_value;
  }
  else
  {
    CString first  = p_value.Left(pos);
    CString second = p_value.Mid(pos + 1);

    result = CheckNCName(first);
    if(result.IsEmpty())
    {
      result = CheckNCName(second);
    }
  }
  return result;
}

CString   
XMLRestriction::CheckNMTOKENS(CString p_value)
{
  CString result;

  if(p_value.Find("  ") >= 0)
  {
    return "NMTOKENS contains seperators larger than a space: " + p_value;
  }

  p_value.Trim();
  int pos = p_value.Find(' ');

  while(pos > 0)
  {
    CString token = p_value.Left(pos);
    p_value = p_value.Mid(pos + 1);

    result = CheckNMTOKEN(token);
    if(!result.IsEmpty())
    {
      result = "NMTOKENS: " + result;
      return result;
    }
    // Next token
    pos = p_value.Find(' ');
  }
  return CheckNMTOKEN(p_value);
}

CString
XMLRestriction::CheckNames(CString p_value)
{
  CString result;

  if(p_value.Find("  ") >= 0)
  {
    return "ENTITIES/IDREFS contains seperators larger than a space: " + p_value;
  }

  p_value.Trim();
  int pos = p_value.Find(' ');

  while(pos > 0)
  {
    CString token = p_value.Left(pos);
    p_value = p_value.Mid(pos + 1);

    result = CheckName(token);
    if(!result.IsEmpty())
    {
      result = "ENTITIES/IDREFS: " + result;
      return result;
    }
    // Next token
    pos = p_value.Find(' ');
  }
  return CheckName(p_value);
}

// [-] P nY[nM[nD]]  [time part]
// [-] P nM[nD]      [time part]
// [-] P nD          [time part]
// [-] P T nH[nM[nS[.S]]]
// [-] P T nM[nS[.S]]
// [-] P T nS[.S]
CString
XMLRestriction::CheckDuration(CString p_value)
{
  CString result;
  CString value(p_value);
  int year = 0, month = 0, day = 0;
  int hour = 0, min   = 0, sec = 0;
  int ch = 0;

  value.Trim();
  // Check for legal negative value
  if(value.GetAt(0) == '-')
  {
    value = value.Mid(1);
  }
  // We must now find a 'P' (period)
  if(value.GetAt(0) != 'P')
  {
    return "duration has no starting P(eriod) marker: " + p_value;
  }
  value = value.Mid(1);

  // Find 'T' marker?
  if(value.GetAt(0) != 'T')
  {
    // Must find year-month-day
    day = atoi(value);
    while(isdigit(value.GetAt(0))) value = value.Mid(1);
    ch = value.GetAt(0);
    if(ch == 'D')
    {
      value = value.Mid(1);
    }  
    else
    {
      if(ch == 'M')
      {
        month = day;
        value = value.Mid(1);
        if(!value.IsEmpty())
        {
          day = atoi(value);
          while(isdigit(value.GetAt(0))) value = value.Mid(1);
          ch = value.GetAt(0);
          if(ch == 'D')
          {
            value = value.Mid(1);
          }
          else
          {
            return "duration with incorrect PnMnD value: " + p_value;
          }
        }
      }
      else if(ch == 'Y')
      {
        year = day;
        value = value.Mid(1);
        if(!value.IsEmpty())
        {
          month = atoi(value);
          while(isdigit(value.GetAt(0))) value = value.Mid(1);
          ch = value.GetAt(0);
          if(ch == 'M')
          {
            value = value.Mid(1);
            if(!value.IsEmpty())
            {
              day = atoi(value);
              while(isdigit(value.GetAt(0))) value = value.Mid(1);
              ch = value.GetAt(0);
              if(ch == 'D')
              {
                value = value.Mid(1);
              }
              else
              {
                return "duration with incorrect PnYnMnD value: " + p_value;
              }
            }
          }
          else
          {
            return "duration with incorrect PnYnM value: " + p_value;
          }
        }
      }
      else
      {
        return "duration with incorrect PnX value: " + p_value;
      }
    }
  }

  if(value.IsEmpty())
  {
    return result;
  }

  // Time marker to find
  if(value.GetAt(0) != 'T')
  {
    return "duration missing T(ime) marker: " + p_value;
  }
  value = value.Mid(1);

  sec = atoi(value);
  while(isdigit(value.GetAt(0))) value = value.Mid(1);
  ch = value.GetAt(0);
  if(ch == '.')
  {
    value = value.Mid(1);
  }
  else
  {
    if(ch == 'M')
    {
      min = sec;
      value = value.Mid(1);
      if(!value.IsEmpty())
      {
        sec = atoi(value);
        while(isdigit(value.GetAt(0))) value = value.Mid(1);
        ch = value.GetAt(0);
        if(ch == '.')
        {
          value = value.Mid(1);
        }
        else if(ch == 'S')
        {
          goto endcheck;
        }
      }
    }
    else
    {
      if(ch == 'H')
      {
        hour = sec;
        value = value.Mid(1);
        min = atoi(value);
        while(isdigit(value.GetAt(0))) value = value.Mid(1);
        ch = value.GetAt(0);
        if(ch == 'M')
        {
        
        }
      }
      else
      {
        return "duration with wrong time TnX: " + p_value;
      }
    }
  }
  // Seconds fractions to find
  while(isdigit(value.GetAt(0))) value = value.Mid(1);
endcheck:
  if(value != "S")
  {
    return "duration with residue characters: " + p_value;
  }
  return "";
}

CString
XMLRestriction::CheckDatatype(XmlDataType p_type,CString p_value)
{
  CString result;

  // Empty value, nothing to check
  if(p_value.IsEmpty())
  {
    return result;
  }

  // Checking only base datatypes
  // String and CDATA are never checked!
  switch(p_type & XDT_Mask)
  {
    case XDT_AnyURI:        result = CheckAnyURI   (p_value);       break;
    case XDT_Base64:        result = CheckBase64   (p_value);       break;
    case XDT_Boolean:       result = CheckBoolean  (p_value);       break;
    case XDT_Date:          result = CheckDate     (p_value);       break;
    case XDT_Integer:       result = CheckInteger  (p_value);       break;
    case XDT_Decimal:       result = CheckDouble   (p_value,false); break;
    case XDT_Double:        result = CheckDouble   (p_value,true);  break;
    case XDT_DateTime:      result = CheckDateTime (p_value,false); break;
    case XDT_DateTimeStamp: result = CheckDateTime (p_value,true);  break;
    case XDT_Float:         result = CheckDouble   (p_value,true);  break;
    case XDT_GregDay:       result = CheckGregDay  (p_value);       break;
    case XDT_GregMonth:     result = CheckGregMonth(p_value);       break;
    case XDT_GregYear:      result = CheckGregYear (p_value);       break;
    case XDT_GregMonthDay:  result = CheckGregMD   (p_value);       break;
    case XDT_GregYearMonth: result = CheckGregYM   (p_value);       break;
    case XDT_HexBinary:     result = CheckHexBin   (p_value);       break;
    case XDT_Long:          result = CheckLong     (p_value);       break;
    case XDT_Int:           result = CheckLong     (p_value);       break;
    case XDT_Short:         result = CheckShort    (p_value);       break;
    case XDT_NNegInteger:   result = CheckNNegInt  (p_value);       break;
    case XDT_PosInteger:    result = CheckPosInt   (p_value);       break;
    case XDT_UnsignedLong:  result = CheckUnsLong  (p_value);       break;
    case XDT_UnsignedInt:   result = CheckUnsLong  (p_value);       break;
    case XDT_UnsignedShort: result = CheckUnsShort (p_value);       break;
    case XDT_UnsignedByte:  result = CheckUnsByte  (p_value);       break;
    case XDT_NPosInteger:   result = CheckNonPosInt(p_value);       break;
    case XDT_NegInteger:    result = CheckNegInt   (p_value);       break;
    case XDT_Time:          result = CheckTime     (p_value);       break;
    case XDT_Token:         result = CheckToken    (p_value);       break;
    case XDT_NMTOKEN:       result = CheckNMTOKEN  (p_value);       break;
    case XDT_Name:          result = CheckName     (p_value);       break;
    case XDT_ENTITY:        result = CheckName     (p_value);       break;
    case XDT_ID:            result = CheckName     (p_value);       break;
    case XDT_IDREF:         result = CheckName     (p_value);       break;
    case XDT_QName:         result = CheckQName    (p_value);       break;
    case XDT_NMTOKENS:      result = CheckNMTOKENS (p_value);       break;
    case XDT_ENTITIES:      result = CheckNames    (p_value);       break;
    case XDT_IDREFS:        result = CheckNames    (p_value);       break;
    case XDT_Duration:      result = CheckDuration (p_value);       break;
    case XDT_DayTimeDur:    result = CheckDuration (p_value);       break;
    case XDT_YearMonthDur:  result = CheckDuration (p_value);       break;
    default:                break;
  }
  return result;
}

CString 
XMLRestriction::CheckRestriction(XmlDataType p_type,CString p_value)
{
  CString result;

  // Checking other restrictions
  if(m_length)
  {
    // Exact length required
    if(p_value.GetLength() != m_length)
    {
      result.Format("Field length not exactly: %d",m_length);
    }
  }
  if(m_maxLength)
  {
    // Maximum size of the field
    if(p_value.GetLength() > m_maxLength)
    {
      result.Format("Field too long. Longer than: %d",m_maxLength);
    }
  }
  if(m_minLength)
  {
    // Minimum size of the field
    if(p_value.GetLength() < m_minLength)
    {
      result.Format("Field is too short. Shorter than: %d",m_minLength);
    }
  }

  // Notation datatype must have a enumerator list
  if(p_type == XDT_NOTATION)
  {
    if(m_enums.empty())
    {
      result = "NOTATION must declare an enumarator list of QNames";
      return result;
    }
    for(auto& value : m_enums)
    {
      result = CheckQName(value.first);
      if(!result.IsEmpty())
      {
        return result;
      }
    }
  }

  if(m_enums.empty())
  {
    return result;
  }
  // See if the value is one of the stated enum values
  for(auto& value : m_enums)
  {
    if(p_value.CompareNoCase(value.first) == 0)
    {
      return result;
    }
  }
  result.Format("Field value [%s] is not in the list of allowed enumeration values.",p_value);
  return result;

  // YET TO BE IMPLEMENTED
  // totalDigits
  // fractionDigits
  // maxExclusive
  // maxInclusive
  // minExclusive
  // minInclusive
}
