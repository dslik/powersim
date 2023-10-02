# sets 1840A SNON values
require "json"
require "socket"

# Get SNON data
print "Connecting to SNON device\n";
socket = TCPSocket.open("192.168.88.101", "200");
socket.print('{"eID":"urn:uuid:26FB922F-1B3D-526E-B9B4-A2BB0BA6F06F","v":["' + ARGV[0] + '"]}');
socket.print("\r");
print socket.gets;
socket.print('{"eID":"urn:uuid:4C0A29C4-8DC2-5766-8E3C-406B8F02973C","v":["' + ARGV[1] + '"]}');
socket.print("\r");
print socket.gets;
socket.print('{"eID":"urn:uuid:623B4CD2-B2C8-5F3A-BA7C-A53F0EF9299D","v":["' + ARGV[2] + '"]}');
socket.print("\r");
print socket.gets;
socket.close;