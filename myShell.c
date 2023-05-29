#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<pwd.h>
#include<time.h>
#include<wait.h>
#include<sys/types.h>
#include<errno.h>

static void getUsername();//得到当前用户名字 ok
static void getWorkdir();// 得到当前工作路径 ok
static int splitCommands(char *command);//分开指令 ok
static void callCd(int commandNum);//cd指令 ok
static void callExit();//exit指令 ok
static void callLs();//ls指令 ok
static void callHelp();//help指令 (等待补充)
static int callCommandWithPioe(int left,int right);//执行指令中带有管道 ok
static int callCommand(int commandNum);//重定向，管道的uni ok
static int isCommandExist(const char* command);//判断是都存在该指令 ok
static int callCommandWithRedi(int left, int right);//执行重定位指令 ok
static void callHello();//ok
static void callDirs();// 显示当前工作路径 ok
static void callTimes();//显示当前时间 ok

char command[BUFSIZ];//当前的指令 ok
char username[BUFSIZ];//用户名字 ok 
char *hostname="QWQ";//主机名字 ok 
char workdir[BUFSIZ];//当前工作路径 ok 
char commands[100][100];//记录指令分割 ok


int main(){

	getWorkdir();
	getUsername();
	while(1)
	{
		printf("\e[33;1m%s@%s:%s\e[0m$ ",username,hostname,workdir);//黄色
		fgets(command,100,stdin);
		int l=strlen(command);
		command[l-1]='\0';
		int num = splitCommands(command);
		//printf("%s",commands[0]);
		if(!strcmp(commands[0],"exit"))
		{
			callExit();
		}
		else if(!(strcmp(commands[0],"cd")))
		{
			callCd(num);
		}
		else if(!(strcmp(commands[0],"ls")))
		{
			callLs();
		}
		else if(!(strcmp(commands[0],"help")))
		{
			callHelp();
		}
		else if(!(strcmp(commands[0],"hello")))
		{
			callHello();
		}
		else if(!(strcmp(commands[0],"dirs")))
		{
			callDirs();
		}
		else if(!(strcmp(commands[0],"times")))
		{
			callTimes();
		}
		else
		{
			int reslut = callCommand(num);
			switch (reslut)
			{
			case -1:
				fprintf(stderr,"\e[31;1mError: Fork error.\n\e[0m");
				break;
			case -3:
				fprintf(stderr,"\e[31;1mError: Miss pipe parameters.\n\e[0m");
				break;
			case -4:
				fprintf(stderr,"\e[31;1mError: Open pipe error.\n\e[0m");
				break;
			case -5:
				fprintf(stderr,"\e[31;1mError: Command not exist in mtshell.\n\e[0m");
				break;
			case -6:
				fprintf(stderr,"\e[31;1mError: Miss redirect file parameters.\n\e[0m");
				break;
			case -7:
				fprintf(stderr,"\e[31;1mError: Input redirection file not exist.\n\e[0m");\
				break;
			case -8:
				fprintf(stderr,"\e[31;1mError: Too many redirection symbol.\"%s\".n\e[0m","<");
				break;
			case -9:
			    fprintf(stderr,"\e[31;1mError: Too many redirection symbol.\"%s\".n\e[0m",">");
				break;
			}
		}
	}
	
	return 0;
}



void getUsername()//得到用户名
{
	struct passwd* pwd = getpwuid(getuid());
	strcpy(username,pwd->pw_name);
}

void getWorkdir()
{
	getcwd(workdir,BUFSIZ);
}

int splitCommands(char *command)
{
	int num = 0,i,j;
	int len = strlen(command);
	for(i=0,j=0;i<len;++i)
	{
		if(command[i]!=' '){
			commands[num][j++] = command[i];
		}
		else{
			if(j!=0)
			{
				commands[num][j] = '\0';
				++num;
				j=0;
			}
		}
	}
	if(j!=0)
	{
		commands[num][j] = '\0';
		++num;
	}
	return num;
}

