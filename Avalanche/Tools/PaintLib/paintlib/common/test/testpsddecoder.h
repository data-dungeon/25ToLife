/*
/--------------------------------------------------------------------
|
|      $Id: testpsddecoder.h,v 1.2 2001/10/21 17:54:40 uzadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTPSDDECODER
#define INCL_PLTESTPSDDECODER

#include <string>

class PLTester;
class PLBmp;

class PLTestPSDDecoder
{

public:
  PLTestPSDDecoder (PLTester * pTester);

  virtual ~PLTestPSDDecoder();

  void RunTests ();

private:
  void test (const std::string& sFName);

  PLTester * m_pTester;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: /Avalanche/tools/PaintLib/paintlib/common/test/testpsddecoder.h $
 * 
 * 1     4/11/02 10:37a Adam Clayton
|      Revision 1.2  2001/10/21 17:54:40  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|
\--------------------------------------------------------------------
*/
