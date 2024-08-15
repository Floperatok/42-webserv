#!/usr/bin/env python3

import cgi

def main():
    print("Content-type: text/html\r\n")
    print("<html><head><title>CGI example in Python</title></head>")
    print("<body>")
    print("Python CGIs are well running on Webserv!")
    print("</body></html>")

if __name__ == "__main__":
    main()
