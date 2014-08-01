#include "Command.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
typedef struct _FILE_INFO
{
	char cFileName[PATH_SIZE]; //文件的标题名
	DWORD dwFileAttributes; //文件的属性
	FILETIME ftLastWriteTime; //文件的最后修改时间
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
} FILE_INFO, * PFILE_INFO;

Command::Command()
{
	//ctor
}

Command::~Command()
{
	//dtor
}
void Command::saveHome()
{
	char cwd[PATH_SIZE];

	if( getcwd( cwd, sizeof( cwd ) ) )
	{
		strcpy( Command::homePath, cwd );
	}
}
bool Command::showPath()
{
	char cwd[PATH_SIZE];

	if( getcwd( cwd, sizeof( cwd ) ) )
	{
		DWORD mSize=20;
		char user[mSize];

		if( GetUserName( user, &mSize ) )
			printf( "%s@",user );

		printf( "%s\n# ", cwd );
		strcpy( Command::path, cwd );
		return true;
	}
	else
	{
		printf( "Error: Failed to Get Current Work Path\n" );
		return false;
	}
}
int fileInfoCmp( const void *a , const void *b )
{
	FILETIME fa=( *( FILE_INFO * )a ).ftLastWriteTime;
	FILETIME fb=( *( FILE_INFO * )b ).ftLastWriteTime;

	if( fa.dwHighDateTime<fb.dwHighDateTime )
		return 1;
	else if( fa.dwHighDateTime>fb.dwHighDateTime )
		return -1;
	else if( fa.dwLowDateTime<fb.dwLowDateTime )
		return 1;
	else return -1;
}
int Command::listdir()
{
	return listdir( path );
}
int Command::listdir( const char *name )
{
	return listdir( name, false, 0 );
}
int Command::listdir( bool includeHidden, int orderType )
{
	return listdir( path, includeHidden, orderType );
}
int Command::listdir( const char *name, bool includeHidden, int orderType )
{

	DIR *dir;
	dir=opendir( name );

	if( dir!=NULL )
	{
		FILE_INFO FileInfo[FILE_COUNT];
		WIN32_FIND_DATA FindFileData[6500];
		int fileIndex=0;
		struct dirent *ptr=readdir( dir );
/*
		while( ptr!=NULL )
		{
			strcpy( FileInfo[fileIndex].cFileName, ptr->d_name );
			FindClose( FindFirstFile( ptr->d_name, &FindFileData ) );
			FileInfo[fileIndex].ftLastWriteTime=FindFileData.ftLastWriteTime;
			FileInfo[fileIndex].dwFileAttributes=FindFileData.dwFileAttributes;
			FileInfo[fileIndex].nFileSizeHigh=FindFileData.nFileSizeHigh;
			FileInfo[fileIndex].nFileSizeLow=FindFileData.nFileSizeLow;
			ptr=readdir( dir );
			fileIndex++;
		}

		closedir( dir );

		if( orderType==2 )
			///按最后修改时间倒序显示
		{
			qsort( FileInfo, fileIndex, sizeof( FILE_INFO ), fileInfoCmp );

			for( int i=0; i<fileIndex; i++ )
			{
				bool isHidden = ( ( FileInfo[i].dwFileAttributes&0x00000002 )==0x00000002 );

				if( includeHidden || !isHidden )
					printf( "%-20.18s", FileInfo[i].cFileName );
			}
		}
		else if( orderType==1 )
			///按字母倒序显示
		{
			for( int i=fileIndex-1; i>=0; i-- )
			{
				bool isHidden = ( ( FileInfo[i].dwFileAttributes&0x00000002 )==0x00000002 );

				if( includeHidden || !isHidden )
					printf( "%-20.18s", FileInfo[i].cFileName );
			}
		}
		else
			///默认的按字母序显示
		{
			for( int i=0; i<fileIndex; i++ )
			{
				bool isHidden = ( ( FileInfo[i].dwFileAttributes&0x00000002 )==0x00000002 );

				if( includeHidden || !isHidden )
					printf( "%-20.18s", FileInfo[i].cFileName );
			}
		}

		printf( "\n" );*/
		return 0;
	}
	else
	{
		printf( "ls: Can Not Find the %s Directory.\n", name );
		return -1;
	}

}

