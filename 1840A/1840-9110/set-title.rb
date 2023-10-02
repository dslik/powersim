# sets 1840A SNON values
require "json"
require "socket"

# Get SNON data
print "Connecting to SNON device\n";
socket = TCPSocket.open("192.168.88.101", "200");
socket.print('{"eID":"urn:uuid:4D6FF230-27FF-5E0B-9CE6-704E09D5241F","v":["' + ARGV[0] + '"]}');
socket.print("\r");
print socket.gets;
socket.close;