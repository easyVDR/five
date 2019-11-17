#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <gtk/gtk.h>

//Prüfen ob die Datei vorhanden ist
bool check_file_exist(char *acFilename)
{
	FILE *pFile=NULL;
	//Testen ob Datei vorhanden ist
	pFile=fopen(acFilename,"r");
	if(pFile!=NULL) 
	{
		fclose(pFile);
		return TRUE;
	}
return FALSE;
}


//Gibt Zeit seit dem 1.1.1970 in Millisekunden zurück
double getTime_ms(void)
{
	struct timeval sTime;
	struct timezone tz;	
	double dTime_ms;
	
	gettimeofday(&sTime, &tz);	
	dTime_ms=((double) sTime.tv_sec * (double)1000);
	dTime_ms+=(sTime.tv_usec/1000);
	return (dTime_ms);
}