/**
* ls命令
* 可接受的选项为-a -r -t， 参数为相对目录名
**/
void Command::ls( int argc, char *argv[] )
{
	if( argc==1 )
	{
		listdir();
	}
	else
	{
		int opt=0;
		bool legalCmd=true;
		bool includeHidden=false;
		int orderType=0;
		opterr=0;
		opt=getopt( argc, argv, "art" );

		while( opt!=-1 )
		{
			switch( opt )
			{
			case 'a':
				includeHidden=true;
				break;

			case 'r':
				orderType=1;
				break;

			case 't':
				orderType=2;
				break;

			default:
				printf( "ls: Can Not Find the Option(s).\n" );
				legalCmd=false;
				break;
			}

			opt=getopt( argc, argv, "art" );
		}

		if( legalCmd )
		{
			if( argv[optind] )
			{
				listdir( argv[optind], includeHidden, orderType );
			}
			else
			{
				listdir( includeHidden, orderType );
			}
		}

		optind=0;
	}
}
void Command::catenate( int argc, char *argv[] )
{
	catenate( argc, argv, 0, false );
}
void Command::catenate( int argc, char *argv[], int numberType, bool squeezeBlank )
{
	int lineNum=1;
	bool lastIsSqueeze=false;

	for( int i=0; i<argc; i++ )
	{
		char filename[PATH_SIZE];
		strcpy( filename, argv[i] );
		std::ifstream in( filename );

		if( !in )
		{
			printf( "more: Failed to Open File.\n" );
			return;
		}

		char line[FILE_BUF];

		while( ! in.eof() )
		{
			if( in.getline( line, FILE_BUF ) )
			{
				///如果上一行为空行，本行也是空行，且有空行合并需求，不输出
				if( lastIsSqueeze && squeezeBlank && line[0]=='\0' ) {}
				else
				{
					if( numberType>0 )
					{
						///如果输出所有行号，或者不是空行
						if( line[0]!='\0' || numberType==1 )
						{
							printf( "%3d | ",lineNum );
							lineNum++;
						}
						else
							printf( "    | " );


					}

					if( line[0]=='\0' )
						lastIsSqueeze=true;
					else
						lastIsSqueeze=false;

					printf( "%s\n", line );
				}

			}
		}

		in.close();

	}

}

/**
* cat命令
* 可接受的选项为-n -b -s， 参数为各文件名
**/
void Command::cat( int argc, char *argv[] )
{
	if( argc==1 )
	{
		printf( "cat: No Input File Given.\n" );
		return;
	}
	else
	{
		int opt=0;
		bool legalCmd=true;
		int numberType=0;
		bool squeezeBlank=false;

		opterr=0;
		opt=getopt( argc, argv, "nbs" );

		while( opt!=-1 )
		{
			switch( opt )
			{
			case 'n':
				numberType=1;
				break;

			case 'b':
				numberType=2;
				break;

			case 's':
				squeezeBlank=true;
				break;

			default:
				printf( "cat: Can Not Find the Option(s).\n" );
				legalCmd=false;
				break;
			}

			opt=getopt( argc, argv, "nbs" );
		}

		if( legalCmd )
		{
			if( argv[optind] )
			{
				///将几个文件名字符指针重新组合到新指针数组中，向函数传入参数
				char **file_argv=( char ** )malloc( ( argc-optind-1 )*sizeof( char* ) );
				int file_argc;

				for( file_argc=0; argv[optind]; optind++, file_argc++ )
					file_argv[file_argc]=argv[optind];

				catenate( file_argc, file_argv, numberType, squeezeBlank );

			}
			else
			{
				printf( "cat: No Input File Given.\n" );
				return;
			}
		}

		optind=0;
	}
}

