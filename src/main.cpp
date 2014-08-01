#include <iostream>
#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include <Command.h>
using namespace std;

const int ARGV_SIZE=20;
const int CMD_SIZE=128;

/*
 * 操作系统接口：兼容Unix/Linux命令接口。
 * 为Windows操作系统建立一个兼容Unix命令的命令接口
 * 实现命令包括ls, cat, cp, mv, md, rd, cd, sort, more, print
 * 可以字符形式接收命令，执行命令，然后显示命令执行结果。
 *
 *
 */

void initConsole()
{
	SetConsoleTitle( "Unix Console for Windows" );
	system( "color 5F" );
	printf( "\nUnix Console for Windows\nVersion: 1.0\nInput 'help' to get help\n\n" );
}



int main(int argc, char *argv[])
{
	initConsole();
	char cmd[CMD_SIZE];
	Command command;
	command.saveHome();

	///可以读取到当前路径，读入有效的命令就循环
	while( command.showPath() && cin.getline( cmd, CMD_SIZE ) )
	{
		///当cmd有长度时，分析输入的内容。如果没有长度，则仅有回车，继续显示路径即可
		if( strlen( cmd ) )
		{
			int argc=0;
			char *argv[ARGV_SIZE];

			///以空格分割cmd字符串
			argv[argc]=strtok( cmd, " " );

			while( argv[argc] )
			{
				//printf( "%s ", argv[argc] );
				argc++;
				argv[argc]=strtok( NULL, " " );
			}

			for( int i = 0; i < ARGV_SIZE; i++ )
				argv[0][i] = tolower( argv[0][i] );

			if( !strcmp( argv[0],"ls" ) )
				command.ls( argc, argv );
			else if( !strcmp( argv[0],"cat" ) )
				command.cat( argc, argv );
			else if( !strcmp( argv[0],"cp" ) )
				command.cp( argc, argv );
			else if( !strcmp( argv[0],"mv" ) )
				command.mv( argc, argv );
			else if( !strcmp( argv[0],"mkdir" ) )
				command.md( argc, argv );
			else if( !strcmp( argv[0],"rmdir" ) )
				command.rd( argc, argv );
			else if( !strcmp( argv[0],"cd" ) )
				command.cd( argc, argv );
			else if( !strcmp( argv[0],"sort" ) )
				command.sort( argc, argv );
			else if( !strcmp( argv[0],"more" ) )
				command.more( argc, argv );
			else if( !strcmp( argv[0],"help" ) )
				command.help( argc, argv );
			else if( !strcmp( argv[0],"clear" ) )
				system( "cls" );
            else if( !strcmp( argv[0],"date" ) )
				system( "date" );
			else
				printf( "'%s' is not recognized as an command.\n", argv[0] );

		}
	}

	return 0;
}
