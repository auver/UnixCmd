const int PATH_SIZE=260;
const int FILE_COUNT=6553;
const int FILE_BUF=512;
int fileInfoCmp( const void * , const void * );
class Command
{
public:
	Command();
	~Command();
	void ls( int argc, char *argv[] );
	void cat( int argc, char *argv[] );
	void cp( int argc, char *argv[] );
	void mv( int argc, char *argv[] );
	void md( int argc, char *argv[] );
	void rd( int argc, char *argv[] );
	void cd( int argc, char *argv[] );
	void sort( int argc, char *argv[] );
	void more( int argc, char *argv[] );
	void help( int argc, char *argv[] );
	bool showPath();
	void saveHome();
protected:
	int listdir();
	int listdir( const char * );
	int listdir( bool , int );
	int listdir( const char*, bool, int );
	void catenate( int argc, char *argv[] );
	void catenate( int argc, char *argv[], int , bool  );
	void makeCopyDir( char copyfile[], char targetfile[], bool  );
	void makeCopyDir( char *argv[], bool  );
	void makeCopy( char *argv[], bool , bool  );
	void makeMove( int argc, char *argv[], int );

private:
	char path[PATH_SIZE];
	char homePath[PATH_SIZE];
};
