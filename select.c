#include <sys/type.h>
#include <sys/time.h>

int main(int argc, char const *argv[])
{
	fd_set ensemble_lecture;
	s1 = socket();
	s2 = socket();
	s3 = socket();
	FD_ZERO(&ensemble_lecture);
	FD_SET(s1,&ensemble_lecture);
	FD_SET(s2,&ensemble_lecture);
	FD_SET(s3,&ensemble_lecture);
	int res = select(s3+1,&ensemble_lecture,NULL,NULL,)
	return 0;
}