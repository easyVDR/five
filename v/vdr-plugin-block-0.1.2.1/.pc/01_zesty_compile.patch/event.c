/**
 * based on event.c,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#include <ctype.h>
#include <vdr/status.h>
#include "event.h"
#include "setup.h"


char* cEventBlock::duptolower(const char *s) {
	char *c = strdup(s);
	char *p = c;
	for (; *p; ++p)
		*p = tolower(*p);
	return c;
}

const char* cEventBlock::LastTitle="just initialized";
const bool* cEventBlock::ReplayingRecording=false;

cEventsBlock EventsBlock;

cEventBlock::cEventBlock(void):
		mRegularExp(false),
		mIgnoreCase(false),
		mCompiled(false),
		whitelisted(false)
{
  strncpy(mPattern, tr("New Entry"), sizeof(mPattern));
}

cEventBlock::cEventBlock(const char *Pattern):
		mRegularExp(false),
		mIgnoreCase(false),
		mCompiled(false),
		whitelisted(false)
{
  strncpy(mPattern, Pattern, sizeof(mPattern));
}

cEventBlock::cEventBlock(const cEventBlock &Src)
{
	operator=(Src);
}

cEventBlock &cEventBlock::operator=(const cEventBlock &Src)
{
	printf("copy construct\n");
	strcpy(mPattern, Src.mPattern);
	mRegularExp = Src.mRegularExp;
	mIgnoreCase = Src.mIgnoreCase;
	whitelisted = Src.whitelisted;
	mCompiled   = false;
	Compile();
	return *this;
}

/*
bool cEventBlock::operator==(const cEventBlock Src) //include whitelisting?
{
 bool b1=(strcmp(duptolower(mPattern),duptolower(Src.mPattern))==0);
 bool b2=(mRegularExp==Src.mRegularExp);
 bool b3=(mIgnoreCase==Src.mIgnoreCase);
 return b1&&b2&&b3;
}
*/

cEventBlock::~cEventBlock()
{
	if (mRegularExp)
		regfree(&mExpression);
}


bool cEventBlock::doesMatch(const char *Event) const 
{//in fact here we only check if the current event has any match in the list
 //of course Event is examined according to the rules defined in the blacklist
	if (mRegularExp)
		return !(regexec(&mExpression, Event, 0, NULL, 0) != 0);
	if (strlen(mPattern)==0) //workaround for empty patterns
	{                      
	 return (strlen(Event)==0);
	}
	if (mIgnoreCase) 
	{
		char *ev = cEventBlock::duptolower(Event);
		char *pa = cEventBlock::duptolower(mPattern);
		printf("check for %s in %s\n", pa, ev);
		bool res = strstr(ev, pa) == NULL;
		free(ev); free(pa);
		return !res;
	} else
		return !(strstr(Event, mPattern) == NULL);
}

bool cEventBlock::Parse(char *s) 
{
 char *patternbuf=NULL;
 int fields=sscanf(s,"%d:%d:%a[^\n]", &mRegularExp, &mIgnoreCase, &patternbuf);

 if (!patternbuf) return true; 

 if (fields==3)
 {
  strncpy(mPattern,patternbuf,sizeof(mPattern));
  char c=NULL;
  int fields2=0;  
  
  if (strcmp(patternbuf,"-:")==0)//workaround if pattern empty
  {
   fields2=2;
   c='-';
   patternbuf=(char*)"";
  }
  else
  {
   if (strcmp(patternbuf,"+:")==0)
   {
    fields2=2;
    c='+';
    patternbuf=(char*)""; 
   }                      //end of workaround
   else 
   {
    free(patternbuf); //normal entry handling
    fields2=sscanf(mPattern,"%1c:%a[^\n]",&c,&patternbuf);
   }
  }
  
  if (fields2==2)
  {
   if(c=='+')
   {
    whitelisted=true;
   }
   else whitelisted=false;
   strncpy(mPattern,skipspace(stripspace(patternbuf)),sizeof(mPattern));   
  }
  else 
  {
   whitelisted=false;
  }
 }  
 else 
 { // old taste backward compatibility
  strncpy(mPattern, skipspace(stripspace(s)), sizeof(mPattern));
  mRegularExp = false;
  mIgnoreCase = false;
  whitelisted = false;
 }
 return Compile();
}

bool cEventBlock::Compile(void) {
	mCompiled = false;
	if (mRegularExp) {
		if (regcomp(&mExpression, mPattern, REG_EXTENDED | (mIgnoreCase ? REG_ICASE : 0)) != 0) {
			esyslog("ERROR: malformed regular expression: %s", mPattern);
			return false;
		} else
			mCompiled = true;
	}
	return true;
}

bool cEventBlock::Save(FILE *f) {
  char c='c';
  if (whitelisted) c='+';
  else c='-';
  return fprintf(f, "%d:%d:%c:%s\n", mRegularExp, mIgnoreCase, c, mPattern) > 0;
}