void callExit()
{
	exit(-1);
}

void callCd(int commandNum){
	if(commandNum<2)
	{
		fprintf(stderr,"\e[31;1mError: Miss parameter while using command \"cd\".\n\e[0");return;//只有cd没有路径
	}
	else if(commandNum>2)
	{
		fprintf(stderr,"\e[31;1mError: Too many parameter while using command \"cd\".\n\e[0");return;//路径太多
	}
	else
	{
		int ret = chdir(commands[1]);
		getWorkdir();
		if(ret)
		{
			fprintf(stderr,"\e[31;1mError: No such path \"%s\".\n\e[0",commands[1]);return;//没有该路径
		}
		return;
	}
}

void callLs()
{
	DIR *dirp;
	struct dirent *dp;
	dirp = opendir(".");//打开当前目录
	while ((dp = readdir(dirp))!=NULL)
	{
		printf("%s\n",dp->d_name);//输出目录指针下的文件
	}
	(void) closedir(dirp);
	return;
}

void callHello()
{
	printf("Hello is myshell\n");
}

void callDirs()
{
	char dirs[BUFSIZ];
	getcwd(dirs,BUFSIZ);
	printf("%s\n",dirs);
}

void callTimes()
{
	time_t t;
	struct tm * lt;
	time (&t);//获取时时间戳
	lt = localtime(&t); //转换为时间结构
	printf("%d/%d/%d %d:%d:%d\n",lt->tm_year+1900,lt->tm_mon,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	
}

void callHelp()
{
	printf("dongzeyu 202100202058\n");
	printf("命令\t功能\n");
	printf("cd\t读取文档\n");
	printf("dirs\t显示当前工作路径\n");
	printf("ls\t显示当前文件下所有文件\n");
	printf("times\t显示当前系统时间\n");
	printf("help\t显示帮助目录\n");
	printf("|\t管道\n");
	printf("hello\thello指令\n");
	printf("exit\t退出shell\n");
}

int callCommand(int commandNum) { // 给用户使用的函数，用以执行用户输入的命令
	pid_t pid = fork();
	if (pid == -1) {
		return -1;
	} else if (pid == 0) {
		/* 获取标准输入、输出的文件标识符 */
		int inFds = dup(STDIN_FILENO);
		int outFds = dup(STDOUT_FILENO);

		int result = callCommandWithPipe(0, commandNum);
		
		/* 还原标准输入、输出重定向 */
		dup2(inFds, STDIN_FILENO);
		dup2(outFds, STDOUT_FILENO);
		exit(result);
	} else {
		int status;
		waitpid(pid, &status, 0);
		return WEXITSTATUS(status);
	}
}

int callCommandWithPipe(int left, int right) { // 所要执行的指令区间[left, right)，可能含有管道
	if (left >= right) return 0;
	/* 判断是否有管道命令 */
	int pipeIdx = -1;
	for (int i=left; i<right; ++i) {
		if (strcmp(commands[i], "|") == 0) {
			pipeIdx = i;
			break;
		}
	}
	if (pipeIdx == -1) { // 不含有管道命令
		return callCommandWithRedi(left, right);
	} else if (pipeIdx+1 == right) { // 管道命令'|'后续没有指令，参数缺失
		return -3;
	}

	/* 执行命令 */
	int fds[2];
	if (pipe(fds) == -1) {
		return -4;
	}
	int result = 0;
	pid_t pid = vfork();
	if (pid == -1) {
		result = -1;
	} else if (pid == 0) { // 子进程执行单个命令
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO); // 将标准输出重定向到fds[1]
		close(fds[1]);
		
		result = callCommandWithRedi(left, pipeIdx);
		exit(result);
	} else { // 父进程递归执行后续命令
		int status;
		waitpid(pid, &status, 0);
		int exitCode = WEXITSTATUS(status);
		
		if (exitCode != 0) { // 子进程的指令没有正常退出，打印错误信息
			char info[4096] = {0};
			char line[BUFSIZ];
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO); // 将标准输入重定向到fds[0]
			close(fds[0]);
			while(fgets(line, BUFSIZ, stdin) != NULL) { // 读取子进程的错误信息
				strcat(info, line);
			}
			printf("%s", info); // 打印错误信息
			
			result = exitCode;
		} else if (pipeIdx+1 < right){
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO); // 将标准输入重定向到fds[0]
			close(fds[0]);
			result = callCommandWithPipe(pipeIdx+1, right); // 递归执行后续指令
		}
	}

	return result;
}

