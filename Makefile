
peer-server:
	python3 peer_server.py -p 58000

peer:
	python3 p2pnet.py -s 127.0.0.1 -p 58000 -l 5000 -n 3 -h 4

peer2:
	python3 p2pnet.py -s 127.0.0.1 -p 58000 -l 5001 -n 3 -h 4

peer3:
	python3 p2pnet.py -s 127.0.0.1 -p 58000 -l 5002 -n 3 -h 4

.PHONY: peer-server peer peer2 peer3