bool cEventsBlock::Acceptable(const char *src)
{
 cVector<cEventBlock*> temp_vec (5);
 cEventBlock stringmatch=*(new cEventBlock("nostringmatch"));

 ListMatches(src,&temp_vec,stringmatch);
 if(strcmp(stringmatch.Pattern(),"nostringmatch")!=0)
 {
#ifdef LOGGING
  dsyslog("plugin-block: exact string match found %s: ",stringmatch.Pattern());
#endif
  if (stringmatch.isWhitelisted())
  {
#ifdef LOGGING
   dsyslog("plugin-block: string match entry is whitelisted - returning acceptable");
#endif   
    cSetupBlock::LastAcceptableChannel=cDevice::CurrentChannel();
    return true;
  }
#ifdef LOGGING
  dsyslog("plugin-block: string match entry is not whitelisted - returning not acceptable");
#endif     
  return false;
 }

 int vec_size=temp_vec.Size();
 if (vec_size<1)  
 {
#ifdef LOGGING
  dsyslog("plugin-block: no matching pattern found at all - returning acceptable");
#endif
  cSetupBlock::LastAcceptableChannel=cDevice::CurrentChannel();
  return true;
 }
 
 bool b=temp_vec.At(0)->isWhitelisted();
 int count=1;
 for(;count<vec_size;count++)
 {
  if (temp_vec.At(count)->isWhitelisted()!=b)
  {
#ifdef LOGGING
   dsyslog("plugin-block: Contradictory rules detected - returning fuzzy fallback: %d",cSetupBlock::FuzzyFallback);
#endif   
   b=cSetupBlock::FuzzyFallback;
   if (b) cSetupBlock::LastAcceptableChannel=cDevice::CurrentChannel();
   return b;
  }
 }
#ifdef LOGGING
 dsyslog("plugin-block: Acceptable returning:  %d",b);
#endif 
 if (b) cSetupBlock::LastAcceptableChannel=cDevice::CurrentChannel();
 return b;
}
 
 

cEventsBlock &cEventsBlock::operator=(const cEventsBlock &Source) {
  cList<cEventBlock>::Clear();
  
  const cEventBlock *event = Source.First();
  while (event != NULL) {
    printf("transferring %p\n", event);
    Add(new cEventBlock(*event));
    event = Source.Next(event);
  }
  return *this;
}

void cEventsBlock::ListMatches(const char *title, cVector <cEventBlock*> *match_vec, cEventBlock &stringmatch) 
{
  cEventBlock *event = First();
  char* temp_title=strdup(title);  
  char* temp_pattern=NULL;  
  while (event != NULL) {
    if (event->doesMatch(title)) //doesMatch examines if there is a match according to all properties of the blacklist entry
    {
     #ifdef LOGGING
     char *condition=(char*)(event->isWhitelisted() ? "positive" : "negative");
     dsyslog("plugin-block: Matching rule detected - Pattern: '%s' (%s)",event->Pattern(),condition);
     #endif
     match_vec->Append(event); //put match in the vector; of course there could be more than one match
                               //lets say you would block everything containing the word 'Judge'
                               //but 'Judge Dredd' also has an explicit entry
                               //in case Judge Dredd is the current show then both entries would match
                               //therefore it is necessary to do further analysis of the type of the matches
                               //or the according rules and priorities respectively

     if (event->IgnoreCase())//now we check if there the current match is also a stringmatching entry
                            //first check if ignore case is set
                            //if so replace the strings to be compared by their lower case pendants
                            //so next we only have to do one comparison
     {
      temp_title=cEventBlock::duptolower(temp_title);
      temp_pattern=cEventBlock::duptolower(event->Pattern());
     }
     else 
     {
      temp_pattern=(char*)event->Pattern();
     }
     if (strcmp(temp_title,temp_pattern)==0) //check for stringmatch
     {
#ifdef LOGGING
      dsyslog("plugin-block: ListMatches: stringmatch found '%s'",event->Pattern());
#endif      
      stringmatch=*event;
     }
     if (event->IgnoreCase())
     {
       free(temp_title);
       free(temp_pattern);
     }
    }
    event = Next(event);
  }
}

void cEventsBlock::ListMatches(const char *title, cVector <cEventBlock*> *match_vec) 
{
  cEventBlock *event = First();
  while (event != NULL) {
    if (event->doesMatch(title))
    {
     match_vec->Append(event);
    }
    event = Next(event);
  }
}


cEventBlock* cEventsBlock::hasStringMatch(const char* src, bool ignorecase)
{
 cEventBlock *event_ptr=First();
 char *low_src=cEventBlock::duptolower(src);
 while (event_ptr!=NULL)
 {
  if (event_ptr->IgnoreCase() || ignorecase)
  {
   char *low_pattern=cEventBlock::duptolower(event_ptr->Pattern());
   if (strcmp(low_src,low_pattern)==0)
   {
    return event_ptr;
   }
  }
  else
  {
   if (strcmp(src,event_ptr->Pattern())==0)
   {
    return event_ptr;
   }
  }
  event_ptr=Next(event_ptr);
 }
 return NULL;
}
   

int cEventsBlock::getIndexOf(cEventBlock *src)
{
 //TODO make this a recursive method taking a list pointer and lets begin search in the middle of the list
 //(middle->Previous() and middle->Next())
 //find out if the first char of Pattern is less or higher than src.char etc
 //once the first char of src matches begin search there straight forward
 //this might speedup setting cursor to the right position (New entry) in the OSD
 //NO: it turned out that the Get method of cListBase will have a serious impact on this solution
 //maybe a major rewrite and/or use of STL implementations of lists or vectors etc would be a better alternative

 cEventBlock* listptr=First();
 int list_count=0;
 const char* src_pattern=src->Pattern();

 while(listptr!=NULL)
 {
  if (strcmp(src_pattern,listptr->Pattern())==0)
  {
   return list_count;
  }
  list_count+=1;
  listptr=Next(listptr);
 }
 return -1;
}

