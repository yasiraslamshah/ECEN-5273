Yasir Aslam Shah 
Programming Assignment3
Fall 2018

# HTTP Proxy Server
In this assignement, Webproxy is implemented as an intermediate entity called proxy.

The proxy is between HTTP client and HTTP server.
A Request is sent to proxy and the proxy forwards request to HTTP Server and recieves reply from HTTP server.

The zip contains the following:
1. PA31.c: C Code
2. makefile:   Make recipe for the code
3. BlockedList:List containing Blocked website

Start the proxy server, 
$make   #then,
$./PA31 <PORT> <TIMEOUT>

TIMEOUT is the threshold to keep a cache file in memory when the threshold is crossed, the cache is deleted

Set the proxy setting for the browser to manual with port as 8100


    1. Blocking websites – Block website 

    2. Caching – retrieve cache from memory in case file is present otherwise create a cache for the same

    3. Timeout Expiration - TIMEOUT is the threshold to keep a cache file in memory when the threshold is crossed, the cache is deleted