void Command::makeCopyDir( char copyfile[], char targetfile[], bool needCheck )
{
	char *argv[2];
	argv[0]=copyfile;
	argv[1]=targetfile;
	makeCopyDir( argv, needCheck );
}

void Command::makeCopyDir( char *argv[2], bool needCheck )
{
	DIR *dir;
	dir=opendir( argv[0] );

	if( dir!=NULL )
	{
		struct dirent *ptr=readdir( dir );

		while( ptr!=NULL )
		{
			bool status;
			char fileName[PATH_SIZE];
			char targetName[PATH_SIZE];

			if( strcmp( ptr->d_name, "." )!=0 && strcmp( ptr->d_name, ".." )!=0 )
			{
				WIN32_FIND_DATA FindFileData;
				strcpy( fileName, argv[0] );
				strcat( fileName, "\\" );
				strcat( fileName, ptr->d_name );
				strcpy( targetName, argv[1] );
				strcat( targetName, "\\" );
				strcat( targetName, ptr->d_name );
				FindClose( FindFirstFile( fileName, &FindFileData ) );

				///如果是目录
				if( ( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )==FILE_ATTRIBUTE_DIRECTORY )
				{
					///有必要先新建文件夹
					mkdir( targetName );
					makeCopyDir( fileName, targetName, needCheck );
				}
				else
				{
					status=CopyFile( fileName, targetName, needCheck );

					if( !status && needCheck )
					{
						char ch;
						printf( "cp: Existing the Same Name File %s, Press y to Overwrite It.\n", targetName );
						ch=getch();

						if( ch=='Y' or ch=='y' )
							status=CopyFile( fileName, targetName, false );

						if( !status )
							printf( "cp: Failed to Copy File %s.\n" , fileName );
					}
					else if( !status )
						printf( "cp: Failed to Copy File %s.\n", fileName );
				}
			}

			ptr=readdir( dir );
		}

		closedir( dir );
	}
}

void Command::makeCopy( char *argv[2], bool needCheck, bool copyDir )
{
	if( !copyDir )
	{
		bool status;
		status=CopyFile( argv[0], argv[1], needCheck );

		if( !status && needCheck )
		{
			char ch;
			printf( "cp: Existing the Same Name File, Press y to Overwrite it.\n" );
			ch=getch();

			if( ch=='Y' or ch=='y' )
				status=CopyFile( argv[0], argv[1], false );

			if( !status )
				printf( "cp: Failed to Copy File.\n" );
		}
		else if( !status )
			printf( "cp: Failed to Copy File.\n" );
	}
	///文件夹拷贝
	else
	{
		makeCopyDir( argv, needCheck );
	}
}

/**
* cp命令
* 可接受的选项为-i -R， 参数为各文件名
**/
void Command::cp( int argc, char *argv[] )
{
	if( argc<3 )
	{
		printf( "cp: Not 2 Files or Directories Given.\n" );
		return;
	}
	else
	{
		int opt=0;
		bool legalCmd=true;
		bool needCheck=false;
		bool copyDir=false;

		opterr=0;
		opt=getopt( argc, argv, "iRr" );

		while( opt!=-1 )
		{
			switch( opt )
			{
			case 'i':
				needCheck=true;
				break;

			case 'R':
			case 'r':
				copyDir=true;
				break;

			default:
				printf( "cat: Can not Find the Option(s).\n" );
				legalCmd=false;
				break;
			}

			opt=getopt( argc, argv, "iRr" );
		}

		if( legalCmd )
		{
			if( argv[optind] && argv[optind+1] && optind==argc-2 )
			{
				char *file_argv[2];
				file_argv[0]=argv[optind];
				file_argv[1]=argv[optind+1];
				makeCopy( file_argv, needCheck, copyDir );
			}
			else
			{
				printf( "cp: Not 2 Files or Directories Given.\n" );
				return;
			}
		}

		optind=0;

	}
}

