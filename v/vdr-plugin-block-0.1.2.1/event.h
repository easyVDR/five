/**
 * based on event.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_EVENT_H
#define VDR_BLOCK_EVENT_H

#include <regex.h>
#include <vdr/config.h>

#define EVLINELENGTH 256

class cEventBlock : public cListObject {
  friend class cMenuSetupEditBlock;

private:
  char mPattern[EVLINELENGTH];
	int  mRegularExp;
	int  mIgnoreCase;
	bool mCompiled;
        int whitelisted;
        
	regex_t mExpression;

public:
  cEventBlock(void);
  cEventBlock(const char *Pattern);
	cEventBlock(const cEventBlock &Src);
	~cEventBlock();

	cEventBlock &operator=(const cEventBlock &Src);
	bool operator==(const cEventBlock Src);
	

  bool doesMatch(const char *Event) const ;
  const int isWhitelisted(void) const {return whitelisted;}
  void setWhitelisted(const bool b) {whitelisted=b;}
  bool IgnoreCase(void) const {return mIgnoreCase;}
  void setIgnoreCase(const bool b) {mIgnoreCase=b;}

  bool Parse(char *s);
	bool Compile(void);
  bool Save(FILE *f);
  static const char *LastTitle;

  static const bool *ReplayingRecording;
  
  static char *duptolower(const char*);

  const char *Pattern(void) const { return mPattern; }

  virtual int Compare(const cListObject &src) const 
  {
   cEventBlock* rhs=(cEventBlock*)&src;
   char* l=duptolower(mPattern);
   char* r=duptolower(rhs->mPattern);
   int ret=strcmp(l,r);
   free(l); free(r);
   return ret; }//TODO check if the code relies on this because in fact we have to return !strcmp(r,l) here!!!
};

class cEventsBlock : public cConfig<cEventBlock> {
private:
//  int recursive_pos_seek (char *pattern, int start);        	
  
public:
  bool Acceptable(const char *Event);
  cEventsBlock &operator=(const cEventsBlock &Source);
  void ListMatches (const char *src, cVector <cEventBlock*> *match_vec, cEventBlock &stringmatch);
  void ListMatches (const char *src, cVector <cEventBlock*> *match_vec);
  cEventBlock* hasStringMatch(const char* src, bool ignorecase=false);
  int getIndexOf(cEventBlock *Src);
};

extern cEventsBlock EventsBlock;

#endif // VDR_BLOCK_EVENT_H
