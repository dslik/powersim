# sets 1840A SNON values
require "json"
require "socket"

# Get SNON data
print "Connecting to SNON device\n";
socket = TCPSocket.open("192.168.88.101", "200");
socket.print("\r\n");
print socket.gets;
socket.print('{"eID":"urn:uuid:254CB903-3406-5B2A-A6DD-9DEA8A0DB148","v":["vita40_amber_steady"]}');
print socket.gets;
socket.print("\r\n");
print socket.gets;
print socket.gets;
socket.close;