void Command::makeMove( int argc, char *argv[], int replaceWay )
{
	if( replaceWay==2 )
	{
		for( int i=0; i<argc-1; i++ )
		{
			MoveFileEx( argv[i], argv[argc-1], MOVEFILE_REPLACE_EXISTING );
		}
	}
	else if( replaceWay==1 )
	{
		for( int i=0; i<argc-1; i++ )
		{
			MoveFile( argv[i], argv[argc-1] );
		}
	}
	else
	{
		for( int i=0; i<argc-1; i++ )
		{
			bool status;
			status=MoveFile( argv[i], argv[argc-1] );

			if( !status )
			{
				char ch;
				DWORD error=GetLastError();

				if( error==80 || error==183 )
				{
					printf( "mv: Existing the Same Name File, Press y to Overwrite It.\n" );
					ch=getch();

					if( ch=='Y' or ch=='y' )
						status=MoveFileEx( argv[i], argv[argc-1], MOVEFILE_REPLACE_EXISTING );

					if( !status )
						printf( "mv: Failed to Move File.\n" );
				}
				else
					printf( "mv: Failed to Move File.\n" );
			}
		}
	}

}

void Command::mv( int argc, char *argv[] )
{
	if( argc<3 )
	{
		printf( "cp: Not Enough Files or Directories Given.\n" );
		return;
	}
	else
	{
		int opt=0;
		bool legalCmd=true;
		int replaceWay=0;

		opterr=0;
		opt=getopt( argc, argv, "inf" );

		while( opt!=-1 )
		{
			switch( opt )
			{
			case 'i':
				replaceWay=0;
				break;

			case 'n':
				replaceWay=1;
				break;

			case 'f':
				replaceWay=2;
				break;

			default:
				printf( "cat: Can not Find the Option(s).\n" );
				legalCmd=false;
				break;
			}

			opt=getopt( argc, argv, "inf" );
		}

		if( legalCmd )
		{
			if( argv[optind] && argv[optind+1] )
			{
				///将几个文件名字符指针重新组合到新指针数组中，向函数传入参数
				char **file_argv=( char ** )malloc( ( argc-optind-1 )*sizeof( char* ) );
				int file_argc;

				for( file_argc=0; argv[optind]; optind++, file_argc++ )
					file_argv[file_argc]=argv[optind];

				makeMove( file_argc, file_argv, replaceWay );
			}
			else
			{
				printf( "cp: Not Enough Files or Directories Given.\n" );
				return;
			}
		}

		optind=0;

	}
}


void Command::md( int argc, char *argv[] )
{
	char dirname[PATH_SIZE];

	if( argc==1 )
	{
		printf( "mkdir: No Directory Input.\n" );
		return;
	}
	else if( argc==2 )
	{
		strcpy( dirname , argv[1] );

		if( mkdir( dirname )==-1 )
		{
			printf( "mkdir: Fail to Creat the Directory." );
			return;
		}

	}
	else
		printf( "mkdir: Too Many Options.\n" );
}

void Command::rd( int argc, char *argv[] )
{
	char dirname[PATH_SIZE];

	if( argc==1 )
	{
		printf( "rmdir: No Directory Input.\n" );
		return;
	}
	else if( argc==2 )
	{
		strcpy( dirname , argv[1] );

		if( rmdir( dirname )==-1 )
		{
			printf( "rmdir: Failed to Remove The Directory." );
			return;
		}

	}
	else
		printf( "rmdir: Too Many Options.\n" );
}

