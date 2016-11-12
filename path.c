#include <string.h>
#include <unistd.h>

void get_real(char *directory, char *ab_path)
{
	if (directory[0] == '/')
	{
		strcpy(ab_path, directory);
		return;
	}
	getcwd(ab_path, 1024);
	unsigned cnt = 0;
	size_t i = 0;
	while (directory[i])
	{		
		if (directory[i] == '.')
			++cnt;
		else if (directory[i] == '/')
			cnt = 0;
		else
			break;
		if (cnt == 2)
		{
			while (ab_path[strlen(ab_path) - 1] != '/')
				ab_path[strlen(ab_path) - 1] = '\0';
			ab_path[strlen(ab_path) - 1] = '\0';
			cnt = 0;
		}
		++i;
	}
	if (strlen(directory + i) > 0)
		strcat(ab_path, "/");
	strcat(ab_path, directory + i);
}
