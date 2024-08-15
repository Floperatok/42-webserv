#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("Content-type: text/html\r\n");
    printf("\r\n");

    printf("<html><head><title>CGI example in C</title></head>\n");
    printf("<body>");
    printf("C CGIs are well running on Webserv!");
    printf("</body>\n</html>");

    return 0;
}