void Command::cd( int argc, char *argv[] )
{
	char dirname[PATH_SIZE];

	if( argc==1 )
	{
		strcpy( dirname , homePath );
	}
	else if( argc==2 )
	{
		strcpy( dirname , argv[1] );
	}
	else
	{
		printf( "cd: Too Many Options.\n" );
		return;
	}

	if( ( access( dirname, 0 )==-1 ) || ( chdir( dirname )!=0 ) )
		printf( "cd: Failed to Find This Directory.\n" );
	else
	{
		char cwd[PATH_SIZE];

		if( !getcwd( cwd, PATH_SIZE ) )
			printf( "cd: Failed to Find This Directory.\n" );
	}

}

void Command::sort( int argc, char *argv[] )
{
	char filename[PATH_SIZE];

	if( argc==1 )
	{
		printf( "sort: No Input File Given.\n" );
		return;
	}
	else if( argc==2 )
	{
		strcpy( filename , argv[1] );
		std::string line;
		std::vector<std::string> content;
		std::ifstream in( filename );

		if( !in )
		{
			printf( "sort: Failed to Find This File.\n" );
			return;
		}

		while( in >> line ) content.push_back( line );

		stable_sort( content.begin(), content.end() );

		for( std::vector<std::string>::iterator it = content.begin(); it != content.end(); it++ )
		{
			printf( "%s\n",it->c_str() );
		}

		in.close();

	}
	else
		printf( "sort: Too Many Options.\n" );
}

void Command::more( int argc, char *argv[] )
{
	if( argc==1 )
	{
		printf( "more: No Input File Given.\n" );
		return;
	}
	else if( argc==2 )
	{
		int filesize=0;
		int current=0;
		char filename[PATH_SIZE];
		char page[FILE_BUF];
		int pageNum=1;
		strcpy( filename, argv[1] );
		///ate 初始位置为文件尾; binary 二进制
		std::ifstream in( filename, std::ifstream::ate | std::ifstream::binary );

		if( !in )
		{
			printf( "more: Fail to Open File.\n" );
			return;
		}

		///文件尺寸=末尾指针-1（除去'\0'）
		filesize = in.tellg();
		filesize--;
		///重新定位文件流
		in.seekg( 0, std::ifstream::beg );

		while( true )
		{
			printf( "\n\n====================%-20s[page: %d/%d]====================\n", filename, pageNum, filesize/FILE_BUF+1 );
			in.read( page, FILE_BUF-1 );
			current += in.gcount();
			page[in.gcount()] = '\0';
			printf( "%s\n", page );
			printf( "Proportion:%d/%d=====================================================\n\n" , current , filesize );
			pageNum++;

			///退出循环的条件
			if( in.get() && ! in.eof() )
				system( "pause" );
			else
				break;
		}

		in.close();
	}
	else
	{
		printf( "more: Too Many Options.\n" );
		return;
	}
}
void Command::help( int argc, char *argv[] )
{
	printf( "Unix Console for Windows\nVersion: 1.0\n下面是可以使用的命令及可用的参数\n" );
	printf( "ls: 显示当前目录，如果跟随选项，显示选项指定的目录\n\t[-a]显示包括隐藏项目的所有项目 [-r]逆序排列 [-t]根据修改时间排序\n" );
	printf( "cat: 把选项对应各文件连在一起，然后输出到屏幕上\n\t[-b]对非空输出行编号 [-n]对输出的所有行编号 [-s]不输出多行空行 [文件]\n" );
	printf( "cp: 拷贝选项0文件到选项1\n\t[-i]覆盖前询问 [-r,-R]递归复制目录及其子目录的所有内容\n" );
	printf( "mv: 移动选项0文件到选项1\n\t[-f]覆盖前不询问 [-i]覆盖前询问 [-n]不覆盖已存在文件\n" );
	printf( "mkdir: 新建目录\n" );
	printf( "rmdir: 删除目录\n" );
	printf( "cd: 设置当前路径\n" );
	printf( "sort: 对文件进行排序\n" );
	printf( "more: 逐页显示文件内容到屏幕上\n" );
	printf( "clear: 清屏\n" );
	printf( "date: 显示日期\n" );
}
