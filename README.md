# Project Malik
A webservice to validate that an IP is within a specific country. Written in C++
## Description
The purpose of the tool is to take a JSON object which contains and IP and an allow-list of countries and then returns a bool if the IP is located within those allowed countries.

Example JSON body Request: {"IP":"8.8.8.8", "allow-list":"USA, Japan"}<br />
Example JSON body Response: {"IP-Allowed":true}

The tool utilizes country mappings from https://dev.maxmind.com/geoip/geoip2/geolite2/ <br />
It also provides caching for faster response times.