int callCommandWithRedi(int left, int right) { // 所要执行的指令区间[left, right)，不含管道，可能含有重定向
	if (!isCommandExist(commands[left])) { // 指令不存在
		return -5;
	}	

	/* 判断是否有重定向 */
	int inNum = 0, outNum = 0;
	char *inFile = NULL, *outFile = NULL;
	int endIdx = right; // 指令在重定向前的终止下标

	for (int i=left; i<right; ++i) {
		if (strcmp(commands[i], "<") == 0) { // 输入重定向
			++inNum;
			if (i+1 < right)
				inFile = commands[i+1];
			else return -6; // 重定向符号后缺少文件名

			if (endIdx == right) endIdx = i;
		} else if (strcmp(commands[i], ">") == 0) { // 输出重定向
			++outNum;
			if (i+1 < right)
				outFile = commands[i+1];
			else return -6; // 重定向符号后缺少文件名
				
			if (endIdx == right) endIdx = i;
		}
	}
	/* 处理重定向 */
	if (inNum == 1) {
		FILE* fp = fopen(inFile, "r");
		if (fp == NULL) // 输入重定向文件不存在
			return -7;
		
		fclose(fp);
	}
	
	if (inNum > 1) { // 输入重定向符超过一个
		return -8;
	} else if (outNum > 1) { // 输出重定向符超过一个
		return -9;
	}

	int result = 0;
	pid_t pid = vfork();
	if (pid == -1) {
		result = -1;
	} else if (pid == 0) {
		/* 输入输出重定向 */
		if (inNum == 1)
			freopen(inFile, "r", stdin);
		if (outNum == 1)
			freopen(outFile, "w", stdout);

		/* 执行命令 */
		char* comm[BUFSIZ];
		for (int i=left; i<endIdx; ++i)
			comm[i] = commands[i];
		comm[endIdx] = NULL;
		execvp(comm[left], comm+left);
		exit(errno); // 执行出错，返回errno
	} else {
		int status;
		waitpid(pid, &status, 0);
		int err = WEXITSTATUS(status); // 读取子进程的返回码

		if (err) { // 返回码不为0，意味着子进程执行出错，用红色字体打印出错信息
			printf("\e[31;1mError: %s\n\e[0m", strerror(err));
		}
	}


	return result;
}


int isCommandExist(const char* command) { // 判断指令是否存在
	if (command == NULL || strlen(command) == 0) return 0;

	int result = 1;
	int fds[2];
	if (pipe(fds) == -1) {
		result = 0;
	} else {
		/* 暂存输入输出重定向标志 */
		int inFd = dup(STDIN_FILENO);
		int outFd = dup(STDOUT_FILENO);

		pid_t pid = vfork();
		if (pid == -1) {
			result = 0;
		} else if (pid == 0) {
			/* 将结果输出重定向到文件标识符 */
			close(fds[0]);
			dup2(fds[1], STDOUT_FILENO);
			close(fds[1]);

			char tmp[BUFSIZ];
			sprintf(tmp, "command -v %s", command);
			system(tmp);
			exit(1);
		} else {
			waitpid(pid, NULL, 0);
			/* 输入重定向 */
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);

			if (getchar() == EOF) { // 没有数据，意味着命令不存在
				result = 0;
			}
			
			/* 恢复输入、输出重定向 */
			dup2(inFd, STDIN_FILENO);
			dup2(outFd, STDOUT_FILENO);
		}
	}

	return result;
}